#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetLogLevel (OF_LOG_VERBOSE);
	font.loadFont("verdana.ttf",72);

	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();

	grabber = shared_ptr<ofVideoGrabber>(new ofVideoGrabber());
	grabber->setPixelFormat(OF_PIXELS_RGB);
	DEVICE_ID = 0;
	grabber->setDeviceID(DEVICE_ID);
	grabber->initGrabber(screenWidth, screenHeight);

	haarScaleFactor = screenWidth / 176;

	haarWidth = int(screenWidth / haarScaleFactor);
	haarHeight = int(screenHeight / haarScaleFactor);

	haarFinder.setup("right_eye.xml");
	haarFinder.setNeighbors(1);
	haarFinder.setScaleHaar(2);

	firstRun = true;

	minRadius = 10;
	maxRadius = 50;

	maxTimesDrawn = 10;

	orientation = OF_ORIENTATION_DEFAULT;

	percent = 50;

	showPercent = false;

	ofxAccelerometer.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
	grabber->update();
	if (grabber->isFrameNew()) {

		//Detect Eyes
		image.setFromPixels(grabber->getPixelsRef());
		fixImageRotation();

		colorCv = image.getPixels();
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
		ofColor color = grabber->getPixelsRef().getColor(circle->x, circle->y);
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

		int x;
		int y;


		if (orientation == OF_ORIENTATION_DEFAULT) {
			//Portrait Rightside Up
			x = (eye.boundingRect.y + (eye.boundingRect.height/2)) * haarScaleFactor;
			y = (haarHeight - (eye.boundingRect.x + (eye.boundingRect.width/2))) * haarScaleFactor;

			ofLog() << "ORIENTATION: Portrait";
		} else if (orientation == OF_ORIENTATION_180) {
			//Portrait Upside Down
			x = (haarWidth - (eye.boundingRect.y + (eye.boundingRect.height/2))) * haarScaleFactor;
			y = (eye.boundingRect.x + (eye.boundingRect.width/2)) * haarScaleFactor;

			ofLog() << "ORIENTATION: Portrait (Flipped)";
		} else if (orientation == OF_ORIENTATION_90_RIGHT) {
			//Landscape Right
			x = (haarWidth - (eye.boundingRect.x + (eye.boundingRect.width/2))) * haarScaleFactor;
			y = (haarHeight - (eye.boundingRect.y + (eye.boundingRect.height/2))) * haarScaleFactor;

			ofLog() << "ORIENTATION: Landscape (Flipped)";
		} else {
			//Landscape
			x = (eye.boundingRect.x + (eye.boundingRect.width/2)) * haarScaleFactor;
			y = (eye.boundingRect.y + (eye.boundingRect.height/2)) * haarScaleFactor;

			ofLog() << "ORIENTATION: Landscape";
		}

		int radius1 = eye.boundingRect.width;

		ofSetColor(0, 0, 255);
		ofCircle(x, y, 100, radius1);
	}
	ofPopStyle();

	if (showPercent) {
		ofSetHexColor(0xFFFFFF);
		stringstream ss;
		ss << percent;
		font.drawString(ss.str(), screenWidth/2, screenHeight/2);
		ss.str("");
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLog() << "Key Pressed: " << key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	ofLog() << "Key Released: " << key;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id) {
	percent = ((x*100)/screenWidth) + minRadius;
	if (percent > 100) {
		percent = 100;
	}
	showPercent = true;
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id) {
	percent = ((x*100)/screenWidth) + minRadius;
	if (percent > 100) {
		percent = 100;
	}
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id) {
	showPercent = false;
	maxRadius = percent;
	circles.clear();
	firstRun = true;

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id) {
	saveImage();
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
	string name = "../../../../DCIM/HungryCircles/" + ss.str() + ".png";
	ofSaveScreen(name);
	image.loadImage(name);
	fixImageRotation();
	image.saveImage(name);

	ofxAndroidToast("Image Saved");
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

void ofApp::fixImageRotation() {
	ofVec3f accel = ofxAccelerometer.getForce();
	int oX = floor(accel.x + 0.5);
	int oY = floor(accel.y + 0.5);

	if (oX == 1 && oY == 0) {
		image.rotate90(1);

		colorCv.allocate(screenHeight, screenWidth);
		colorCvSmall.allocate(haarHeight, haarWidth);
		grayCv.allocate(haarHeight, haarWidth);

		orientation = OF_ORIENTATION_DEFAULT;

	} else if (oX == -1 && oY == 0) {
		image.rotate90(3);

		colorCv.allocate(screenHeight, screenWidth);
		colorCvSmall.allocate(haarHeight, haarWidth);
		grayCv.allocate(haarHeight, haarWidth);

		orientation = OF_ORIENTATION_180;

	} else if (oX == 0 && oY == 1) {
		image.rotate90(2);

		colorCv.allocate(screenWidth, screenHeight);
		colorCvSmall.allocate(haarWidth, haarHeight);
		grayCv.allocate(haarWidth, haarHeight);

		orientation = OF_ORIENTATION_90_RIGHT;

	} else {
		colorCv.allocate(screenWidth, screenHeight);
		colorCvSmall.allocate(haarWidth, haarHeight);
		grayCv.allocate(haarWidth, haarHeight);

		orientation = OF_ORIENTATION_90_LEFT;
	}
}
