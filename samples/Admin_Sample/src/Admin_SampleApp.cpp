#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include <iostream>

#include "ciSpaceBrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Admin_SampleApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    void onClientConnect( Spacebrew::Config & e ) { cout << "OnClientConnect" << e.name << endl; }
    void onClientUpdated( Spacebrew::Config & e ) { cout << "OnClientUpdated" << e.name << endl; }
    void onClientDisconnect( Spacebrew::Config & e ) { cout << "OnclientDisconnect" << e.name << endl; }
    void onRouteAdded( Spacebrew::Route & e ) { cout << "OnRouteAdded" << e.getPubEnd().name << endl; }
    void onRouteRemoved( Spacebrew::Route & e ) { cout << "onRouteRemoved" << e.getPubEnd().name << endl; }
    void onDataPublished( Spacebrew::DataMessage & e ) { cout << "onDataPublished" << e.clientName << endl; }
    
    void onMessage( Spacebrew::Message & m ) { cout << "onMessage" << endl; }
    
    Spacebrew::AdminConnection spacebrewAdmin;
};

void Admin_SampleApp::setup()
{
    spacebrewAdmin.addPublish("Mouse X", Spacebrew::TYPE_RANGE);
    spacebrewAdmin.addPublish("Mouse Y", Spacebrew::TYPE_RANGE);
    spacebrewAdmin.addSubscribe("Values", Spacebrew::TYPE_RANGE);
    spacebrewAdmin.connect(this, Spacebrew::SPACEBREW_CLOUD, "Cinder Admin App");
    spacebrewAdmin.addListener(&Admin_SampleApp::onMessage, this);
    spacebrewAdmin.addClientConnect(&Admin_SampleApp::onClientConnect, this);
    spacebrewAdmin.addClientUpdate(&Admin_SampleApp::onClientUpdated, this);
    spacebrewAdmin.addClientDisconnect(&Admin_SampleApp::onClientDisconnect, this);
    spacebrewAdmin.addRouteAdded(&Admin_SampleApp::onRouteAdded, this);
    spacebrewAdmin.addRouteRemoved(&Admin_SampleApp::onRouteRemoved, this);
    spacebrewAdmin.addDataPublisher(&Admin_SampleApp::onDataPublished, this);
    
}

void Admin_SampleApp::mouseDown( MouseEvent event )
{
    spacebrewAdmin.sendRange("Values", event.getPos().x);
    spacebrewAdmin.sendRange("Values", event.getPos().y);
}

void Admin_SampleApp::update()
{
}

void Admin_SampleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( Admin_SampleApp, RendererGl )
