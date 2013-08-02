//
//  ciSpaceBrew.h
//  Cinder - Space Brew Client
//
//  Created by Ryan Bartley on 5/11/13.
//
//

#pragma once

#include <vector>
#include <map>

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
    Message(const std::string &name = "", const std::string &type="", const std::string &val="")
    : name( name ), type( type ), _default( val ), value( val ) { }
    
    virtual std::string getJSON( const std::string &configName ) const;
    
    void setName( const std::string &name )  { this->name = name; }
    void setType( const std::string &type )  { this->type = type; }
    void setValue( const std::string &value )  { this->value = value; }
    void setValue( const int value ) { this->value = std::to_string(value); }
    
    const std::string& getName() const { return name; }
    const std::string& getType() const { return type; }
    const std::string& getValue() const { return value; }
    
private:
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
    
    
public:
    /**
     * @brief Get your incoming value as a boolean
     */
    bool valueBoolean() const;
    
    /**
     * @brief Get your incoming value as a range (0-1023)
     */
    int  valueRange() const;

    /**
     * @brief Get your incoming value as a string
     */
    std::string valueString() const;

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
    
    Config( const std::string& name, const std::string& description, const std::string &remoteAddress = "" )
    : name( name ), description( description ), remoteAddress( remoteAddress ) {}
    
    // see documentation below
    // docs left out here to avoid confusion. Most people will use these methods
    // on Spacebrew::Connection directly
    void addSubscribe( const std::string &name, const std::string &type )
    { addSubscribe( Message( name, type ) ); }
    
    void addSubscribe( const Message &m ) { subscribe.push_back(m); }
    
    void addPublish( const std::string &name, const std::string &type, const std::string &def)
    { addPublish( Message( name, type, def ) ); }
    
    void addPublish( const Message &m ) { publish.push_back(m); }
    
    std::vector<Message>& getPublish() { return publish; }
    std::vector<Message>& getSubscribe() { return subscribe; }
    
    const std::vector<Message>& getPublish() const { return publish; }
    const std::vector<Message>& getSubscribe() const { return subscribe; }
    
    const std::string& getName() { return name; }
    const std::string& getRemote() const { return remoteAddress; }
    
    void setRemote( const std::string & remote ) { remoteAddress = remote; }
    
    void resetPubSub() { publish.clear(); subscribe.clear(); }
    
    // note: only use this with Admin configs!
    // it only checks name/address
    bool operator == ( const Config & comp );
    
    std::string getJSON();
    
private:
    std::string name, description, remoteAddress;
    
    std::vector<Message> publish;
    std::vector<Message> subscribe;
};


 
class Connection;
typedef std::shared_ptr< Connection > ConnectionRef;
    
/**
 * @brief Main Spacebrew class, connected to Spacebrew server. Sets up socket, builds configs
 * and publishes ofEvents on incoming messages.
 * @class Spacebrew::Connection
 */
class Connection {

public:
    
    explicit Connection( cinder::app::App * app, const std::string& host = SPACEBREW_CLOUD, const std::string& name = "cinder app", const std::string& description = "" );
    
    static ConnectionRef create( cinder::app::App * app, const std::string& host = SPACEBREW_CLOUD, const std::string& name = "cinder app", const std::string& description = "" )
    { return ConnectionRef( new Connection( app, host, name, description ) ); }
    
    ~Connection();
    
    /**
     * @brief Connect to Spacebrew. Pass empty values to connect to default host as "openFrameworks" app
     * (use only for testing!)
     * @param {cinder::app::App*} app   This defines the attached cinder app to control time and update connection 
     * @param {std::string} host        Host to connect to (e.g. "localhost", SPACEBREW_CLOUD ). Can be IP address OR hostname
     * @param {std::string} name        Name of your app (shows up in Spacebrew admin)
     * @param {std::string} description What does your app do?
     */
    void connect();
    void connect( const std::string &host, const Config &config );
    
    /**
     * @brief Send a message
     * @param {std::string} name    Name of message
     * @param {std::string} type    Message type ("string", "boolean", "range", or custom type)
     * @param {std::string} value   Value (cast to string)
     */
    void send( const std::string &name, const std::string &type, const std::string &value );
    
    /**
     * @brief Send a string message
     * @param {std::string} name    Name of message
     * @param {std::string} value   Value
     */
    void sendString( const std::string &name, const std::string &value );
    
    /**
     * @brief Send a range message
     * @param {std::string} name    Name of message
     * @param {int}         value   Value
     */
    void sendRange( const std::string &name, int value );
    
    /**
     * @brief Send a boolean message
     * @param {std::string} name    Name of message
     * @param {bool}        value   Value
     */
    void sendBoolean( const std::string &name, bool value );
    
    /**
     * Send a Spacebrew Message object
     * @param {Spacebrew::Message} m
     */
    void send( const Message &m );
    
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
    void addSubscribe( const std::string &name, const std::string &type );
    
    /**
     * @brief Add a message that you want to subscribe to
     * @param {Spacebrew::Message} m
     */
    void addSubscribe( const Message &m );
    
    /**
     * @brief Add message of specific name + type to publish
     * @param {std::string} name Name of message
     * @param {std::string} typ  Message type ("string", "boolean", "range", or custom type)
     * @param {std::string} def  Default value
     */
    void addPublish( const std::string &name, const std::string &type, const std::string &def = "" );
    
    /**
     * @brief Add message to publish
     * @param {Spacebrew::Message} m
     */
    void addPublish( const Message &m );
    
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
    void setAutoReconnect( bool bAutoReconnect = true );
    
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
    void onError( const std::string &err );
    void onRead( const std::string &msg );
    void onInterrupt();
    void onPing( const std::string &msg );
    
    /**
     * @brief signal to subscribe to!
     * @example spacebrew.addListener( &Button::onMessage, this);
     * void Button::onMessage( Spacebrew::Message & m ){
     *     cout<< m.value << endl;
     * };
     */
    signals::signal<void (const Message&)> onMessage;
    
    /**
     * @brief Helper function to automatically add a listener to a connections onMessage Signal
     */
    template<typename T, typename Y>
    inline void addListener(T callback, Y *callbackObject)
    {
        onMessage.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
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
    
//Creating the Routes
    
class RouteEndpoint {
public:
    explicit RouteEndpoint( const std::string & clientName, const std::string &name, const std::string &remoteAddress, const std::string &type = "" )
    : clientName( clientName ), name( name ), type( type ), remoteAddress( remoteAddress ) {}
    
    bool operator==( const RouteEndpoint &comp)
    {
        return comp.clientName == clientName && comp.name == name && comp.type == type && comp.remoteAddress == remoteAddress;
    }
    
    void setType( const std::string &type ) { this->type = type; }
    
    const std::string& getClient() const { return clientName; }
    const std::string& getName() const { return name; }
    const std::string& getType() const { return type; }
    const std::string& getRemote() const { return remoteAddress; }
    
    friend class Route;
    
private:
    std::string clientName;
    std::string name;
    std::string type;
    std::string remoteAddress;
    
};

enum RouteUpdateType {
    ADD_ROUTE,
    REMOVE_ROUTE
};
    
static std::string getRouteUpdateTypeString( RouteUpdateType type ){
    switch(type){
        case ADD_ROUTE:
            return "add";
            break;
        case REMOVE_ROUTE:
            return "remove";
            break;
    }
}
    
class Route {
public:
    explicit Route( const RouteEndpoint &pub, const RouteEndpoint &sub)
    : publisher( pub ), subscriber( sub ) {}
    
    void updatePublisher( const RouteEndpoint &pub ) { publisher = pub; }
    void updateSubscriber( const RouteEndpoint &sub ) { subscriber = sub; }
    
    RouteEndpoint getPubEnd() const { return publisher; }
    RouteEndpoint getSubEnd() const { return subscriber; }
    
    bool operator == ( const Route & r) { return publisher == r.getPubEnd() && subscriber == r.getSubEnd(); }
    
private:
    RouteEndpoint publisher, subscriber;
};
    
class DataMessage : public Message {
public:
    explicit DataMessage( const std::string &clientName, const std::string &remoteAddress, const std::string &name = "", const std::string &type = "", const std::string &val = ""  )
    : clientName( clientName ), remoteAddress( remoteAddress ), Message( name, type, val ) {}
    
    std::string clientName;
    std::string remoteAddress;
};
    
class AdminConnection;
typedef std::shared_ptr< AdminConnection > AdminConnectionRef;
    
class AdminConnection : public Connection {
public:
    
    explicit AdminConnection( cinder::app::App * app, const std::string& host = SPACEBREW_CLOUD, const std::string& name = "cinder app", const std::string& description = "" ) : Connection( app, host, name, description ) {}
    
    static AdminConnectionRef create( cinder::app::App * app, const std::string& host = SPACEBREW_CLOUD, const std::string& name = "cinder app", const std::string& description = "" )
    { return AdminConnectionRef( new AdminConnection( app, host, name, description ) ); }
    
    ;
    ~AdminConnection();

    // websocket methods
    virtual void onOpen( const std::string & );
    virtual void onMessage( const std::string & );
    
    // add routes
    /**
     * Method that is used to add a route to the Spacebrew server
     * @param {String} pub_client               Publish client app name
     * @param {String} pub_address              Publish app remote IP address
     * @param {String} pub_name    				Publish name
     * @param {String} sub_client  				Subscribe client app name
     * @param {String} sub_address 				Subscribe app remote IP address
     * @param {String} sub_name    				Subscribe name
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool addRoute( const std::string &pub_client, const std::string &pub_address, const std::string &pub_name, const std::string &sub_client, const std::string &sub_address, const std::string &sub_name );
 
    /**
     * Method that is used to add a route to the Spacebrew server
     * @param {RouteEndpoint} pub_endpoint       Publisher endpoint
     * @param {RouteEndpoint} sub_endpoint       Subscriber endpoint
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool addRoute( const RouteEndpoint &pub_endpoint, const RouteEndpoint &sub_endpoint );
 
    /**
     * Method that is used to add a route to the Spacebrew server
     * @param {Route} route
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool addRoute( const Route &route );
 
    /**
     * Method that is used to remove a route from the Spacebrew server
     * @param {String} pub_client               Publish client app name
     * @param {String} pub_address              Publish app remote IP address
     * @param {String} pub_name    				Publish name
     * @param {String} sub_client  				Subscribe client app name
     * @param {String} sub_address 				Subscribe app remote IP address
     * @param {String} sub_name    				Subscribe name
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool removeRoute( const std::string &pub_client, const std::string &pub_address, const std::string &pub_name,
                     const std::string &sub_client, const std::string &sub_address, const std::string &sub_name );
 
    /**
     * Method that is used to remove a route from the Spacebrew server
     * @param {RouteEndpoint} pub_endpoint       Publisher endpoint
     * @param {RouteEndpoint} sub_endpoint       Subscriber endpoint
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool removeRoute( const RouteEndpoint &pub_endpoint, const RouteEndpoint &sub_endpoint );
 
    /**
     * Method that is used to remove a route from the Spacebrew server
     * @param {Route} route
     *
     * @memberOf Spacebrew::AdminConnection
     */
    bool removeRoute( const Route &route );
 
    // getters
//    std::map< std::string, Config >&      getConnectedClients() { return connectedClients; }
//    std::map< std::string, Route >&       getCurrentRoutes() { return currentRoutes; }
//    const std::map< std::string, Config >&   getConnectedClients() const { return connectedClients; }
//    const std::map< std::string, Route >&    getCurrentRoutes() const { return currentRoutes; }
    
    /**
     * @brief Helper function to automatically add a listener to a admin connections events
     * Note: you must call the normal Spacebrew::addListener function to listen to normal messages
     * @example
     * Spacebrew::connection;
     *
     * void onClientConnect( Spacebrew::Config & e ){};
     * void onClientUpdated( Spacebrew::Config & e ){};
     * void onClientDisconnect( Spacebrew::Config & e ){};
     * void onRouteAdded( Spacebrew::Route & e ){};
     * void onRouteRemoved( Spacebrew::Route & e ){};
     * void onDataPublished( Spacebrew::DataMessage & e ){};
     *
     * void setup(){
     *      Spacebrew::addAdminListener( this, connection);
     * }
     */
    template<typename T, typename Y>
    inline void addClientConnect(T callback, Y *callbackObject)
    {
        connectionClientConnect = onClientConnect.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    template<typename T, typename Y>
    inline void addClientUpdate(T callback, Y *callbackObject)
    {
        connectionClientUpdated = onClientUpdated.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    template<typename T, typename Y>
    inline void addClientDisconnect(T callback, Y *callbackObject)
    {
        connectionClientDisconnect = onClientDisconnect.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    template<typename T, typename Y>
    inline void addRouteAdded(T callback, Y *callbackObject)
    {
        connectionRouteAdded = onRouteAdded.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    template<typename T, typename Y>
    inline void addRouteRemoved(T callback, Y *callbackObject)
    {
        connectionRouteRemoved = onRouteRemoved.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    template<typename T, typename Y>
    inline void addDataPublisher(T callback, Y *callbackObject)
    {
        connectionDataPublished = onDataPublished.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    inline void removeListeners()
    {
        connectionClientConnect.disconnect();
        connectionClientUpdated.disconnect();
        connectionClientDisconnect.disconnect();
        connectionRouteAdded.disconnect();
        connectionRouteRemoved.disconnect();
        connectionDataPublished.disconnect();
    }
 
protected:
    
    // events
    
    
    
    signals::signal<void (Config)>      onClientConnect;
    signals::connection                 connectionClientConnect;
    signals::signal<void (Config)>      onClientUpdated;
    signals::connection                 connectionClientUpdated;
    signals::signal<void (Config)>      onClientDisconnect;
    signals::connection                 connectionClientDisconnect;
    
    signals::signal<void (Route)>       onRouteAdded;
    signals::connection                 connectionRouteAdded;
    signals::signal<void (Route)>       onRouteRemoved;
    signals::connection                 connectionRouteRemoved;
    
    signals::signal<void (DataMessage)> onDataPublished;
    signals::connection                 connectionDataPublished;
    
    std::vector<Config>                 connectedClients;
    std::vector<Route>                  currentRoutes;
    
//    std::map< std::string, Config >      connectedClients;
//    std::map< std::string, Route >       currentRoutes;
    
    /**
    * Method that handles both add and remove route requests. Responsible for parsing requests
    * and communicating with Spacebrew server
    */
    void updateRoute( RouteUpdateType type, const Route &route );
        
    void processIncomingJson( const Json::Value & val );
    
    
};
    }
}