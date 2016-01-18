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
    void sendSliderValue(string name, float value);
    float mRadius;
    
    struct sliderButton {
        bool isSelected;
        ci::vec2 pos;
        float offset;
        ci::Color color;
        string name;
    };
    
    sliderButton mSliderButtons[3];
};

void SliderSenderApp::setup()
{
    string host = Spacebrew::SPACEBREW_CLOUD;
    string name = "cinder-range-example";
    string description = "Sends three range signals";
    
    mSpacebrew = Spacebrew::Connection::create( host, name, description );
    mSpacebrew->addPublish("red", Spacebrew::TYPE_RANGE);
    mSpacebrew->addPublish("green", Spacebrew::TYPE_RANGE);
    mSpacebrew->addPublish("blue", Spacebrew::TYPE_RANGE);
    
    mSpacebrew->connect();
    
    int count = 0;
    //  initialize booleans and offsets to false and zero; set positions
    for (auto &button : mSliderButtons) {
        button.isSelected = false;
        button.offset = 0;
        button.pos.x = ci::app::getWindowWidth() / 2;
        button.pos.y = (ci::app::getWindowHeight() / 4) + (count * ci::app::getWindowHeight() / 4);
        count++;
    }
    
    //  set colors to red, green, and blue
    mSliderButtons[0].color = ci::Color(1, 0, 0);
    mSliderButtons[1].color = ci::Color(0, 1, 0);
    mSliderButtons[2].color = ci::Color(0, 0, 1);
    
    //  keep track of name of each one
    mSliderButtons[0].name = "red";
    mSliderButtons[1].name = "green";
    mSliderButtons[2].name = "blue";
    
    mRadius = 25;
}

void SliderSenderApp::mouseDown( MouseEvent event )
{
    ci::vec2 mousePos = event.getPos();
    for (auto &button : mSliderButtons) {
        if (glm::distance(mousePos, button.pos) < mRadius) {
            button.isSelected = true;
            button.offset = button.pos.x - mousePos.x;
        }
    }
}

void SliderSenderApp::mouseUp(cinder::app::MouseEvent event)
{
    for (auto &button : mSliderButtons) {
        button.isSelected = false;
    }
}

void SliderSenderApp::mouseDrag(MouseEvent event) {
    for (auto &button : mSliderButtons) {
        if (button.isSelected) {
            button.pos.x = event.getPos().x + button.offset;
            sendSliderValue(button.name, button.pos.x);
        }
    }
}

void SliderSenderApp::sendSliderValue(string name, float value)
{
    //  range values are integers from 0 to 1023
    int mappedValue = lmap<int>(value, 0, ci::app::getWindowWidth(), 0, 1023);
    mSpacebrew->sendRange(name, mappedValue);
}

void SliderSenderApp::update()
{
    
}

void SliderSenderApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    for (auto &button : mSliderButtons) {
        if (button.isSelected) {
            ci::gl::color(button.color * 0.5);
        } else {
            ci::gl::color(button.color);
        }
        ci::gl::drawSolidCircle(button.pos, mRadius);
    }
    
}

CINDER_APP( SliderSenderApp, RendererGl )
