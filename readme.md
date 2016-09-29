ciSpacebrew
=====
A [Spacebrew](http://spacebrew.cc) implementation for [Cinder](http://libcinder.org) 0.9.0

Spacebrew is a service and toolkit for choreographing interactive spaces.

###Setup
* ciSpacebrew requires the Cinder-WebSocketPP block (com.wk.websocketpp)
Latest commit at time of writing is `e89368be757550fa1a10b9611345d79a008e7766`
* If you create a project via TinderBox, it will automatically include the proper libraries and paths into your Cinder project.

###Use

* First include the header file `#include "ciSpacebrew.h"`

* Create a ConnectionRef to start a spacebrew session. By default this will connect automatically to the Spacebrew Cloud (sandbox.spacebrew.cc on port 9000).  This is fine for prototyping, but you should set up your own instance for any production work.
	```c++
	Spacebrew::ConnectionRef spacebrew = Spacebrew::Connection::create();
	``` 


* Optionally use a custom constructor to connect to your own instance
	 ```c++
	Spacebrew::ConnectionRef spacebrew = Spacebrew::Connection::create("localhost", 8080, "My Spacebrew app", "An app to test out the functionality of Spacebrew!");
	```

* Add publishers and subscribers to talk to other apps
	```c++
	spacebrew->addPublish("button", Spacebrew::TYPE_BOOLEAN);
	spacebrew->addSubscribe("message", Spacebrew::TYPE_STRING);
	```
	
* Listen for messages and/or send some out!
	```c++
	spacebrew->addListener( [](const Spacebrew::Message &m){
		console() << "We got a message! " << m.getName() << endl;
	}, this);
	
	spacebrew->send("button", Spacebrew::TYPE_BOOLEAN, true);
	```

--
Check out [http://docs.spacebrew.cc/](http://docs.spacebrew.cc/) for more info.


