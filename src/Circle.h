#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"

#include "ofxOpenCv.h"

class Circle {

public:

	Circle();

	void drawCircle();

	int x;
	int y;
	int z;
	int radius;
	ofColor color;

};
