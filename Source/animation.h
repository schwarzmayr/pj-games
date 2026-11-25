#pragma once

#include "timer.h"

class Animation
{
private:
	Timer timer;
	int frameCount;
public:
	Animation() : timer(0), frameCount(0) {}
	Animation(int frameCount, float length) : frameCount(frameCount), timer(length) {
		
	}

	float getLength() const { return timer.getLength(); }

	// Setting this function to int ensures a whole number for the current frame
	int currentFrame() const {
		// Gets the percentrage of time passed and multiplying over the frame count (number of frames in the pic)
		// gives us the current frame. i.e. idle.png has 8 frames so it would pick one of the eight to show
		return static_cast<int>(timer.getTime() / timer.getLength() * frameCount);
	}

	void step(float deltaTime) {
		timer.step(deltaTime);
	}
};