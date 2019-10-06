

#include "ofApp.h"


//-------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0,0,0);

    //OSC setup
   sendTiles.setup(HOST, PORT);
   scanSend.setup(HOST,PORT2);

   //setup video/ grabber and allocate some memory to pixel objects

    //myMovie.load("smoke.avi");
    //width = myMovie.getWidth();
    //height = myMovie.getHeight();
    myGrabber.setDesiredFrameRate(30);

    myGrabber.setDeviceID(0);

    myGrabber.initGrabber(640, 480);

    width = myGrabber.getWidth();
    height = myGrabber.getHeight();

    xOffset = int(width / 12);
    yOffset = int(height / 4);

    //Frame diff
    myTexture.allocate(width,height,GL_RGB);
    
    pixelout.allocate(width, height, 1);
    lastPixels.allocate(width, height,3);

    //scanner
    scanData.allocate(1,width,3);
    bufferSize = width;

    
    
    
//    if (myMovie.isLoaded()){
//        myMovie.play();
//    }

   // cout << myMovie.getDuration() << endl;
    
    
    xTiles = 3;    yTiles = 3;
    tileWidth   =   width  / xTiles; // 3x3 pixel tiles
    tileHeight  =   height / yTiles;
    gridSize    =   xTiles * yTiles;
    cout << "gridsize " << gridSize << endl;
    tileAvs.reserve(gridSize+1);// reserve the vector memory in advance, might be a bit quicker
    tileAvs.clear();
    
    
    // *** allocate memory for loads of ofPixels
    
    tileTexture.allocate(tileWidth, tileHeight, GL_LUMINANCE);
    gradTex.allocate    (tileWidth, tileHeight, GL_LUMINANCE);
    
    tile.allocate        (tileWidth, tileHeight, 1);             //the tile


    
    graphStep = width/(float)gridSize;

}

//--------------------------------------------------------------
void ofApp::update(){
    //start counter
    counter += 1.0;

    //update video/grabber
    //myMovie.update();
    myGrabber.update();

   // if (myMovie.isFrameNew()) {
    if (myGrabber.isFrameNew()) {
        //pixels = myMovie.getPixels();           // If there's a new frame, get the pixels
        pixels = myGrabber.getPixels();

        //Scanner
        ofImage tempImage;
        tempImage.setFromPixels(myGrabber.getPixels());
        //tempImage.mirror(false, true);
        tempImage.setImageType(OF_IMAGE_GRAYSCALE);
        frames.push_front(tempImage);
        //get a line crop
        ofImage tempCrop;
        tempCrop.cropFrom(frames[0], 0, (int)counter % height, width, 1);
        frameCrop.push_front(tempCrop);
        scanData = frameCrop[0].getPixels();

        //OSC magic
        for (int i = 0;i < width; i ++) {
            pixBuf[i] = scanData[i];

            sendScan = new ofxOscMessage;
            sendScan->setAddress("/scan");
            sendScan->addIntArg(pixBuf[i]);
            scanSend.sendMessage(*sendScan, false);
            delete sendScan;

        }

        //endScanner

        // frame difference on Red channel... motion detection from class examples code
        for (int i = 0; i < width; i++){
            for (int j = 0; j < height; j++) {              
                pixelout[(j*width+i)]=abs((lastPixels[(j*width+i)*3+1])-(pixels[(j*width+i)*3+1]));
                lastPixels[(j*width+i)*3+1]=pixels[(j*width+i)*3+1];
            }
        }
        // end of new frame loop
        //  make tiles from the 1 channel frame-diffed image       
        for (int _row=0; _row<yTiles;_row++){
            for (int _tileNum=0; _tileNum<xTiles; _tileNum++){               
              grid.push_back(makeTile(pixelout, _row, _tileNum));     // makes tiles, and push them onto a deque
                if (grid.size()>gridSize) {                                 //  circular buffer of tiles.
                    grid.pop_front();
                }
            }
        }
// get the average difference for each tile in the grid and store them in a vector
            int cnt = 0;
            
            for (const ofPixels &tile : grid){              //loop through all the tiles in the grid deque
                tileAvs[cnt] = getTileAverage(tile);        // average movement in the tile for graphs and audio
                cnt++;
                cnt=cnt % gridSize;

            trigs.push_back(cnt);
            if(trigs.size()>gridSize) trigs.pop_back();

            }
            //change this number to increase/decrease the sensitivity of the triggers
            float sens = 4;
            for(int i = 0; i < trigs.size(); i ++){
                if(tileAvs[i]>sens) trigs[i] = 1;
                else{ trigs[i] = 0;}
                trigTest[i] = trigs[i];

                //setup OSC messages for each tile
                tileSend = new ofxOscMessage();
                tileSend->setAddress("/tile/" + ofToString(i));
                tileSend->addIntArg(trigTest[i]);
                sendTiles.sendMessage(*tileSend, false);
                delete tileSend;


            }
        
        myTexture.allocate(pixelout);
        tileTexture.allocate(pixels);
    }
    if(frames.size() >= bufferSize -1){
        frames.pop_back();
        frameCrop.pop_back();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    if(frames.size() > 0){
        frames[0].draw(width + xOffset,0 + yOffset);

    myTexture.draw(xOffset, yOffset, width, height);

      for (int w = 0; w < xTiles; w ++) {
          for (int h = 0; h < yTiles; h ++) {
            ofNoFill();
            ofDrawRectangle(xOffset + w * tileWidth ,yOffset + h * tileHeight , tileWidth, tileHeight);
         }
      }
    }
//      for (int i = 0;i < gridSize; i ++) {
//          ofDrawBitmapString(trigs[i], )

//      }
      ofDrawBitmapString(trigs[0], tileWidth/2, tileHeight);
      ofDrawBitmapString(trigs[1], tileWidth + tileWidth/2, tileHeight);
      ofDrawBitmapString(trigs[2], tileWidth + tileWidth + tileWidth/2, tileHeight);

      ofDrawBitmapString(trigs[3], tileWidth/2, tileHeight * 2);
      ofDrawBitmapString(trigs[4], tileWidth + tileWidth/2, tileHeight * 2);
      ofDrawBitmapString(trigs[5], tileWidth + tileWidth + tileWidth/2, tileHeight * 2);

      ofDrawBitmapString(trigs[6], tileWidth/2, tileHeight * 3);
      ofDrawBitmapString(trigs[7], tileWidth + tileWidth/2, tileHeight * 3);
      ofDrawBitmapString(trigs[8], tileWidth + tileWidth + tileWidth/2, tileHeight * 3);

      //cout << trigs[0] << endl;

    //myMovie.draw(0,height,width,height);
    //myGrabber.draw(0,height,width,height);

      if(pixBuf[0] > 0){
         ofPushStyle();
         for (int i = 0; i < width; i ++) {
             ofSetColor(0,255,0,80);
             ofSetLineWidth(1);
             ofDrawLine(xOffset + width + i,yOffset + (int)counter % height , xOffset + width + i,yOffset + 1 + ((int)counter % height) - ((255 % pixBuf[i])));
             //cout << pixVals[i] << endl;


         }
         ofPopStyle();
         }



}

//--------------------------------------------------------------

float ofApp::getTileAverage(const ofPixels &tilePix){  // takes in a reference to the pixel tiles to avoid copying data.
                                                        //Can turn ofPixels char*'s into floats for maximillian
 
    size_t arrayLength = tileWidth *  tileHeight;
    
    float sum = 0;
    
    for (int i=0;i<arrayLength;i++){
        sum += tilePix[i];
    }
    
    return sum / arrayLength;       // just returns a value
}

//--------------------------------------------------------------

ofPixels ofApp::makeTile(ofPixels& imagePixels, int rowNum, int tileNum){
    
    size_t numChannels = imagePixels.getNumChannels();      // in case a colour image comes in
    
    size_t arrayWidth = imagePixels.getWidth()*numChannels; // total width of the frame in array elements
    size_t rowStride = arrayWidth * tileHeight;             // the number of index positions to jump to get to the first                            pixel in the next row of tiles.
    
    
// Loop through the image, make grid of tiles
  
    
    for (int i=0; i<tileHeight; i++){
        
        int tileCol = i*tileWidth;
        
        for (int j=0; j<tileWidth; j++){
        
        
        tile.allocate(tileWidth, tileHeight, 1);
        tile[tileCol+j] = imagePixels[(i*arrayWidth + rowStride*rowNum) + ((j+tileWidth*tileNum)*numChannels)];
        }
    }
    
    return tile;
   // grid.push_back(tile); // returns pixel values
}

//--------------------------------------------------------------


void ofApp::keyPressed(int key){
    

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
