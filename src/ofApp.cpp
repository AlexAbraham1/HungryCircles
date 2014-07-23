#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetLogLevel (OF_LOG_VERBOSE);

	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();

	grabber.setPixelFormat(OF_PIXELS_RGB);
	DEVICE_ID = 0;
	grabber.setDeviceID(DEVICE_ID);
	grabber.initGrabber(screenWidth, screenHeight);

	haarScaleFactor = screenWidth / 176;

	int haarWidth = int(screenWidth / haarScaleFactor);
	int haarHeight = int(screenHeight / haarScaleFactor);

	colorCv.allocate(screenWidth, screenHeight);
	colorCvSmall.allocate(haarWidth, haarHeight);
	grayCv.allocate(haarWidth, haarHeight);

	haarFinder.setup("right_eye.xml");
	haarFinder.setNeighbors(1);
	haarFinder.setScaleHaar(2);

	firstRun = true;

	minRadius = 10;
	maxRadius = 50;

	maxTimesDrawn = 20;
}

//--------------------------------------------------------------
void ofApp::update() {
	grabber.update();
	if (grabber.isFrameNew()) {

		//Detect Faces
		colorCv = grabber.getPixels();
		colorCvSmall.scaleIntoMe(colorCv, CV_INTER_NN);
		grayCv = colorCvSmall;
		haarFinder.findHaarObjects(grayCv);
		eyes = haarFinder.blobs;

		if (firstRun) {
			int x = 0;
			int y = 0;

			while (y <= screenHeight) {

				int radius = int(minRadius+ static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/ (maxRadius - minRadius))));

				if (isBlankSpace(x, y)) {
					Circle * circle = new Circle();

					circle->x = x;
					circle->y = y;

					circle->radius = radius;

					circles.push_back(circle);
				}

				x += minRadius;

				if (x > screenWidth) {
					x = 0;
					y += minRadius;
				}

			}
			firstRun = false;
			fixZIndex();
			cleanupCircles();

			for(std::list<Circle*>::iterator circle = added_circles.begin(); circle != added_circles.end(); ++circle) {
				if ((*circle)->timesDrawn > maxTimesDrawn) {
					circle = added_circles.erase(circle);
				}
			}

		}

	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetHexColor(0xFFFFFF);
	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
		Circle * circle = circles[i];
		ofColor color = grabber.getPixelsRef().getColor(circle->x, circle->y);
		circle->color = color;
		circle->drawCircle();
	}

	for(std::list<Circle*>::iterator circle = added_circles.begin(); circle != added_circles.end(); ++circle) {
		if ((*circle)->timesDrawn <= maxTimesDrawn) {
			(*circle)->drawCircle();
		}
	}


	ofPushStyle();

	for (std::vector<ofxCvBlob>::size_type i = 0; i != eyes.size(); i++) {
		ofxCvBlob& eye = eyes[i];
		int x = (eye.boundingRect.x + (eye.boundingRect.width/2)) * haarScaleFactor;
		int y = (eye.boundingRect.y + (eye.boundingRect.height/2)) * haarScaleFactor;
		int radius1 = eye.boundingRect.width;

		ofSetColor(0, 0, 255);
		ofCircle(x, y, 100, radius1);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id) {
	touchCircle(x, y);
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id) {

}

//--------------------------------------------------------------
void ofApp::pause() {

}

//--------------------------------------------------------------
void ofApp::stop() {

}

//--------------------------------------------------------------
void ofApp::resume() {

}

//--------------------------------------------------------------
void ofApp::reloadTextures() {

}

//--------------------------------------------------------------
bool ofApp::backPressed() {
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed() {

}

//--------------------------------------------------------------
void ofApp::cancelPressed() {

}

void ofApp::cleanupCircles() {
	ofLog() << "cleanupCircles() started";
	stringstream ss;
	ss << circles.size();
	ofLog() << "NUMBER OF CIRCLES: " + ss.str();
	std::random_shuffle(circles.begin(), circles.end());
	ofLog() << "cleanupCircles() ended";
	ss.str("");
	ss << circles.size();
	ofLog() << "NUMBER OF CIRCLES: " + ss.str();
}

bool ofApp::isBlankSpace(int x, int y) {
	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {

		Circle * circle = circles[i];

		float x2 = pow((circle->x - x), 2);
		float y2 = pow((circle->y - y), 2);
		float radius2 = pow((circle->radius), 2);

		if ((x2 + y2) < (radius2)/2) {
			return false;
		}

	}
	return true;

}

void ofApp::switchCamera() {
	if (DEVICE_ID == 1) {
		DEVICE_ID = 0;
	} else {
		DEVICE_ID = 1;
	}

	grabber.close();

	grabber.setDeviceID(DEVICE_ID);
	grabber.initGrabber(screenWidth, screenHeight);
}

void ofApp::fixZIndex() {
	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
		Circle * circle = circles[i];

		float smallRadius = (minRadius + ((minRadius + maxRadius)/2))/2;
		if (circle->radius <= smallRadius) {
			circle->z = 50;
		}
	}
}

void ofApp::saveImage() {
	stringstream ss;
	ss << ofGetElapsedTimef();
	string date = ss.str();

	ofSaveScreen(date + ".png");
}

void ofApp::touchCircle(int x, int y) {
	int red = int(0+ static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/ (255 - 0))));
	int green = int(0+ static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/ (255 - 0))));
	int blue = int(0+ static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/ (255 - 0))));
	int radius = int(minRadius+ static_cast<float>(rand())/ (static_cast<float>(RAND_MAX/ (maxRadius - minRadius))));
	ofColor color = ofColor(red, green, blue);

	Circle * circle = new Circle();

	circle->x = x;
	circle->y = y;
	circle->radius = radius;
	circle->z = 200;
	circle->color = color;

	added_circles.push_back(circle);
}
