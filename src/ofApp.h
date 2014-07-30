#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxAccelerometer.h"

#include "ofxOpenCv.h"
#include <vector>

#include "Circle.h";

class ofApp: public ofxAndroidApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);

	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	void touchCancelled(int x, int y, int id);
	void swipe(ofxAndroidSwipeDir swipeDir, int id);

	void pause();
	void stop();
	void resume();
	void reloadTextures();

	bool backPressed();
	void okPressed();
	void cancelPressed();

	void cleanupCircles();
	bool isBlankSpace(int x, int y);
	void switchCamera();
	void switchInvert();
	void fixZIndex();
	void saveImage();
	void touchCircle(int x, int y);
	void detectHandCircles(int xLow, int yLow, int xHigh, int yHigh);
	void fixImageRotation();

	shared_ptr<ofVideoGrabber> grabber;

	ofImage image;
	ofxCvColorImage colorCv;
	ofxCvColorImage colorCvSmall;
	ofxCvGrayscaleImage grayCv;
	ofxCvHaarFinder haarFinder;
	float haarScaleFactor;

	bool firstRun;
	bool invertColors;



	std::vector<Circle*> circles;
	std::vector<ofxCvBlob> eyes;

	std::vector<Circle*> added_circles;

	int minRadius;
	int maxRadius;
	int maxTimesDrawn;

	int DEVICE_ID;

	int screenWidth;
	int screenHeight;
	int haarWidth;
	int haarHeight;

	int orientation;

	int percent;
	int sidebarWidth;

	bool showPercent;

	ofTrueTypeFont font;

};
