#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "ciSpaceBrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Template : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
	void update();
	void draw();
    
    void onMessage(Spacebrew::Message msg);
    
    Spacebrew::Connection * spacebrew;
    Color myColor;

};

void Template::onMessage(Spacebrew::Message msg)
{
    if (msg.name == "backgroundOn") {
        if (msg.value == "true") {
            myColor = Color(255, 255, 255);
        } else {
            myColor = Color(0, 0, 0);
        }
    }
}

void Template::setup()
{
    spacebrew = new Spacebrew::Connection(this);
    
    string host = "sandbox.spacebrew.cc"; // change to localhost to test Spacebrew local server
    string name = "cinder-example";
    string description = "It's amazing";
    spacebrew->addSubscribe("backgroundOn", Spacebrew::TYPE_BOOLEAN);
    spacebrew->addPublish("cinder-mouse", "boolean", "false");
    spacebrew->connect( host, name, description );
    
    spacebrew->addListener( &Template::onMessage, this);
    
    myColor = Color(0,0,0);
}

void Template::mouseDown( MouseEvent event )
{
    spacebrew->sendBoolean("cinder-mouse", true);
}

void Template::mouseUp( MouseEvent event )
{
    spacebrew->sendBoolean("cinder-mouse", false);
}

void Template::update()
{
    
}

void Template::draw()
{
	// clear out the window with black
	gl::clear(  myColor  );
}

CINDER_APP_NATIVE( Template, RendererGl )
