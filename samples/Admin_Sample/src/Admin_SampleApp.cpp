#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Admin_SampleApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void Admin_SampleApp::setup()
{
}

void Admin_SampleApp::mouseDown( MouseEvent event )
{
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
