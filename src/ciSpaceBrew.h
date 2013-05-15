//
//  ciSpaceBrew.h
//  Cinder - Space Brew Client
//
//  Created by Ryan Bartley on 5/11/13.
//
//

#pragma once

#include <vector>

#include "WebSocketClient.h"
#include "cinder/app/App.h"

#include "json/json.h"

namespace cinder {
    namespace Spacebrew {
        
        static const int            SPACEBREW_PORT  = 9000;
        static const std::string    SPACEBREW_CLOUD = "sandbox.spacebrew.cc";
        static const std::string    TYPE_STRING     = "string";
        static const std::string    TYPE_RANGE      = "range";
        static const std::string    TYPE_BOOLEAN    = "boolean";
        
        class Message {
        public:
            
            Message(std::string _name="", std::string _type="", std::string _val="");
            virtual std::string getJSON( std::string configName );
            
            std::string name;
            
            std::string type;
            
            std::string _default;
            
            std::string value;
            
            bool valueBoolean();
            
            int  valueRange();
        
            std::string valueString();
        
            friend std::ostream& operator<<(std::ostream& os, const Message& vec);
        };
    
        inline std::ostream& operator<<(std::ostream& os, const Message& m) {
            os << m.name << ", " << m.type << ", " << m.value;
            return os;
        }
            
        class Config {
        public:
            
            void addSubscribe( std::string name, std::string type );
            void addSubscribe( Message m );
            void addPublish( std::string name, std::string type, std::string def);
            void addPublish( Message m );
            
            std::string getJSON();
            std::string name, description;
            
        private:
            
            std::vector<Message> publish;
            std::vector<Message> subscribe;
        };
        
        
        class Connection {
            
            //think about getting rid of the default constructor
            Connection(){};
            
        public:
            
            Connection(cinder::app::App * app);
            ~Connection();
            
            
            void connect( std::string host = SPACEBREW_CLOUD, std::string name = "cinder app", std::string description = "" );
            
            void connect( std::string host, Config _config );
            
            void send( std::string name, std::string type, std::string value );
            
            void sendString( std::string name, std::string value );
            
            void sendRange( std::string name, int value );
            
            void sendBoolean( std::string name, bool value );
            
            void send( Message m );
            
            void send( Message * m );
            
            void addSubscribe( std::string name, std::string type );
            
            void addSubscribe( Message m );
            
            void addPublish( std::string name, std::string type, std::string def="");
            
            void addPublish( Message m );
            
            Config* getConfig();
            
            bool isConnected();
            
            void setAutoReconnect( bool bAutoReconnect=true );
            
            void setReconnectRate( int reconnectMillis );
            
            bool doesAutoReconnect();
            
            std::string getHost();
            
            //These are the connections to ciWebSocketPP
            void onConnect();
            void onDisconnect();
            void onError( std::string msg );
            void onRead(std::string msg);
            void onInterrupt();
            void onPing();
            
            
            //This is the connection to the outside world
            signals::signal<void (Message)> onMessageConnection;
            
            //This is how the connection to the outside world works
            template<typename T, typename Y>
            inline void addListener(T callback, Y *callbackObject)
            {
                onMessageConnection.connect(std::bind(callback, callbackObject, std::placeholders::_1));
            }
            
        protected:
            
            cinder::app::App * app;
            void update( );
            
            std::string host;
            bool bConnected;
            void updatePubSub();
            
            Config config;
            
            bool bAutoReconnect;
            int lastTimeTriedConnect;
            int reconnectInterval;
            
            WebSocketClient client;
            
        };
    }
}