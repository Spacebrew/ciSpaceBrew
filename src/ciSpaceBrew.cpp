//
//  ciSpaceBrew.cpp
//  EchoClientApp
//
//  Created by Ryan Bartley on 5/11/13.
//
//

#include "ciSpaceBrew.h"

using namespace std;

namespace cinder {
    namespace Spacebrew {

    #pragma mark Message
        
        string Message::getJSON( const string &configName ) const
        {
            if ( type == "string" || type == "boolean" ){
                return "{\"message\":{\"clientName\":\"" + configName + "\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":\"" + value +"\"}}";
            } else {
                return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":" + value +"}}";
            }
        }
        
        bool Message::valueBoolean() const
        {
            if (type != "boolean") cerr << "This Message is not a boolean type! You'll most likely get 'false'." << endl;
            return value == "true";
        }
        
        int Message::valueRange() const
        {
            if ( type != "range" ) cerr << "This Message is not a range type! Results may be unpredicatable." << endl;
            return atoi(value.c_str());
        }
        
        string Message::valueString() const
        {
            if (type != "string") cerr << "This Message is not a string type! Returning raw value as string." << endl;
            return value;
        }
        
    #pragma mark Config
        
        void Config::addSubscribe( const string &name, const string &type )
        {
            subscribe.push_back( Message( name, type ) );
        }
        
        void Config::addSubscribe( const Message &m )
        {
            subscribe.push_back(m);
        }
        
        void Config::addPublish( const string &name, const string &type, const string &def )
        {
            publish.push_back( Message( name, type, def ) );
        }
        
        void Config::addPublish( const Message &m )
        {
            publish.push_back(m);
        }
        
        string Config::getJSON()
        {
            string message = "{\"config\": {\"name\": \"" + name +"\",\"description\":\"" + description +"\",\"publish\": {\"messages\": [";
            
            
            for (vector<Message>::iterator it = publish.begin(); it < publish.end(); it++){
                message += "{\"name\":\"" + it->name + "\",";
                message += "\"type\":\"" + it->type + "\",";
                message += "\"default\":\"" + it->value + "\"";
                message += "}";
                if ( (it + 1) < publish.end() ){
                    message += ",";
                }
            }
            
            message += "]},\"subscribe\": {\"messages\": [";
            
            for (vector<Message>::iterator it = subscribe.begin(); it < subscribe.end(); it++){
                message += "{\"name\":\"" + it->name + "\",";
                message += "\"type\":\"" + it->type + "\"";
                message += "}";
                if ( (it + 1) < subscribe.end() ){
                    message += ",";
                }
            }
            
            message += "]}}}";
            
            return message;
        }
        
    #pragma mark Connection
        
        Connection::Connection( cinder::app::App * app, const std::string& host, const std::string& name, const std::string& description )
        : app( app ),
            host( "ws://" + host + ":" + to_string(SPACEBREW_PORT) ),
            bConnected( false ),
            reconnectInterval( 2000 ),
            bAutoReconnect( false ),
            lastTimeTriedConnect( 0 ),
            config( Config( name, description ) ),
			client( NULL )
        {
			initialize();
        }
        
        Connection::~Connection()
        {
			// Disconnect update signal:
            updateConnection.disconnect();
			// Delete previous ws client object, if necessary:
			if( client != NULL ) { delete client; }
        }
		
		void Connection::initialize()
		{
			// Delete previous ws client object, if necessary:
			if( client != NULL ) { delete client; }
			// Initialize ws client:
			client = new WebSocketClient();
			// Setup callbacks:
			updateConnection = this->app->getSignalUpdate().connect( std::bind( &Connection::update, this ) ) ;
			//TODO: See if we can add a listener here to websocketpp cinder to automate this
			//WebSocketPP Interface
			client->addConnectCallback( &Connection::onConnect, this );
			client->addDisconnectCallback( &Connection::onDisconnect, this );
			client->addErrorCallback( &Connection::onError, this );
			client->addInterruptCallback( &Connection::onInterrupt, this );
			client->addPingCallback( &Connection::onPing, this );
			client->addReadCallback( &Connection::onRead, this );
		}
		
        void Connection::update()
        {
            client->poll();

            if ( bAutoReconnect ) {
                if ( !bConnected && this->app->getElapsedSeconds() - lastTimeTriedConnect > reconnectInterval * 1e-3 ) {
					// Disconnect update signal:
					updateConnection.disconnect();
					// Re-initialize ws client:
					initialize();
					// Re-establish connection:
					client->connect( this->host );
					// Reset connection attempt time:
					lastTimeTriedConnect = this->app->getElapsedSeconds();
                }
            }
        }
        
        void Connection::connect()
        {
            client->connect( host );
        }
        
        void Connection::connect( const string &host, const Config &config )
        {
            this->host = "ws://" + host + ":" + to_string(SPACEBREW_PORT);
            this->config = config;
            
            client->connect( this->host );
        }
        
        void Connection::send( const string &name, const string &type, const string &value )
        {
            if (bConnected) {
                Message m( name, type, value );
                send(m);
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::sendString( const string &name, const string &value )
        {
            if (bConnected) {
                Message m( name, "string", value );
                send(m);
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::sendRange( const string &name, int value )
        {
            if ( bConnected ) {
                Message m( name, "range", to_string(value));
                send(m);
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::sendBoolean( const string &name, bool value )
        {
            if (bConnected) {
                string out = value ? "true" : "false";
                Message m( name, "boolean", out );
                send(m);
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::send( const Message &m )
        {
            if ( bConnected ) {
                client->write( m.getJSON( config.name ) );
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::send( Message* m )
        {
            if ( bConnected ) {
                client->write( m->getJSON( config.name ) );
            } else {
                cerr << "Send failed, not connected!" << endl;
            }
        }
        
        void Connection::addSubscribe( const string &name, const string &type )
        {
            config.addSubscribe( name, type );
            if ( bConnected ) {
                updatePubSub();
            }
        }
        
        void Connection::addSubscribe( const Message &m )
        {
            config.addSubscribe(m);
            if ( bConnected ) {
                updatePubSub();
            }
        }
        
        void Connection::addPublish( const string &name, const string &type, const string &def)
        {
            config.addPublish( name, type, def );
            if ( bConnected ) {
                updatePubSub();
            }
        }
        
        void Connection::addPublish( const Message &m )
        {
            config.addPublish(m);
            if ( bConnected ) {
                updatePubSub();
            }
        }
        
        Config * Connection::getConfig()
        {
            return &config;
        }
        
        bool Connection::isConnected()
        {
            return bConnected;
        }
        
        void Connection::setAutoReconnect( bool _bAutoReconnect )
        {
            bAutoReconnect = _bAutoReconnect;
        }
        
        void Connection::setReconnectRate( int reconnectMillis )
        {
            reconnectInterval = reconnectMillis;
        }
        
        bool Connection::doesAutoReconnect()
        {
            return bAutoReconnect;
        }
        
        void Connection::updatePubSub()
        {
            client->write( config.getJSON() );
        }
        
        string Connection::getHost()
        {
            return host;
        }
        
        void Connection::onConnect(  )
        {
            bConnected = true;
            updatePubSub();
        }
        
        void Connection::onDisconnect(  )
        {
            bConnected = false;
            //TODO: Figure out the time
            lastTimeTriedConnect = this->app->getElapsedSeconds();
        }
        
        void Connection::onError( const string &err )
        {
            cout << "error: " << err << endl;
        }
        
        void Connection::onRead( const string &stuff )
        {
            Message m;
            Json::Value json;
            Json::Reader reader;
            if (reader.parse(stuff, json)) {
                
                m.name = json["message"]["name"].asString();
                m.type = json["message"]["type"].asString();
                
                if (m.type == "string" && json["message"]["value"].isString()) {
                    m.value = json["message"]["value"].asString();
                } else if (m.type == "boolean") {
                    if (json["message"]["value"].isInt()) {
                        m.value = json["message"]["value"].asInt();
                    } else if (json["message"]["value"].isString()) {
                        m.value = json["message"]["value"].asString();
                    }
                } else if (m.type == "range") {
                    if (json["message"]["value"].isInt()) {
                        m.value = json["message"]["value"].asInt();
                    } else if (json["message"]["value"].isString()) {
                        m.value = json["message"]["value"].asString();
                    }
                }
            }
            
            onMessage(m);
        }
        
        void Connection::onInterrupt()
        {
            
        }
        
        void Connection::onPing( const string &msg )
        {
            
        }
    
    }
    
    //Creating the Routes
}