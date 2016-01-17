#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ciSpacebrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SliderSenderApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;

    Spacebrew::ConnectionRef mSpacebrew;
    
    void onMessage( const Spacebrew::Message &m );
    void sendSliderValue(int slider, float value);
    float mRadius;
    
    bool mSliderIsSelected[3];
    ci::vec2 mSliderPos[3];
    float mSliderOffset[3];
    ci::Color mSliderColor[3];
};

void SliderSenderApp::setup()
{
    string host = Spacebrew::SPACEBREW_CLOUD;
    string name = "cinder-range-example";
    string description = "WIP";
    
    mSpacebrew = Spacebrew::Connection::create( host, name, description );
    mSpacebrew->addPublish("red", Spacebrew::TYPE_RANGE);
    mSpacebrew->addPublish("green", Spacebrew::TYPE_RANGE);
    mSpacebrew->addPublish("blue", Spacebrew::TYPE_RANGE);
    
    mSpacebrew->connect();
    
    for (auto &button : mSliderIsSelected) {
        button = false;
    }
    
    int count = 0;
    for (auto &sliderPos : mSliderPos) {
        sliderPos.x = ci::app::getWindowWidth() / 2;
        sliderPos.y = (ci::app::getWindowHeight() / 4) + (count * ci::app::getWindowHeight() / 4);
        count++;
    }
    
    mSliderColor[0] = ci::Color(1, 0, 0);
    mSliderColor[1] = ci::Color(0, 1, 0);
    mSliderColor[2] = ci::Color(0, 0, 1);
    
    mRadius = 25;
}

void SliderSenderApp::mouseDown( MouseEvent event )
{
    ci::vec2 mousePos = event.getPos();
    for (int i = 0; i < 3; i++) {
        if (glm::distance(mousePos, mSliderPos[i]) < mRadius) {
            mSliderIsSelected[i] = true;
            mSliderOffset[i] = mSliderPos[i].x - mousePos.x;
        }
    }
}

void SliderSenderApp::mouseUp(cinder::app::MouseEvent event)
{
    for (auto &slider : mSliderIsSelected) {
        slider = false;
    }
}

void SliderSenderApp::mouseDrag(MouseEvent event) {
    for (int i = 0; i < 3; i++) {
        if (mSliderIsSelected[i]) {
            mSliderPos[i].x = event.getPos().x + mSliderOffset[i];
            sendSliderValue(i, mSliderPos[i].x);
        }
    }
}

void SliderSenderApp::sendSliderValue(int slider, float value)
{
    int mappedValue = lmap<int>(value, 0, ci::app::getWindowWidth(), 0, 1023);
    switch (slider) {
        case 0:
            mSpacebrew->sendRange("red", mappedValue);
            break;
        case 1:
            mSpacebrew->sendRange("green", mappedValue);
            break;
        case 2:
            mSpacebrew->sendRange("blue", mappedValue);
            break;
    }
}

void SliderSenderApp::update()
{
    
}

void SliderSenderApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    
    for (int i = 0; i < 3; i++) {
        if (mSliderIsSelected[i]) {
            ci::gl::color(mSliderColor[i] * 0.5);
        } else {
            ci::gl::color(mSliderColor[i]);
        }
        ci::gl::drawSolidCircle(mSliderPos[i], mRadius);
    }
    ci::gl::color(1, 1, 1);
}

CINDER_APP( SliderSenderApp, RendererGl )
