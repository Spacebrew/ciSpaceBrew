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

#include "jsoncpp/json.h"

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
	
	Message() = default;
	virtual ~Message() = default;
    /** @constructor */
	Message( const std::string &name, const std::string &type, const std::string &val );
	Message( const std::string &name, const std::string &type );
	Message( const Message &other );
	Message& operator=( const Message &other );
	Message( Message &&other );
	Message& operator=( Message &&other );
	
    virtual std::string getJSON( const std::string &configName ) const;
	
	/**
	 * @brief Sets the name of this Message to \a name
	 */
	void setName( const std::string &name ) { mName = name; }
	
	/**
	 * @brief Returns a const reference to the Name of this message
	 */
	const std::string& getName() const { return mName; }
	
	/**
	 * @brief Sets the type of the message to \a type
	 */
	void setType( const std::string &type ) { mType = type; }
	
	/**
	 * @brief Returns a const reference to the type of this message
	 */
	const std::string& getType() const { return mType; }
	
	/**
	 * @brief Sets your value with \a value
	 */
	void setValue( const std::string &value ) { mValue = value; }
	
	/**
	 * @brief Returns a const reference to your value as a raw string
	 */
	const std::string& getRawValue() const { return mValue; }
	
	/**
	 * @brief Returns the underlying value as a boolean
	 */
	bool valueAsBoolean() const;
	
	/**
	 * @brief Returns the underlying value as a range between (0,1023)
	 */
	int  valueAsRange() const;
	
	/**
	 * @brief Returns the underlying value as a string
	 */
	const std::string& valueAsString() const;
	
protected:
    /**
     * @brief Name of Message
     * @type {std::string}
     */
    std::string mName;
    
    /**
     * @brief Message type ("string", "boolean", "range", or custom type)
     * @type {std::string}
     */
    std::string mType;
    
    /**
     * @brief Current value (cast to string)
     * @type {std::string}
     */
    std::string mValue;
	
    friend std::ostream& operator<<(std::ostream& os, const Message& vec);
};

inline std::ostream& operator<<(std::ostream& os, const Message& m) {
    os << m.getName() << ", " << m.getType() << ", " << m.getRawValue() << std::endl;
    return os;
}
  
/**
 * @brief Wrapper for Spacebrew config message. Gets created automatically by
 * Spacebrew::Connection, but can sometimes be nice to use yourself.
 * @class Spacebrew::Config
 */
class Config {
public:
    
	Config() = default;
	~Config() = default;
	Config( const std::string& name, const std::string& description );
	
	Config( const Config &other );
	Config& operator=( const Config &other );
	Config( Config &&other );
	Config& operator=( Config &&other );
    
    // see documentation below
    // docs left out here to avoid confusion. Most people will use these methods
    // on Spacebrew::Connection directly
    void addSubscribe( const std::string& name, const std::string& type );
    void addSubscribe( const Message& m );
    void addPublish( const std::string& name, const std::string& type, const std::string& def);
    void addPublish( const Message& m );
    
    std::string getJSON() const;
	
	const std::string& getName() const { return mName; }
	const std::string& getDescription() const { return mDescription; }
    
private:
	
	std::string	mName, mDescription;
    std::vector<Message> mPublishers;
    std::vector<Message> mSubscribers;
};


 
class Connection;
typedef std::shared_ptr< Connection > ConnectionRef;
    
/**
 * @brief Main Spacebrew class, connected to Spacebrew server. Sets up socket, builds configs
 * and publishes ofEvents on incoming messages.
 * @class Spacebrew::Connection
 */
class Connection : ci::Noncopyable {
public:
    
	static ConnectionRef create( const std::string& host = SPACEBREW_CLOUD,
								 const std::string& name = "cinder app",
								 const std::string& description = "" );
	
    virtual ~Connection();
	
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
	const Config& getConfig() const { return mConfig; }
	
    /**
     * @return Are we connected?
     */
	bool isConnected() { return mIsConnected; }
	
    /**
     * @brief Turn on/off auto reconnect (try to connect when/if Spacebrew server closes)
     * @param {boolean} bAutoReconnect (true by default)
     */
	void setAutoReconnect( bool shouldAutoReconnect = true ){ mShouldAutoReconnect = shouldAutoReconnect; }
	
    /**
     * @brief How often should we try to reconnect if auto-reconnect is on (defaults to 1 second [1000 millis])
     * @param {int} reconnectMillis How often to reconnect, in milliseconds
     */
	void setReconnectRate( int reconnectMillis ) { mReconnectInterval = reconnectMillis; }
	
    /**
     * @return Are we trying to auto-reconnect?
     */
	bool doesAutoReconnect() { return mShouldAutoReconnect; }
	
    /**
     * @return Current hostname
     */
	const std::string& getHost() const { return mHost; }
	
    //These are the connections to ciWebSocketPP
    virtual void onConnect();
    virtual void onDisconnect();
    virtual void onRead( const std::string &msg );
	virtual void onInterrupt() {}
	virtual void onPing( const std::string &msg ) {}
		
    /**
     * @brief signal to subscribe to!
     * @example spacebrew.addListener( &Button::onMessage, this);
     * void Button::onMessage( Spacebrew::Message & m ){
     *     cout<< m.value << endl;
     * };
     */
	ci::signals::Signal<void (const Message&)> onMessage;
    
    /**
     * @brief Helper function to automatically add a listener to a connections onMessage Signal
     */
    template<typename T, typename Y>
    inline void addListener(T callback, Y *callbackObject)
    {
        onMessage.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) );
    }
    
    
protected:
	Connection( const std::string& host, const std::string& name, const std::string& description );
	void initialize();
	
	virtual void update();
	void updatePubSub() { mClient->write( mConfig.getJSON() ); }
	
	std::unique_ptr<WebSocketClient> mClient;
	//This is the connection to your Cinder App's Update Method
	ci::signals::Connection mUpdateConnection;
	
    std::string		mHost;
    Config			mConfig;
    
	bool			mIsConnected,
					mShouldAutoReconnect;
    double			mLastTimeTriedConnect,
					mReconnectInterval;
};
    
//Creating the Routes
    
	
}