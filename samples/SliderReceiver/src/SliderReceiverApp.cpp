#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ciSpaceBrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SliderReceiverApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void onMessage( const Spacebrew::Message &msg );
    
    Spacebrew::ConnectionRef mSpacebrew;
    ci::Color mCurrentColor;
};

void SliderReceiverApp::setup()
{
    string host = Spacebrew::SPACEBREW_CLOUD;
    string name = "cinder-range-receiver";
    string description = "Listens for three range signals";
    mSpacebrew = Spacebrew::Connection::create( host, name, description );
    mSpacebrew->addSubscribe("red", Spacebrew::TYPE_RANGE);
    mSpacebrew->addSubscribe("green", Spacebrew::TYPE_RANGE);
    mSpacebrew->addSubscribe("blue", Spacebrew::TYPE_RANGE);
    mSpacebrew->addListener(&SliderReceiverApp::onMessage, this);
    
    mSpacebrew->connect();
    
    mCurrentColor = ci::Color(0.5f, 0.5f, 0.5f);
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
    ci::gl::color(mCurrentColor);
    ci::gl::drawSolidRect(ci::Rectf(getWindowBounds()));
    ci::Rectf contrastRect = ci::Rectf(getWindowWidth() / 3, getWindowHeight() / 3, (getWindowWidth() / 3) * 2, (getWindowHeight() / 3) * 2);
    ci::gl::color(ci::Color(1.0f - mCurrentColor.r, 1.0f - mCurrentColor.g, 1.0f - mCurrentColor.b));
    ci::gl::drawSolidRect(contrastRect);
}

void SliderReceiverApp::onMessage(const Spacebrew::Message &msg)
{
    //  get the value
    float receivedValue = (float)msg.valueAsRange();
    
    //  map the received value to a number between 0 and 1
    float colorValue = lmap<float>(receivedValue, 0.f, 1023.f, 0.f, 1.f);
    
    //  change the colors
    if (msg.getName() == "red") {
        mCurrentColor.r = colorValue;
    } else if (msg.getName() == "green") {
        mCurrentColor.g = colorValue;
    } else if (msg.getName() == "blue") {
        mCurrentColor.b = colorValue;
    }
}

CINDER_APP( SliderReceiverApp, RendererGl )
