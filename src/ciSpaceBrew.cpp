//
//  ciSpaceBrew.cpp
//  EchoClientApp
//
//  Created by Ryan Bartley on 5/11/13.
//
//

#include "ciSpaceBrew.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace ci::app;

namespace Spacebrew {

#pragma mark Message
	
Message::Message(const std::string &name, const std::string &type, const std::string &val)
: mName( name ), mType( type ), mValue( val )
{
}
	
Message::Message(const std::string &name, const std::string &type )
: mName( name ), mType( type )
{
}

Message::Message( const Message &other )
: mName( other.mName ), mType( other.mType), mValue( other.mValue )
{
}

Message::Message( Message &&other )
: mName( std::move( other.mName ) ), mType( std::move( other.mType ) ),
	mValue( std::move( other.mValue ) )
{
}
	
Message& Message::operator=( const Message &other )
{
	mName = other.mName;
	mType = other.mType;
	mValue = other.mValue;
	return *this;
}
	
Message& Message::operator=( Message &&other )
{
	mName = std::move( other.mName );
	mType = std::move( other.mType );
	mValue = std::move( other.mValue );
	return *this;
}
	
string Message::getJSON( const string &configName ) const
{
    if ( mType == "string" || mType == "boolean" ){
        return "{\"message\":{\"clientName\":\"" + configName + "\",\"name\":\"" + mName + "\",\"type\":\"" + mType + "\",\"value\":\"" + mValue +"\"}}";
    } else {
        return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + mName + "\",\"type\":\"" + mType + "\",\"value\":" + mValue +"}}";
    }
}

bool Message::valueAsBoolean() const
{
    if (mType != "boolean")
		CI_LOG_E( "This Message is not a boolean type! You'll most likely get 'false'." );
    return mValue == "true";
}

int Message::valueAsRange() const
{
    if ( mType != "range" )
		CI_LOG_E("This Message is not a range type! Results may be unpredicatable.");
    return atoi( mValue.c_str() );
}

const string& Message::valueAsString() const
{
    if ( mType != "string" )
		CI_LOG_E( "This Message is not a string type! Returning raw value as string." );
    return mValue;
}
    
#pragma mark Config
	
Config::Config( const std::string& name, const std::string& description )
: mName( name ), mDescription( description )
{
}
	
Config::Config( const Config &other )
: mName( other.mName ), mDescription( other.mDescription ),
	mPublishers( other.mPublishers ), mSubscribers( other.mSubscribers )
{
}
	
Config& Config::operator=( const Config &other )
{
	mName = other.mName;
	mDescription = other.mDescription;
	mPublishers = mPublishers;
	mSubscribers = mSubscribers;
	return *this;
}
	
Config::Config( Config &&other )
: mName( std::move( other.mName ) ), mDescription( std::move( other.mDescription ) ),
	mPublishers( std::move( other.mPublishers ) ), mSubscribers( std::move( other.mSubscribers ) )
{
}
	
Config& Config::operator=( Config &&other )
{
	mName = std::move( other.mName );
	mDescription = std::move( other.mDescription );
	mPublishers = std::move( other.mPublishers );
	mSubscribers = std::move( other.mSubscribers );
	return *this;
}
    
void Config::addSubscribe( const string &name, const string &type )
{
    mSubscribers.push_back( Message( name, type ) );
}

void Config::addSubscribe( const Message &m )
{
    mSubscribers.push_back( m );
}

void Config::addPublish( const string &name, const string &type, const string &def )
{
    mPublishers.push_back( Message( name, type, def ) );
}

void Config::addPublish( const Message &m )
{
    mPublishers.push_back( m );
}

string Config::getJSON() const
{
    string message = "{\"config\": {\"name\": \"" + mName +"\",\"description\":\"" + mDescription +"\",\"publish\": {\"messages\": [";
	int i = 0;
	for( auto & pub : mPublishers ) {
        message += "{\"name\":\"" + pub.getName() + "\",";
        message += "\"type\":\"" + pub.getType() + "\",";
        message += "\"default\":\"" + pub.valueAsString() + "\"";
        message += "}";
        if( i++ < mPublishers.size() - 1 )
            message += ",";
    }
    
    message += "]},\"subscribe\": {\"messages\": [";
	
	i = 0;
	for ( auto & sub : mSubscribers ) {
        message += "{\"name\":\"" + sub.getName() + "\",";
        message += "\"type\":\"" + sub.getType() + "\"";
        message += "}";
        if ( i++ < mSubscribers.size() - 1 )
            message += ",";
    }
    
    message += "]}}}";
    
    return message;
}
    
#pragma mark Connection
	
ConnectionRef Connection::create( const std::string& host, const std::string& name, const std::string& description )
{
	return ConnectionRef( new Connection( host, name, description ) );
}

Connection::Connection( const std::string& host, const std::string& name, const std::string& description )
: mHost( "ws://" + host + ":" + to_string(SPACEBREW_PORT) ),
    mIsConnected( false ), mReconnectInterval( 2000 ), mShouldAutoReconnect( false ),
    mLastTimeTriedConnect( 0 ), mConfig( Config( name, description ) )
{
	initialize();
}

Connection::~Connection()
{
	// Disconnect update signal:
    mUpdateConnection.disconnect();
}

void Connection::initialize()
{
	mClient.reset( new WebSocketClient() );
	// Setup callbacks:
	mUpdateConnection = app::App::get()->getSignalUpdate().connect( std::bind( &Connection::update, this ) ) ;
	//TODO: See if we can add a listener here to websocketpp cinder to automate this
	//WebSocketPP Interface
	mClient->connectOpenEventHandler( &Connection::onConnect, this );
	mClient->connectCloseEventHandler( &Connection::onDisconnect, this );
	mClient->connectFailEventHandler( [](std::string err){ CI_LOG_E(err); } );
	mClient->connectInterruptEventHandler( &Connection::onInterrupt, this );
	mClient->connectPingEventHandler( &Connection::onPing, this );
	mClient->connectMessageEventHandler( &Connection::onRead, this );
}

void Connection::update()
{
    mClient->poll();

    if ( mShouldAutoReconnect ) {
        if ( ! mIsConnected && getElapsedSeconds() - mLastTimeTriedConnect > mReconnectInterval * 1e-3 ) {
			// Disconnect update signal:
			mUpdateConnection.disconnect();
			// Re-initialize ws client:
			initialize();
			// Re-establish connection:
			mClient->connect( mHost );
			// Reset connection attempt time:
			mLastTimeTriedConnect = getElapsedSeconds();
        }
    }
}

void Connection::connect()
{
    mClient->connect( mHost );
}

void Connection::connect( const string &host, const Config &config )
{
    mHost = "ws://" + host + ":" + to_string(SPACEBREW_PORT);
    mConfig = config;
    
    mClient->connect( mHost );
}

void Connection::send( const string &name, const string &type, const string &value )
{
    if ( mIsConnected ) {
        Message m( name, type, value );
        send( m );
    } else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::sendString( const string &name, const string &value )
{
    if ( mIsConnected ) {
        Message m( name, "string", value );
        send( m );
    }
	else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::sendRange( const string &name, int value )
{
    if ( mIsConnected ) {
        Message m( name, "range", to_string(value));
        send( m );
    }
	else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::sendBoolean( const string &name, bool value )
{
    if ( mIsConnected ) {
        string out = value ? "true" : "false";
        Message m( name, "boolean", out );
        send( m );
    }
	else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::send( const Message &m )
{
    if ( mIsConnected ) {
        mClient->write( m.getJSON( mConfig.getName() ) );
    }
	else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::send( Message* m )
{
    if ( mIsConnected ) {
        mClient->write( m->getJSON( mConfig.getName() ) );
    }
	else {
        CI_LOG_E( "Send failed, not connected!" );
    }
}

void Connection::addSubscribe( const string &name, const string &type )
{
    mConfig.addSubscribe( name, type );
    if ( mIsConnected ) {
        updatePubSub();
    }
}

void Connection::addSubscribe( const Message &m )
{
    mConfig.addSubscribe( m );
    if ( mIsConnected ) {
        updatePubSub();
    }
}

void Connection::addPublish( const string &name, const string &type, const string &def)
{
    mConfig.addPublish( name, type, def );
    if ( mIsConnected ) {
        updatePubSub();
    }
}

void Connection::addPublish( const Message &m )
{
    mConfig.addPublish( m );
    if ( mIsConnected ) {
        updatePubSub();
    }
}

bool Connection::isConnected()
{
    return mIsConnected;
}

void Connection::onConnect()
{
    mIsConnected = true;
    updatePubSub();
}

void Connection::onDisconnect()
{
    mIsConnected = false;
    //TODO: Figure out the time
	mLastTimeTriedConnect = getElapsedSeconds();
}

void Connection::onRead( const string &message )
{
    Message m;
    Json::Value json;
    Json::Reader reader;
    if ( reader.parse( message, json ) ) {
        
        m.setName( json["message"]["name"].asString() );
        m.setType( json["message"]["type"].asString() );
        
        if ( m.getType() == "string" && json["message"]["value"].isString() ) {
            m.setValue( json["message"]["value"].asString() );
        }
		else if ( m.getType() == "boolean" ) {
            if ( json["message"]["value"].isInt() ) {
				m.setValue( json["message"]["value"].asInt() == 0 ? "false" : "true" );
            }
			else if ( json["message"]["value"].isString() ) {
                m.setValue( json["message"]["value"].asString() );
            }
        } else if ( m.getType() == "range" ) {
            if ( json["message"]["value"].isInt() ) {
                m.setValue( to_string( json["message"]["value"].asInt() ) );
            }
			else if ( json["message"]["value"].isString() ) {
                m.setValue( json["message"]["value"].asString() );
            }
        }
    }
    
    onMessage.emit( m );
}

}

//Creating the Routes