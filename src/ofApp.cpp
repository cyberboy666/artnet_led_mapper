#include "ofApp.h"
#include "GLFW/glfw3.h"

void ofApp::setup(){

    font.load("arial.ttf", 50);

    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetFrameRate(framerate);
    jsonLoad("config.json");

    if(!setResolutions){
        inputWidth = ofGetScreenWidth();
        inputHeight = ofGetScreenHeight();
    }

    img.allocate(inputWidth, inputHeight, OF_IMAGE_COLOR);
    innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
    mapTest.allocate(inputWidth, inputHeight, GL_RGB);
    ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
        
    gui.setup(nullptr, true, ImGuiConfigFlags_ViewportsEnable );

    testcards = {"testcard_01.png", "testcard_grid.jpg"};

    createInputsList();
    loadInput();

    if(stripDataList.size() == 0){
        stripData thisStrip;
        stripDataList.push_back(thisStrip);
    }

    if(isFullscreen){
        ofLog() << "setting main to full screen..";
        ofSetFullscreen(true);
    }

    artnet.setup(artnetIp, artnetPort);
    artsync.setup(artsyncIp, artsyncPort);
    
}

void ofApp::createInputsList(){

    videoInputs = {};

    for (size_t i = 0; i < testcards.size(); i++)
    {
        inputType imageInput;
        imageInput.type = "TESTCARD";
        imageInput.typeId = i;
        imageInput.name = "TESTCARD_" + ofToString(i);
        videoInputs.push_back(imageInput); 
    }

    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable && devices[i].deviceName.find("bcm2835-isp") ==  std::string::npos){
            //log the device
            inputType videoInput;
            videoInput.type = "VIDEO";
            videoInput.typeId = devices[i].id ;
            videoInput.name = "VIDEO_" + ofToString(devices[i].id) + " :" + devices[i].deviceName;
            videoInputs.push_back(videoInput);            
        }
    }

    ndiReceiver.ReceiveImage(ndiTexture);

    try {
        if(ndiReceiver.GetNDIversion() == ""){ hasNDILoaded = false;}
        else{ hasNDILoaded = true;}
    } catch (...) { hasNDILoaded = false;}

    ofLog() << "hasNDILoaded: " << hasNDILoaded;
    
    int nsenders = ndiReceiver.GetSenderCount();
    ofLog() << "ndiReceiver.GetSenderCount(): " << ndiReceiver.GetSenderCount();
    for(int i = 0; i < nsenders; i++){
       inputType ndiInput;
        ndiInput.type = "NDI";
        ndiInput.typeId = i;
        ndiInput.name = "NDI_" + ofToString(i) + " : " + ndiReceiver.GetSenderName(i);
        videoInputs.push_back(ndiInput);
    }
    if(selectedInputIndex >= videoInputs.size()){selectedInputIndex = 0;}
}

void ofApp::drawLineAndCircle(stripData thisStripData, string text,bool isFocus){
    // calculate values
    float ledSpacing = spacing_values[spacing_index];
    float xPosRel = static_cast<float>(thisStripData.xPos) / canvasWidth * ofGetScreenWidth();
    float yPosRel = static_cast<float>(thisStripData.yPos) / canvasHeight * ofGetScreenHeight();
    float ledSpaceX = static_cast<float>(ledSpacing*cos(glm::radians(static_cast<float>(thisStripData.angle)))) / canvasWidth * ofGetScreenWidth();
    float ledSpaceY = static_cast<float>(ledSpacing*sin(glm::radians(static_cast<float>(thisStripData.angle)))) / canvasHeight * ofGetScreenHeight();
    // draw strip lines
    ofSetColor(100);
    ofSetLineWidth(10);
    ofDrawLine(xPosRel, yPosRel, xPosRel + ledSpaceX*(thisStripData.numLeds-1),yPosRel + ledSpaceY*(thisStripData.numLeds-1));
    // draw head circles
     if(isFocus){ofSetColor(255, 0, 25);}
     else{ofSetColor(0, 0, 255);}
     ofDrawCircle(xPosRel, yPosRel, stripHeadRadius);
    // calculate and draw led pixels
     for (int i = 0; i < thisStripData.numLeds; i++){
        int ledPosX = xPosRel + i*ledSpaceX;
        int ledPosY = yPosRel + i*ledSpaceY;
        ofColor pixelColor;
        if(ledPosX < innerfboPixels.getWidth() && ledPosY < innerfboPixels.getHeight()){
            pixelColor = innerfboPixels.getColor(ledPosX, ledPosY);
        }
        else{pixelColor = ofColor(0);}
        ofSetColor(pixelColor);
        ofDrawCircle(ledPosX, ledPosY, ledRadius);
        colorList.push_back(pixelColor);
    }
     ofSetColor(255,255,255);
    float numOffsetX = 50*cos(glm::radians(static_cast<float>(thisStripData.angle)));
    float numOffsetY = 50*sin(glm::radians(static_cast<float>(thisStripData.angle)));
    font.drawString(text, xPosRel  - font.stringWidth(text)/2 - numOffsetX, yPosRel + font.stringHeight(text)/2 - numOffsetY);
     
}


void ofApp::update(){

    innerFbo.begin();
        ofClear(255, 255, 255, 0); // Clear the FBO with a transparent background
        ofSetColor(255); // Set color to white
        if(videoInputs[selectedInputIndex].type == "VIDEO"){
            vidGrabber.update();
            vidGrabber.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
        else if(videoInputs[selectedInputIndex].type == "TESTCARD"){
            img.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
        else if(videoInputs[selectedInputIndex].type == "NDI"){
            ndiReceiver.ReceiveImage(ndiTexture);
            ndiTexture.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
    innerFbo.end();

    numPixels = 0;
    colorList = {};
    innerFbo.readToPixels(innerfboPixels);
    mapTest.begin();
        ofClear(0);
        for (int i = 1; i <= numberStrips; i++) {
            numPixels = numPixels + stripDataList[i-1].numLeds;
            bool isFocus = i == stripFocus;
            if(!hideMaptest){
                drawLineAndCircle(stripDataList[i-1], ofToString(i), isFocus);
            }
        }
    mapTest.end();

    // orginise artnet data into universes
    int numUniverses = numPixels / 170 + 1;
    artnetData.resize(numUniverses);
    for (int i = 0; i < numUniverses; i++) {
        artnetData[i].allocate(170, 1, OF_PIXELS_RGB);
    }

    int u = 0;
    int i = 0;
    while (i < colorList.size()) {
        if (i % 170 == 0 && i != 0) {u++;}
        if (u < artnetData.size()) {
            artnetData[u].setColor(i % 170, 0, colorList[i]);
        } else {
            ofLogError() << "Universe index out of bounds: " << u;
            break;
        }
    i++;
    }

    // send artnet data
    for(int i = 0; i < artnetData.size(); i++){
        ofxArtnetMessage artnetMessage(artnetData[i]);
        artnetMessage.setUniverse15(i);
        artnet.sendArtnet(artnetMessage);

        // for (int i = 0; i < 5; i++)
        // {
        //     int a = i +1;
        // }

    }
    if(sendArtSync){artsync.sendArtSync();}

}

//--------------------------------------------------------------
void ofApp::draw(){

    float max_canvas = static_cast<float>MAX(canvasWidth, canvasHeight);

    if(isFullscreen){
        if(fullscreenType == 1){
            ofClear(0);
            innerFbo.draw(0,0, ofGetScreenWidth(), ofGetScreenHeight());
            if(showFramerate){drawFramerate();}
            return;
        }
        else if(fullscreenType == 2){
            ofClear(0);
            mapTest.draw(0, 0, ofGetScreenWidth() * (canvasWidth/ max_canvas ), ofGetScreenHeight() * (canvasHeight/ max_canvas ));
            if(showFramerate){drawFramerate();}
            return;
        }
    }

    printFramerate();

    if(!hidePreview){   
        innerFbo.draw(600,400, ofGetScreenWidth() / 3, ofGetScreenHeight() / 3);
    }

    if(!hideMaptest){
        mapTest.draw(600, 20, (ofGetScreenWidth() / 3) * (canvasWidth/ max_canvas ), (ofGetScreenHeight() / 3) * (canvasHeight/ max_canvas ));
    }

    
    ImGuiWindowFlags window_flags = 0;
    // window_flags |= ImGuiWindowFlags_NoTitleBar;
    // window_flags |= ImGuiWindowFlags_NoScrollbar;
    // window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNav;
    // window_flags |= ImGuiWindowFlags_NoBackground;
    // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoDocking;
    // window_flags |= ImGuiWindowFlags_UnsavedDocument;

    gui.begin();
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, ofGetWindowHeight() - 150), ImGuiCond_FirstUseEver);
        ImGui::Begin("ARTNET_LED_MAPPER SETTINGS", NULL, window_flags);
        if (ImGui::CollapsingHeader("HELP")){
            ImGui::Text("MAPPING WITH A MOUSE:");
            ImGui::BulletText("CLICK on a STRIP HEAD to select it (BLUE/RED larger circle)");
            ImGui::BulletText("CLICK and drag a STRIP HEAD to position it");
            ImGui::BulletText("hold SHIFT and drag around to rotate selected STRIP");
            ImGui::BulletText("or RIGHT CLICK and drag around to rotate selected STRIP");
            ImGui::Separator();
            ImGui::Text("MAPPING WITH A KEYBOARD:");
            ImGui::BulletText("press ALT plus LEFT/RIGHT to select a STRIP");
            ImGui::BulletText("press ARROW KEYS to position it");
            ImGui::BulletText("press SHIFT plus UP/DOWN to rotate it");
            ImGui::BulletText("press SHIFT plus LEFT/RIGHT to subtract/add leds");
            ImGui::BulletText("press ALT plus UP/DOWN to set grid size (how much each press moves)");
            ImGui::Separator();
            ImGui::Text("OTHER KEYBOARD SHORTCUTS:");
            ImGui::BulletText("press F to toggle ON/OFF FULLSCREEN");

        }

        ImGui::SeparatorText("MODE SETTING");

        if(ImGui::Button("save config")){
            jsonSave();
        }
        ImGui::SameLine();
        if(ImGui::Button("load config")){
            jsonLoad("config.json");
        }
        ImGui::SameLine();
        if(ImGui::Button("refresh inputs")){
            createInputsList();
        }
        if(!hasNDILoaded){
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("NDI SDK MAY NOT BE LOADED");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        }

        const char** input_names = new const char*[videoInputs.size()];
        for (int i = 0; i < videoInputs.size(); i++)
        {
            input_names[i] = videoInputs[i].name.c_str();
        }
        if(ImGui::Combo("VIDEO SOURCE", &selectedInputIndex, input_names, videoInputs.size())){
            loadInput();
        }

        if(setResolutions){
            if(ImGui::InputInt("FRAMERATE", &framerate)){
                vidGrabber.setDesiredFrameRate(framerate);
                ofSetFrameRate(framerate);
                loadInput();
            }

            if(ImGui::InputInt("INPUT WIDTH", &inputWidth) || ImGui::InputInt("INPUT HEIGHT", &inputHeight)){
                innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
                ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
                loadInput();
            }
        }

        string resolutionText = "INPUT RESOLUTION: " + ofToString(inputWidth) + "X" + ofToString(inputHeight);
        ImGui::Text(resolutionText.c_str());


        ImGui::Checkbox("HIDE MAPTEST", &hideMaptest);
        ImGui::SameLine();
        ImGui::Checkbox("HIDE PREVIEW", &hidePreview);
        ImGui::SameLine();
        if(ImGui::Checkbox("OVERWRITE DEFAULTS", &setResolutions) && !setResolutions){
            inputWidth = ofGetScreenWidth();
            inputHeight = ofGetScreenHeight();
            framerate = 30;
            innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
            ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
            vidGrabber.setDesiredFrameRate(framerate);
            ofSetFrameRate(framerate);
            loadInput();
        }
        if(ImGui::Checkbox("fullscreen (press f)", &isFullscreen)){
            // switch window to fullscreen
            ofSetFullscreen(isFullscreen);
        }
        ImGui::SameLine();
        ImGui::RadioButton("display ", &fullscreenType, 0); ImGui::SameLine();
        ImGui::RadioButton("preview ", &fullscreenType, 1); ImGui::SameLine();
        ImGui::RadioButton("maptest ", &fullscreenType, 2);

        // ImGui::Checkbox("show framerate (press r)", &showFramerate);
        ImGui::SeparatorText("LED STRIPS SETTING");

        ImGui::InputInt("NUMBER OF STRIPS", &numberStrips);
        if(numberStrips < stripDataList.size()){
            for (int i = 0; i < stripDataList.size() - numberStrips; i++) {
                stripDataList.pop_back();
            }
        }
        else if(numberStrips > stripDataList.size()){
            for (int i = 0; i < numberStrips - stripDataList.size(); i++) {
            stripData thisStrip;
            stripDataList.push_back(thisStrip);
            }
        }

        // more strip settings going in here:

        ImGui::Combo("LED SPACINGS (mm)", &spacing_index, spacing_options, IM_ARRAYSIZE(spacing_options));

        char artnetIpChar[20];
        std::strncpy(artnetIpChar, artnetIpTemp.c_str(), sizeof(artnetIpChar) - 1);
        if(ImGui::InputText("TARGET IP", artnetIpChar, IM_ARRAYSIZE(artnetIpChar))){
            artnetIpTemp = artnetIpChar;
        }
        // ImGui::SameLine();

        ImGui::InputInt("TARGET PORT", &artnetPortTemp);

        bool updateNetworkSettings = artnetPortTemp != artnetPort || artnetIp != artnetIpTemp;
        if(updateNetworkSettings){
            ImVec4 buttonColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            }

        if(ImGui::Button("UPDATE TARGET IP AND PORT")){
            ofLog() << "artnetIpTemp is " << artnetIpTemp;
            bool isValidIp = artnet.setup(artnetIpTemp, artnetPortTemp);
            ofLog() << "isValidIp " << isValidIp;
            if(isValidIp){
                artnetIp = artnetIpTemp;
                artnetPort = artnetPortTemp;
            }
            else{
                artnet.setup(artnetIp, artnetPort);
                artnetIpTemp = artnetIp;
                artnetPortTemp = artnetPort;
            }
        }

        if(updateNetworkSettings){ImGui::PopStyleColor();}

        // gateway ?

        // output framerate ...

        // artsync on/off 
        if(ImGui::Checkbox("SEND ARTSYNC", &sendArtSync)){
            artsyncIp = artnetIp;
            artsyncPort = artnetPort;
        }
                // ImGui::SameLine();
        //       
        if(sendArtSync){

            char artsyncIpChar[20];
            std::strncpy(artsyncIpChar, artsyncIpTemp.c_str(), sizeof(artsyncIpChar) - 1);
            if(ImGui::InputText("ARTSYNC IP", artsyncIpChar, IM_ARRAYSIZE(artsyncIpChar))){
                artsyncIpTemp = artsyncIpChar;
            }

            ImGui::InputInt("ARTSYNC PORT", &artsyncPortTemp);

            bool updateNetworkSettings = artsyncPortTemp != artsyncPort || artsyncIp != artsyncIpTemp;
            if(updateNetworkSettings){
                ImVec4 buttonColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
                }

            if(ImGui::Button("UPDATE ARTSYNC IP AND PORT")){
                ofLog() << "artsyncIpTemp is " << artsyncIpTemp;
                bool isValidIp = artsync.setup(artsyncIpTemp, artsyncPortTemp);
                ofLog() << "isValidIp " << isValidIp;
                if(isValidIp){
                    artsyncIp = artsyncIpTemp;
                    artsyncPort = artsyncPortTemp;
                }
                else{
                    artsync.setup(artsyncIp, artsyncPort);
                    artsyncIpTemp = artsyncIp;
                    artsyncPortTemp = artsyncPort;
                }
            }

            if(updateNetworkSettings){ImGui::PopStyleColor();}
        }

        ImGui::SeparatorText("CANVAS VALUES");
        ImGui::InputInt("CANVAS WIDTH (mm)", &canvasWidth);
        ImGui::InputInt("CANVAS HEIGHT (mm)", &canvasHeight);

        ImGui::SeparatorText("INTERFACE");
        if(ImGui::InputInt("SELECTED STRIP", &stripFocus)){
            if(stripFocus > numberStrips){stripFocus = 0;}
            else if(stripFocus < 0){stripFocus = numberStrips;}
        }
        ImGui::Combo("GRID SIZE (mm)", &grid_index, grid_options, IM_ARRAYSIZE(grid_options));

        ImGui::SeparatorText("STRIP VALUES");

        for (int i = 1; i <= numberStrips; i++) {

            if(ImGui::TreeNode(("STRIP " + ofToString(i)).c_str())){

                ImGui::InputInt(("STRIP " + ofToString(i) + " NUMBER LEDS").c_str(), &stripDataList[i-1].numLeds);
                ImGui::InputInt(("STRIP " + ofToString(i) + " X POS (mm)").c_str(), &stripDataList[i-1].xPos);
                ImGui::InputInt(("STRIP " + ofToString(i) + " Y POS (mm)").c_str(), &stripDataList[i-1].yPos);
                ImGui::InputInt(("STRIP " + ofToString(i) + " ANGLE (degrees)").c_str(), &stripDataList[i-1].angle);
            }
        }

    ImGui::End();
    gui.end();
}



void ofApp::loadInput(){
    if(videoInputs[selectedInputIndex].type == "VIDEO"){
        if(vidGrabber.isInitialized()){vidGrabber.close();}
        vidGrabber.setDesiredFrameRate(framerate);
        vidGrabber.setDeviceID(videoInputs[selectedInputIndex].typeId);
        vidGrabber.initGrabber(inputWidth, inputHeight);
    }
    else{vidGrabber.close();}
    if(videoInputs[selectedInputIndex].type == "TESTCARD"){
        
        img.load(testcards[videoInputs[selectedInputIndex].typeId]);
    }
    
    if(videoInputs[selectedInputIndex].type == "NDI"){
        ndiReceiver.SetSenderIndex(videoInputs[selectedInputIndex].typeId);
        
    }
    else{
        ndiReceiver.ReleaseReceiver();
    }
}

void ofApp::drawFramerate(){
    if(showFramerate){
        stringstream info;
        float framerate = roundf(ofGetFrameRate());
        info << "FPS: " << framerate ;
        ofPushMatrix();
            ofScale(2, 2);
            ofDrawRectangle(font.getStringBoundingBox(info.str(), 0, font.stringHeight(info.str())));
            ofSetColor(255,0,0);
            font.drawString(info.str(), 0, font.stringHeight(info.str()));
            ofSetColor(255);
        ofPopMatrix();
    }
}

void ofApp::printFramerate(){
    stringstream info;
    float framerate = roundf(ofGetFrameRate());
    info << "FPS: " << framerate ;
    ofDrawBitmapStringHighlight(info.str(), 600, 12, ofColor::black, ofColor::yellow);

}

void ofApp::keyPressed(ofKeyEventArgs& keyArgs){

    if (keyArgs.key == OF_KEY_CONTROL) {
        ctrlPressed = true;
    }
    if (keyArgs.key == OF_KEY_SHIFT) {
        shiftPressed = true;
    }
    if (keyArgs.key == OF_KEY_ALT) {
        altPressed = true;
    }

    int gridSize = ofToInt(grid_options[grid_index]);
    // set position with arrow keys
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && !shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus-1].yPos -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && !shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus-1].yPos += gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && !shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus-1].xPos -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && !shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus-1].xPos += gridSize; 
        }
    }
    // set size with ctrl + arrow keys
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus - 1].angle -= gridSize*72/400; 
        }
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus - 1].angle += gridSize*72/400; 
        }
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus - 1].numLeds -= 1; 
        }
    }
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && shiftPressed && !altPressed){
        if(stripFocus > 0){
            stripDataList[stripFocus - 1].numLeds += 1; 
        }
    }
    // set focus and grid with alt + arrow keys
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && !shiftPressed && altPressed){
        if(stripFocus >= stripDataList.size()){stripFocus = 0;}
        else{ stripFocus++;}
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && !shiftPressed && altPressed){
        if(stripFocus <= 0){stripFocus = stripDataList.size();}
        else{ stripFocus--;}
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && !shiftPressed && altPressed){
        if(grid_index == IM_ARRAYSIZE(grid_options) - 1){ grid_index = 0; }
        else{ grid_index++; }
    }
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && !shiftPressed && altPressed){
        if(grid_index == 0){ grid_index = IM_ARRAYSIZE(grid_options) - 1; }
        else{ grid_index--; }
    }

    if (keyArgs.key == 'f' || keyArgs.key == 'F') {
            isFullscreen = !isFullscreen;
            ofSetFullscreen(isFullscreen);
        }
    if (keyArgs.key == 'r' || keyArgs.key == 'R') {
        showFramerate = !showFramerate;
    }
}

void ofApp::keyReleased(ofKeyEventArgs& keyArgs) {
    // Check if Ctrl key is released
    if (keyArgs.key == OF_KEY_CONTROL) {
        ctrlPressed = false;
    }

    // Check if Shift key is released
    if (keyArgs.key == OF_KEY_SHIFT) {
        shiftPressed = false;
    }

    // Check if Alt key is released
    if (keyArgs.key == OF_KEY_ALT) {
        altPressed = false;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if(button == 1){
        isFullscreen = !isFullscreen;
        ofSetFullscreen(isFullscreen);
        return;
    }
    if(button == 2){return;}
    if(hideMaptest){return;}
    ofRectangle fboBounds(600, 20, ofGetScreenWidth() / 3, ofGetScreenHeight() / 3);
    if (!fboBounds.inside(x, y)) {
        return; 
        }
    float max_canvas = static_cast<float>MAX(canvasWidth, canvasHeight);
    float xPos = static_cast<float>(x - 600) / (ofGetScreenWidth() / 3) * max_canvas;
    float yPos = static_cast<float>(y - 20) / (ofGetScreenHeight() / 3) * max_canvas;
    float headRadius =  static_cast<float>(stripHeadRadius / innerFbo.getWidth()) * max_canvas;

    bool isFocused = false;
    for (int i = stripDataList.size(); i >= 1; i--) {
        int distanceX = xPos - stripDataList[i-1].xPos;
        int distanceY = yPos - stripDataList[i-1].yPos;

        if (distanceX * distanceX + distanceY * distanceY < headRadius * headRadius){
            stripFocus = i;
            isFocused = true;
            break;
        }
    }
    if(!isFocused && !shiftPressed){stripFocus=0;}
    lastMouseXPos = xPos - stripDataList[stripFocus-1].xPos;
    lastMouseYPos = yPos - stripDataList[stripFocus-1].yPos;
}

void ofApp::mouseDragged(int x, int y, int button) {
    if(hideMaptest){return;}
    ofRectangle fboBounds(600, 20, ofGetScreenWidth() / 3, ofGetScreenHeight() / 3);
    if (!fboBounds.inside(x, y)) {
        return; 
        }
    float xPos = static_cast<float>(x - 600) / (ofGetScreenWidth() / 3) * canvasWidth;
    float yPos = static_cast<float>(y - 20) / (ofGetScreenHeight() / 3) * canvasHeight;

    if(!(shiftPressed || button == 2)){
        stripDataList[stripFocus-1].xPos = (xPos - lastMouseXPos);
        stripDataList[stripFocus-1].yPos = (yPos - lastMouseYPos);
    }
    else if(shiftPressed || button == 2){
        int newAngle = glm::degrees(glm::atan((yPos - stripDataList[stripFocus-1].yPos)/(xPos - stripDataList[stripFocus-1].xPos)));
        if(xPos < stripDataList[stripFocus-1].xPos){
            newAngle = newAngle + 180;
        }
        stripDataList[stripFocus-1].angle = newAngle;
    }

}

void ofApp::jsonLoad(string path){
    if (json.open(path)) {
        // Access configuration values
        hideMaptest = json["hide_maptest"].asBool();
        hidePreview = json["hide_preview"].asBool();
        setResolutions = json["set_resolutions"].asBool();
        sendArtSync = json["send_artsync"].asBool();
        inputWidth = json["input_width"].asInt();
        inputHeight = json["input_height"].asInt();
        framerate = json["framerate"].asInt();
        numberStrips = json["number_strips"].asInt();
        selectedInputIndex = json["selected_input_index"].asInt();
        canvasWidth = json["canvas_width"].asInt();
        canvasHeight = json["canvas_height"].asInt();
        artnetIp  = json["artnet_ip"].asString().c_str();
        artnetPort = json["artnet_port"].asInt();
        artsyncIp  = json["artsync_ip"].asString().c_str();
        artsyncPort = json["artsync_port"].asInt();
        spacing_index = json["spacing_index"].asInt();

        // Load tv_data values
        const ofxJSONElement& stripMappings = json["strip_mappings"];
        for (int i = 0; i < stripMappings.size(); i++) {
            stripData thisStrip;
            thisStrip.numLeds = stripMappings[i]["num_leds"].asInt();
            thisStrip.angle = stripMappings[i]["angle"].asInt();
            thisStrip.xPos = stripMappings[i]["x_pos"].asInt();
            thisStrip.yPos = stripMappings[i]["y_pos"].asInt();
            stripDataList.push_back(thisStrip);
        }
    }

}

void ofApp::jsonSave(){
        json["hide_maptest"] = hideMaptest;
        json["hide_preview"] = hidePreview;
        json["set_resolutions"] = setResolutions;
        json["send_artsync"] = sendArtSync;
        json["input_width"] = inputWidth;
        json["input_height"] = inputHeight;
        json["framerate"] = framerate;
        json["number_strips"] = numberStrips;
        json["selected_input_index"] = selectedInputIndex;
        json["canvas_width"] = canvasWidth;
        json["canvas_height"] = canvasHeight;
        json["artnet_ip"] = artnetIp;
        json["artnet_port"] = artnetPort;
        json["artsync_ip"] = artsyncIp;
        json["artsync_port"] = artsyncPort;
        json["spacing_index"] = spacing_index; 
    
        json["strip_mappings"] = {};
        for (int i = 0; i < stripDataList.size(); i++) {
            json["strip_mappings"][i]["num_leds"] = stripDataList[i].numLeds;
            json["strip_mappings"][i]["angle"] = stripDataList[i].angle;
            json["strip_mappings"][i]["x_pos"] = stripDataList[i].xPos;
            json["strip_mappings"][i]["y_pos"] = stripDataList[i].yPos;
        }

        json.save("config.json", true);
}