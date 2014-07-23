#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"

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
	bool removeCircles(int x1, int y1, int x2, int y2, int radius1,
			int radius2);
	bool isBlankSpace(int x, int y);
	void switchCamera();
	void switchInvert();
	void setZIndex();

	ofVideoGrabber grabber;

	ofxCvColorImage colorCv;
	ofxCvColorImage colorCvSmall;
	ofxCvGrayscaleImage grayCv;

	ofxCvHaarFinder faceFinder;

	bool firstRun;
	bool invertColors;

	int *circleArray;
	float haarScaleFactor;

	std::vector<Circle*> circles;
	std::vector<ofxCvBlob> faces;
	std::vector<ofxCvBlob> eyes;

	int one_second_time;
	int camera_fps;
	int frames_one_sec;
	int minRadius;
	int maxRadius;
	int minZ;
	int maxZ;

	int DEVICE_ID;

	int screenWidth;
	int screenHeight;

};
