#include "cinder/app/AppBasic.h"
#include "cinder/params/Params.h"
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"

#include "ciSpaceBrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Button : public ci::app::AppBasic
{
public:
    
    void prepareSettings( Settings* settings );
    void draw();
    void update();
    void setup();
    
    void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
    
    //A signal recieved from SpaceBrew will appear inside here
    void onMessage( Spacebrew::Message );
    
    // useful quick test
    bool checkInsideCircle( Vec2f point, Vec2f position, int radius );
    
    // button stuff
    int                 radius;
    bool                bButtonPressed;
    bool                bBackgroundOn;
    ci::Font			mFont;
	gl::TextureFontRef  mTextureFont;
    ci::Vec2f			mSize;
    gl::Texture         mTexture;
    
    // create your spacebrew object
    Spacebrew::Connection* spacebrew;
    string msg;
};

void Button::mouseUp( MouseEvent event )
{
    if (bButtonPressed){
        spacebrew->sendBoolean("button", false);
    }
    bButtonPressed = false;
    msg = "PRESS ME";
}

void Button::mouseDown( MouseEvent event )
{
    if ( checkInsideCircle( Vec2f(event.getPos()), Vec2f(getWindowWidth() / 2.0f, getWindowHeight()/2.0f), radius) ){
        msg = "THANKS";
        bButtonPressed = true;
        spacebrew->sendBoolean("button", true);
    }
}

void Button::prepareSettings( Settings* settings )
{
    settings->setFrameRate(60);
    settings->setWindowSize(1000, 1000);
}

void Button::setup()
{
    //These are just standards that you can change.
    //You can find the connections at...spacebrew.cc
    // in the getting started section under spacebrew cloud.
    string host = Spacebrew::SPACEBREW_CLOUD;
    string name = "cinder-button-example";
    string description = "It's amazing";
    
    spacebrew = new Spacebrew::Connection(this);
    spacebrew->addPublish("button", Spacebrew::TYPE_BOOLEAN);
    spacebrew->addSubscribe("background", Spacebrew::TYPE_BOOLEAN);
    spacebrew->connect(host, name, description);
    
    // listen for spacebrew message events
    spacebrew->addListener( &Button::onMessage, this);
    
    // circle stuff
    bButtonPressed  = false;
    radius          = 250;
    mFont           = Font( "Georgia", 40 );
    bBackgroundOn   = false;
    
    mTextureFont    = gl::TextureFont::create( mFont );
    msg             = "PRESS ME";
    
    Spacebrew::Config * config = spacebrew->getConfig();
    cout << config->getJSON() << endl;
    
}

void Button::update()
{
    
}

void Button::draw()
{
    //creating the context
    gl::setViewport( getWindowBounds() );
	gl::setMatricesWindow( getWindowSize() );
    gl::enableAlphaBlending();
    gl::clear( ColorA( 0.0f, 0.0f, 0.0f) );
    
    //drawing the background
    if ( !bBackgroundOn ){
        gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f) );
    } else {
        gl::color( ColorA( 0.8f, 0.8f, 0.8f, 1.0f) );
    }
    gl::drawSolidRect( Rectf( getWindowBounds() ) );
    
    //drawing the button
    if( bButtonPressed ) {
        gl::color( ColorA( 0.75f, 0.0f, 0.0f, 1.0f) );
    } else {
        gl::color( ColorA( 0.0f, 0.9f, 0.9f, 1.0f) );
    }
    gl::drawSolidCircle( Vec2f( getWindowWidth()/2, getWindowHeight()/2 ), radius, 100);
    
    //Positioning the text in the center
    Vec2f middlePos = Vec2f(mTextureFont->measureString(msg).x/2, mTextureFont->measureString(msg).y/2-mTextureFont->getAscent()/2);
    Rectf boundsRect( 0, 0, mTextureFont->measureString(msg).x+1, mTextureFont->measureString(msg).y );
    
    if ( mTextureFont ) {
		
        gl::pushMatrices();
        
            gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
            gl::translate( Vec2f( getWindowWidth()/2-middlePos.x, getWindowHeight()/2-middlePos.y ) );
            mTextureFont->drawStringWrapped( msg, boundsRect );
        
        gl::popMatrices();
        
	}
}

//the spacebrew onmessage function
void Button::onMessage( Spacebrew::Message m)
{
    if ( m.name == "background" ){
        bBackgroundOn = m.valueBoolean();
        
    }
}

//implementing the check to see if we're inside the button
bool Button::checkInsideCircle( Vec2f point, Vec2f position, int radius ){
    return ( point.x < position.x + radius
            && point.x > position.x - radius
            && point.y < position.y + radius
            && point.y > position.y - radius );
}

CINDER_APP_BASIC( Button, RendererGl );
