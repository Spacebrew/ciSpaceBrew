#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ciSpacebrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ButtonApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	
	void mouseDown( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	
	// A signal recieved from SpaceBrew will appear inside here
	void onMessage( const Spacebrew::Message &m );
	
	// useful quick test
	bool checkInsideCircle( vec2 point, vec2 position, int radius );
	
	// button stuff
	ci::Font			mFont;
	gl::TextureFontRef  mTextureFont;
	
	vec2				mSize;
	int                 radius;
	bool                bButtonPressed;
	bool                bBackgroundOn;
	
	// create your spacebrew object
	Spacebrew::ConnectionRef spacebrew;
	string msg;
};

void ButtonApp::mouseUp( MouseEvent event )
{
	if (bButtonPressed){
		spacebrew->sendBoolean("button", false);
	}
	bButtonPressed = false;
	msg = "PRESS ME";
}

void ButtonApp::mouseDown( MouseEvent event )
{
	if ( checkInsideCircle( vec2(event.getPos()), vec2(getWindowWidth() / 2.0f, getWindowHeight()/2.0f), radius) ){
		msg = "THANKS";
		bButtonPressed = true;
		spacebrew->sendBoolean("button", true);
	}
}

void ButtonApp::setup()
{
	//These are just standards that you can change.
	//You can find the connections at...spacebrew->cc
	// in the getting started section under spacebrew cloud.
	string host = Spacebrew::SPACEBREW_CLOUD;
	string name = "cinder-button-example";
	string description = "It's amazing";
	
	spacebrew = Spacebrew::Connection::create( host, name, description );
	
	//create as many publishers and subscribers as you need.
	spacebrew->addPublish("button", Spacebrew::TYPE_BOOLEAN);
	spacebrew->addSubscribe("background", Spacebrew::TYPE_BOOLEAN);
	
	//send 'this' to Spacebrew so that it can connect to the update signal from cinder.
	spacebrew->connect();
	
	// listen for spacebrew message events
	spacebrew->addListener( &ButtonApp::onMessage, this);
	
	// circle stuff
	bButtonPressed  = false;
	radius          = 250;
	mFont           = Font( "Georgia", 40 );
	bBackgroundOn   = false;
	
	mTextureFont    = gl::TextureFont::create( mFont );
	msg             = "PRESS ME";
	
	auto & config = spacebrew->getConfig();
	console() << config.getJSON() << endl;
}

void ButtonApp::update()
{
	
}

void ButtonApp::draw()
{
	//creating the context
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f) );
	
	//drawing the background
	if ( ! bBackgroundOn ){
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
	gl::drawSolidCircle( getWindowCenter(), radius, 100);
	
	//Positioning the text in the center
	vec2 middlePos = vec2(mTextureFont->measureString(msg).x/2, mTextureFont->measureString(msg).y/2-mTextureFont->getAscent()/2);
	Rectf boundsRect( 0, 0, mTextureFont->measureString(msg).x+1, mTextureFont->measureString(msg).y );
	
	if ( mTextureFont ) {
		
		gl::pushMatrices();
		
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
		gl::translate( vec2( getWindowWidth()/2-middlePos.x, getWindowHeight()/2-middlePos.y ) );
		mTextureFont->drawStringWrapped( msg, boundsRect );
		
		gl::popMatrices();
		
	}
}

//the spacebrew onmessage function
void ButtonApp::onMessage( const Spacebrew::Message &m )
{
	if ( m.getName() == "background" ){
		bBackgroundOn = m.valueAsBoolean();
	}
}

//implementing the check to see if we're inside the button
bool ButtonApp::checkInsideCircle( vec2 point, vec2 position, int radius ){
	return ( point.x < position.x + radius
			&& point.x > position.x - radius
			&& point.y < position.y + radius
			&& point.y > position.y - radius );
	
}

CINDER_APP( ButtonApp, RendererGl, []( App::Settings* settings ) {
	settings->setFrameRate(60);
	settings->setWindowSize(1000, 1000);
})
