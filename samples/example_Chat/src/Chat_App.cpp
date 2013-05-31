#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Texture.h"
#include <vector>
#include "ciSpaceBrew.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ChatApp : public AppNative {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
    void prepareSettings( Settings * settings );
    
    void onMessage( Spacebrew::Message msg );
    
    void renderChat( vector<string> * chat, gl::TextureFontRef chatFont, int placement);
    
    Spacebrew::Connection * spacebrew;
    
    vector<string>      iWrote;
    vector<string>      uWrote;
    
    string              s;
    
    bool                receivedU;
    bool                receivedMe;
    
    ci::Font            mFont;
    ci::Font            uFont;
    gl::TextureFontRef  mTextureFont;
    gl::TextureFontRef  uTextureFont;
    ci::Font            params;
    gl::TextureFontRef  pTextureFont;
    
    ci::Vec2f           mSize;
    gl::Texture         mTexture;
    
    
};

void ChatApp::prepareSettings( Settings * settings )
{
    settings->setFrameRate(60);
    settings->setWindowSize(1000, 1000);
}

void ChatApp::keyDown( KeyEvent event)
{
    //This receives the keys from your keyboard and you can type until you want
    //to send by pressing the enter or return key
    if (event.getCode() != KeyEvent::KEY_RETURN) {
        s += event.getChar();
        cout << event.getChar();
    } else if (event.getCode() == KeyEvent::KEY_RETURN) {
        iWrote.push_back(s);
        spacebrew->sendString("myChat", s);
        s = "";
        receivedMe = true;
        cout << endl;
    }
    
}

void ChatApp::onMessage(Spacebrew::Message msg)
{
    //This receives the message from your chatter and pushes the info into the vector
    if (msg.name == "yourChat") {
        uWrote.push_back(msg.valueString());
        receivedU = true;
    }
}

void ChatApp::renderChat( vector<string> *chat, gl::TextureFontRef chatFont, int placement )
{
    //draw background and text
    glPushMatrix();
    {
        gl::color( ColorA( 0.0f, 0.8f, 0.5f, 0.5f ) );
        
        gl::translate( Vec2f( getWindowWidth()/10 * placement, 0 ) );
        
        Rectf chatBack( 0, 0, getWindowWidth()/5, getWindowHeight() );
        gl::drawSolidRect(chatBack);
        
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
        gl::drawLine( Vec2f( 0, 0 ), Vec2f( 0, getWindowHeight() ) );
        gl::drawLine( Vec2f( getWindowWidth()/5, 0 ), Vec2f( getWindowWidth()/5, getWindowHeight() ) );
        
        
        glPushMatrix();
        {
            int j = 0;
            float k = 1;
            
            gl::translate( Vec2f( 0, getWindowHeight()/2 ) );
            
            
            for (vector<string>::iterator it = chat->begin(); it != chat->end(); it++) {
                
                gl::color( ColorA( 1.0f, 1.0f, 1.0f, (float)(k/chat->size()) ) );
                
                Rectf chatWrap = Rectf( 0, 0, getWindowWidth()/5+1, chatFont->getAscent() );
                
                gl::translate( Vec2f(0, j));
                
                chatFont->drawStringWrapped( *it, chatWrap );
                
                j = chatFont->measureStringWrapped(*it, chatWrap).y;//chatFont->getAscent() + chatFont->getDescent();
                k++;
            }
        }
        glPopMatrix();
        
    }
    glPopMatrix();

}

void ChatApp::setup()
{
    string host = Spacebrew::SPACEBREW_CLOUD;
    //change this name to distinguish between the chatters
    string name = "cinder-chat-example";
    string description = "It's amazing";
    
    spacebrew = new Spacebrew::Connection(this);
    spacebrew->addPublish("myChat", Spacebrew::TYPE_STRING);
    spacebrew->addSubscribe("yourChat", Spacebrew::TYPE_STRING);
    spacebrew->connect(host, name, description);
    
    spacebrew->addListener( &ChatApp::onMessage, this);
    
    receivedU = false;
    receivedMe = false;
    
    mFont = Font( "Georgia", 24 );
    uFont = Font( "Helvetica", 24 );
    
    mTextureFont = gl::TextureFont::create( mFont );
    uTextureFont = gl::TextureFont::create( uFont );
}

void ChatApp::update()
{
    
}

void ChatApp::draw()
{
	//creating the context
    gl::setViewport( getWindowBounds() );
	gl::setMatricesWindow( getWindowSize() );
    gl::enableAlphaBlending();
    gl::clear( ColorA( 0.0f, 0.0f, 0.0f) );
    
    //drawing the background
    if ( receivedU ){
        gl::color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
        receivedU = false;
    } else if ( receivedMe ) {
        gl::color( ColorA( 0.0f, 0.0f, 1.0f, 1.0f ) );
        receivedMe = false;
    } else {
        gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
    }
    gl::drawSolidRect( Rectf( getWindowBounds() ) );
    
    //rendering the chats
    renderChat( &iWrote, mTextureFont, 1 ); //myChat
    renderChat( &uWrote, uTextureFont, 7 ); //yourChat
    
    glPushMatrix();
    {
        gl::translate( Vec2f( getWindowWidth()/2 - ((getWindowWidth()/5+1)/2), getWindowHeight()/2 ) );
        
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
        Rectf textWrap = Rectf( 0, mFont.getAscent(), getWindowWidth()/5+1, uTextureFont->getAscent() );
        mTextureFont->drawStringWrapped( s, textWrap );
        
        gl::color( ColorA( 0.3f, 0.5f, 0.6f, 0.5f) );
        Rectf typeBack( 0, 0, getWindowWidth()/5, getWindowHeight()/5 );
        gl::drawSolidRect(typeBack);
        
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
        Rectf textWrap = Rectf( 0, mFont.getAscent(), getWindowWidth()/5+1, uTextureFont->getAscent() );
        mTextureFont->drawStringWrapped( s, textWrap );
    }
    glPopMatrix();

}

CINDER_APP_NATIVE( ChatApp, RendererGl )
