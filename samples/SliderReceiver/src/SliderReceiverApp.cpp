#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SliderReceiverApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SliderReceiverApp::setup()
{
}

void SliderReceiverApp::mouseDown( MouseEvent event )
{
}

void SliderReceiverApp::update()
{
}

void SliderReceiverApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( SliderReceiverApp, RendererGl )
