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
    
    void renderChat( const vector<string> * chat, int placement, string label );
    void renderType();
    
    Spacebrew::Connection * spacebrew;
    
    vector<string>      iWrote;
    vector<string>      uWrote;
    
    string              s;
    string              meLabel;
    string              uLabel;
    string              chatLabel;
    
    bool                receivedU;
    bool                receivedMe;
    
    ci::Font            chatFont;
    gl::TextureFontRef  chatTextureFont;
    ci::Font            labels;
    gl::TextureFontRef  lTextureFont;
    
    ci::Vec2f           mSize;
    gl::Texture         mTexture;
    
    
};

void ChatApp::prepareSettings( Settings * settings )
{
    settings->setFrameRate(60);
    settings->setWindowSize(1000, 1000);
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
    
    meLabel = "    Your Chat";
    uLabel = "    Their Chat";
    chatLabel = " Chat To Send";
    
    chatFont = Font( "Georgia", 20 );
    labels = Font( "Helvetica", 30);
    
    chatTextureFont = gl::TextureFont::create( chatFont );
    lTextureFont = gl::TextureFont::create( labels );
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
    
    //rendering the chats and current typing
    renderChat( &iWrote, 1, meLabel ); //myChat
    renderChat( &uWrote, 7, uLabel ); //yourChat
    renderType();
    
}

void ChatApp::renderType()
{
    glPushMatrix();
    {
        gl::translate( Vec2f( getWindowWidth()/2 - ((getWindowWidth()/5+1)/2), getWindowHeight()/2 ) );
        
        //Drawing the chat box
        gl::color( ColorA( 0.5f, 0.5f, 0.6f, 1.0f) );
        Rectf typeBack( 0, 0, getWindowWidth()/5, getWindowHeight()/5 );
        gl::drawSolidRect(typeBack);
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
        gl::lineWidth(1);
        gl::drawStrokedRect(typeBack);
        
        //Drawing the Label of your typing
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
        Rectf chatLabelWrap = Rectf( 0, -(lTextureFont->getAscent()/3), getWindowWidth()/5+1, lTextureFont->getAscent() );
        lTextureFont->drawStringWrapped( chatLabel, chatLabelWrap );
        
        //Drawing what you're actually writing real time
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
        Rectf textWrap = Rectf( 0, chatFont.getAscent(), getWindowWidth()/5+1, chatTextureFont->getAscent() );
        chatTextureFont->drawStringWrapped( s, textWrap );
    }
    glPopMatrix();
}

void ChatApp::keyDown( KeyEvent event)
{
    //This receives the keys from your keyboard and you can type until you want
    //to send by pressing the enter or return key
    if (event.getCode() != KeyEvent::KEY_RETURN && event.getCode() != KeyEvent::KEY_BACKSPACE) {
        s += event.getChar();
        cout << event.getCode();
    } else if (event.getCode() == KeyEvent::KEY_BACKSPACE) {
        s.pop_back();
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

void ChatApp::renderChat( const vector<string> *chat, int placement, string label )
{
    //draw background and text
    glPushMatrix();
    {
        gl::translate( Vec2f( getWindowWidth()/10 * placement, 0 ) );
        
        //Draw the Background box
        gl::color( ColorA( 0.2f, 0.2f, 0.3f, 1.0f ) );
        Rectf chatBack( 0, 0, getWindowWidth()/5, getWindowHeight() );
        gl::drawSolidRect(chatBack);
        
        //Contain the chat box with two white lines
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
        gl::lineWidth(5);
        gl::drawLine( Vec2f( -5, 0 ), Vec2f( -5, getWindowHeight() ) );
        gl::drawLine( Vec2f( (getWindowWidth()/5) + 5, 0 ), Vec2f( (getWindowWidth()/5) + 5, getWindowHeight() ) );
        
        //Draw Chat Label (i.e. Your Chat, Their Chat) at the bottom of the screen
        glPushMatrix();
        {
            gl::translate( 0, getWindowHeight() - ( lTextureFont->getAscent()/2 ) );
            gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f) );
            Rectf labelWrap = Rectf( 0, 0, getWindowWidth()/5+1, lTextureFont->getAscent() );
            lTextureFont->drawStringWrapped( label, labelWrap );
        }
        glPopMatrix();
        
        //Draw all of the chat's 
        glPushMatrix();
        {
            int j = 0;
            float k = chat->size();
            
            gl::translate( Vec2f( 0, (getWindowHeight()/4) * 2.5 ) );
            
            for (vector<string>::const_reverse_iterator rit = chat->rbegin(); rit != chat->rend(); rit++) {
                
                gl::color( ColorA( 1.0f, 1.0f, 1.0f, (float)(k/chat->size()) ) );
                
                Rectf chatWrap = Rectf( 0, 0, getWindowWidth()/5+1, chatTextureFont->getAscent() );
                j = chatTextureFont->measureStringWrapped(*rit, chatWrap).y;
                
                gl::translate( Vec2f( 0, -j ) );

                chatTextureFont->drawStringWrapped( *rit, chatWrap );

                k--;
            }
        }
        glPopMatrix();
    }
    glPopMatrix();
}



CINDER_APP_NATIVE( ChatApp, RendererGl )
