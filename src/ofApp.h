#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxJSON.h"
#include "GLFW/glfw3.h"
#include "ofxNDI.h" // NDI classes
#include "ofxArtnet.h"


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		// void keyPressed(int key);
        void keyPressed(ofKeyEventArgs& keyArgs);
        void keyReleased(ofKeyEventArgs& keyArgs);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);

    void jsonLoad(string path);
    void createInputsList();
    void jsonSave();
    void printFramerate();
    void drawFramerate();
    void loadInput();

    // control variables
    bool ctrlPressed = false;
    bool shiftPressed = false;
    bool altPressed = false;
    float lastMouseXPos;
    float lastMouseYPos;

	ofxJSONElement json;
	ofxImGui::Gui gui;
    ofVideoGrabber vidGrabber;
    ofxNDIreceiver ndiReceiver;
    bool hasNDILoaded = false;
    bool showFramerate = false;
    string glType;

	// data in config
    bool hideConfig=false;
    bool hideMaptest = false;
    bool hidePreview = false;
    bool setResolutions = false;
    bool sendArtSync = false;
    int inputWidth;
    int inputHeight;
    int framerate=30;
    int numberStrips=9;
    int selectedInputIndex = 0;
    int canvasWidth=5000;
    int canvasHeight=5000;
    string artnetIp = "127.0.0.1";
    int artnetPort = 6454;
    string artsyncIp = "127.0.0.1";
    int artsyncPort = 6454;
    int spacing_index = 0;

    // data generated or defaulted
    int grid_index = 0;
    int stripFocus = 0;
    bool isFullscreen;
    int stripHeadRadius = 30;
    int ledRadius = 10;

    // strip data
    struct stripData{
        int numLeds;
        int xPos;
        int yPos;
        int angle;
        float xPosRel;
        float yPosRel;
        float ledSpaceX;
        float ledSpaceY;


        stripData() : numLeds(60), xPos(0), yPos(0), angle(0) {}
    };
    vector<stripData> stripDataList;

    void drawLineAndCircle(stripData thisStripData, string text,bool isFocus=false);

    // input devices
    struct inputType {
        string type;
        int typeId;
        string name;
    };

    vector<inputType> videoInputs;


    ofFbo innerFbo;
    ofPixels innerfboPixels;
    ofFbo mapTest;
    ofImage img;
    ofTrueTypeFont font;
    ofTrueTypeFont fpsFont;

    int numPixels;
    vector<ofColor> colorList;
    vector<ofPixels> artnetData;
    ofxArtnetSender artnet;
    ofxArtnetSender artsync;

    vector<string> testcards;
    
    const char* grid_options[5] = { "100", "50", "10", "5", "1" };
    const char* spacing_options[3] = { "1/30", "1/60", "1/144"};
    float spacing_values[3] = {33.33,16.66,6.944};

    ofTexture ndiTexture;
};
