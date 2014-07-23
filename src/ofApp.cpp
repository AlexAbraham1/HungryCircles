#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetLogLevel (OF_LOG_VERBOSE);

	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();

	grabber.setPixelFormat(OF_PIXELS_RGB);
	DEVICE_ID = 1;
	grabber.setDeviceID(DEVICE_ID);
	grabber.initGrabber(screenWidth, screenHeight);

	haarScaleFactor = 6;

	int haarWidth = int(screenWidth / haarScaleFactor);
	int haarHeight = int(screenHeight / haarScaleFactor);

	colorCv.allocate(screenWidth, screenHeight);
	colorCvSmall.allocate(haarWidth, haarHeight);
	grayCv.allocate(haarWidth, haarHeight);

	faceFinder.setup("left_eye.xml");
	faceFinder.setNeighbors(12);
	faceFinder.setScaleHaar(1.2);

	one_second_time = ofGetSystemTime();
	camera_fps = 0;
	frames_one_sec = 0;

	firstRun = true;
	invertColors = false;

	minRadius = 10;
	maxRadius = 30;
}

//--------------------------------------------------------------
void ofApp::update() {
	grabber.update();
	if (grabber.isFrameNew()) {
		frames_one_sec++;
		if (ofGetSystemTime() - one_second_time >= 1000) {
			camera_fps = frames_one_sec;
			frames_one_sec = 0;
			one_second_time = ofGetSystemTime();
		}

		//Detect Faces
		colorCv = grabber.getPixels();
		colorCvSmall.scaleIntoMe(colorCv, CV_INTER_NN);
		grayCv = colorCvSmall;
		faceFinder.findHaarObjects(grayCv);
		faces = faceFinder.blobs;

		if (firstRun) {
			int x = 0;
			int y = 0;

			while (y <= screenHeight) {

				int radius = int(
						minRadius
								+ static_cast<float>(rand())
										/ (static_cast<float>(RAND_MAX
												/ (maxRadius - minRadius))));

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
			//setZIndex();
			cleanupCircles();

		}

	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetHexColor(0xFFFFFF);
	grabber.draw(0, 0);
//	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
//		Circle * circle = circles[i];
//		ofColor color = grabber.getPixelsRef().getColor(screenWidth - circle->x,
//				circle->y);
//		if (invertColors) {
//			color.invert();
//		}
//		circle->color = color;
//		circle->drawCircle();
//	}
	ofPushStyle();
	for (std::vector<ofxCvBlob>::size_type i = 0; i != faces.size(); i++) {
		ofxCvBlob& face = faces[i];
		int x = (face.boundingRect.x + (face.boundingRect.width/2)) * haarScaleFactor;
		int y = (face.boundingRect.y + (face.boundingRect.height/2)) * haarScaleFactor;
		int radius1 = face.boundingRect.width;
		int radius2 = face.boundingRect.width/2;

		ofSetColor(255, 0, 255);
		ofCircle(x,y, radius1);
		ofSetColor(0, 0, 255);
		ofCircle(x, y, radius2);
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

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id) {
//	stringstream ss;
//	ss << ofGetElapsedTimef();
//	string date = ss.str();
//
//	ofSaveScreen(date + ".png");

	switchInvert();
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

bool ofApp::removeCircles(int x1, int y1, int x2, int y2, int radius1,
		int radius2) {
	float distance = ofDist(x1, y1, x2, y2);
	int radius = radius1 + radius2;

	if (radius >= distance) {
		return true;
	} else {
		return false;
	}
}

bool ofApp::isBlankSpace(int x, int y) {
	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
		Circle * circle = circles[i];

		float x2 = pow((circle->x - x), 2);
		float y2 = pow((circle->y - y), 2);
		float radius2 = pow((circle->radius), 2);

		if ((x2 + y2) < (radius2)) {
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

void ofApp::switchInvert() {
	invertColors = !invertColors;
}

void ofApp::setZIndex() {
	for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
		Circle * circle = circles[i];

		int zIndex = int(
				0
						+ static_cast<float>(rand())
								/ (static_cast<float>(RAND_MAX
										/ (circles.size() - 0)))) % 200;

		circle->z = zIndex;
	}
}
