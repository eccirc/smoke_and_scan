#pragma pack(16)


#include "ofMain.h"
#include "ofxOsc.h"


#define HOST "locahost"
#define PORT 12345
#define PORT2 23456

class ofApp : public ofBaseApp{
    
public:
    void        setup();
    void        update();
    void        draw();
    
    void        keyPressed  (int key);
    void        keyReleased(int key);
    void        mouseMoved(int x, int y );
    void        mouseDragged(int x, int y, int button);
    void        mousePressed(int x, int y, int button);
    void        mouseReleased(int x, int y, int button);
    void        windowResized(int w, int h);
    void        dragEvent(ofDragInfo dragInfo);
    void        gotMessage(ofMessage msg);
   

 /*                      RICHARDS CODE - TAHNK YOU RICHARD!!!
 * *********************                                     ********************
 */
    float       getTileAverage(const ofPixels& );            // returns the average difference in that tile
    ofPixels    makeTile(ofPixels&, int _row, int _tile);     // divides the image into tiles


    //ofVideoPlayer 		myMovie;
    ofVideoGrabber		myGrabber;
    
    ofPixels            pixels;
    ofPixels            tile;
    ofPixels            pixelout, lastPixels;
    
    vector<float>       tileAvs;
    deque<ofPixels>     grid;
    
    ofTexture           myTexture, tileTexture, gradTex;

    int                 width, height, tileHeight, tileWidth, tileCount, gridSize, xTiles, yTiles;
    int xOffset, yOffset;
    float               graphStep;
//********************************************************************************

    //Tile triggers and OSC

    vector<int> trigs;
    int trigTest[9];

    ofxOscSender sendTiles;

    //ofxOscMessage messages[9];
    //try with pointers..
    ofxOscMessage * tileSend;

    //FRAME SCAN//

    deque<ofImage> frames;
    deque<ofImage> frameCrop;

    int bufferSize;


    ofPixels scanData;

    //These need to be the same as the video/image width
    unsigned char pixBuf[640];
    //int pixVals[640];
    //create a variable for counting i.e. moving the scan line
    float counter;

    //Frame scan OSC setup
    ofxOscSender scanSend;
    ofxOscMessage * sendScan;


    
    

};
