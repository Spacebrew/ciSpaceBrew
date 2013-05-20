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
        
        // Some useful constants
        static const int            SPACEBREW_PORT  = 9000;
        static const std::string    SPACEBREW_CLOUD = "sandbox.spacebrew.cc";
        static const std::string    TYPE_STRING     = "string";
        static const std::string    TYPE_RANGE      = "range";
        static const std::string    TYPE_BOOLEAN    = "boolean";
        
        /**
         * @brief Spacebrew message
         * @class Spacebrew::Message
         */
        class Message {
        public:
            
            /** @constructor */
            Message(std::string _name="", std::string _type="", std::string _val="");
            virtual std::string getJSON( std::string configName );
            
            /**
             * @brief Name of Message
             * @type {std::string}
             */
            std::string name;
            
            /**
             * @brief Message type ("string", "boolean", "range", or custom type)
             * @type {std::string}
             */
            std::string type;
            
            /**
             * @brief Default value
             * @type {std::string}
             */
            std::string _default;
            
            /**
             * @brief Current value (cast to string)
             * @type {std::string}
             */
            std::string value;
            
            /**
             * @brief Get your incoming value as a boolean
             */
            bool valueBoolean();
            
            /**
             * @brief Get your incoming value as a range (0-1023)
             */
            int  valueRange();
        
            /**
             * @brief Get your incoming value as a string
             */
            std::string valueString();
        
            friend std::ostream& operator<<(std::ostream& os, const Message& vec);
        };
    
        inline std::ostream& operator<<(std::ostream& os, const Message& m) {
            os << m.name << ", " << m.type << ", " << m.value;
            return os;
        }
          
        /**
         * @brief Wrapper for Spacebrew config message. Gets created automatically by
         * Spacebrew::Connection, but can sometimes be nice to use yourself.
         * @class Spacebrew::Config
         */
        class Config {
        public:
            
            // see documentation below
            // docs left out here to avoid confusion. Most people will use these methods
            // on Spacebrew::Connection directly
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
        
        
        /**
         * @brief Main Spacebrew class, connected to Spacebrew server. Sets up socket, builds configs
         * and publishes ofEvents on incoming messages.
         * @class Spacebrew::Connection
         */
        class Connection {
            
            //think about getting rid of the default constructor
            Connection(){};
            
        public:
            
            Connection(cinder::app::App * app);
            ~Connection();
            
            /**
             * @brief Connect to Spacebrew. Pass empty values to connect to default host as "openFrameworks" app
             * (use only for testing!)
             * @param {std::string} host        Host to connect to (e.g. "localhost", SPACEBREW_CLOUD ). Can be IP address OR hostname
             * @param {std::string} name        Name of your app (shows up in Spacebrew admin)
             * @param {std::string} description What does your app do?
             */
            void connect( std::string host = SPACEBREW_CLOUD, std::string name = "cinder app", std::string description = "" );
            void connect( std::string host, Config _config );
            
            /**
             * @brief Send a message
             * @param {std::string} name    Name of message
             * @param {std::string} type    Message type ("string", "boolean", "range", or custom type)
             * @param {std::string} value   Value (cast to string)
             */
            void send( std::string name, std::string type, std::string value );
            
            /**
             * @brief Send a string message
             * @param {std::string} name    Name of message
             * @param {std::string} value   Value
             */
            void sendString( std::string name, std::string value );
            
            /**
             * @brief Send a range message
             * @param {std::string} name    Name of message
             * @param {int}         value   Value
             */
            void sendRange( std::string name, int value );
            
            /**
             * @brief Send a boolean message
             * @param {std::string} name    Name of message
             * @param {bool}        value   Value
             */
            void sendBoolean( std::string name, bool value );
            
            /**
             * Send a Spacebrew Message object
             * @param {Spacebrew::Message} m
             */
            void send( Message m );
            
            /**
             * @brief Send a Spacebrew Message object. Use this method if you've overridden Spacebrew::Message
             * (especially) if you've created a custom getJson() method!)
             * @param {Spacebrew::Message} m
             */
            void send( Message * m );
            
            /**
             * @brief Add a message that you want to subscribe to
             * @param {std::string} name    Name of message
             * @param {std::string} type    Message type ("string", "boolean", "range", or custom type)
             */
            void addSubscribe( std::string name, std::string type );
            
            /**
             * @brief Add a message that you want to subscribe to
             * @param {Spacebrew::Message} m
             */
            void addSubscribe( Message m );
            
            /**
             * @brief Add message of specific name + type to publish
             * @param {std::string} name Name of message
             * @param {std::string} typ  Message type ("string", "boolean", "range", or custom type)
             * @param {std::string} def  Default value
             */
            void addPublish( std::string name, std::string type, std::string def="");
            
            /**
             * @brief Add message to publish
             * @param {Spacebrew::Message} m
             */
            void addPublish( Message m );
            
            /**
             * @return Current Spacebrew::Config (list of publish/subscribe, etc)
             */
            Config* getConfig();
            
            /**
             * @return Are we connected?
             */
            bool isConnected();
            
            /**
             * @brief Turn on/off auto reconnect (try to connect when/if Spacebrew server closes)
             * @param {boolean} bAutoReconnect (true by default)
             */
            void setAutoReconnect( bool bAutoReconnect=true );
            
            /**
             * @brief How often should we try to reconnect if auto-reconnect is on (defaults to 1 second [1000 millis])
             * @param {int} reconnectMillis How often to reconnect, in milliseconds
             */
            void setReconnectRate( int reconnectMillis );
            
            /**
             * @return Are we trying to auto-reconnect?
             */
            bool doesAutoReconnect();
            
            /**
             * @return Current hostname
             */
            std::string getHost();
            
            //These are the connections to ciWebSocketPP
            void onConnect();
            void onDisconnect();
            void onError( std::string msg );
            void onRead(std::string msg);
            void onInterrupt();
            void onPing();
            
            /**
             * @brief signal to subscribe to!
             * @example spacebrew.addListener( &Button::onMessage, this);
             * void Button::onMessage( Spacebrew::Message & m ){
             *     cout<< m.value << endl;
             * };
             */
            signals::signal<void (Message)> onMessage;
            
            /**
             * @brief Helper function to automatically add a listener to a connections onMessage Signal
             */
            template<typename T, typename Y>
            inline void addListener(T callback, Y *callbackObject)
            {
                onMessage.connect(std::bind(callback, callbackObject, std::placeholders::_1));
            }
            
        protected:
            
            //This is the connection to your Cinder App's Update Method
            signals::connection updateConnection;
            //Think about getting rid of this.
            cinder::app::App * app;
            void update();
            
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