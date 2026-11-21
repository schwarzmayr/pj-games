#pragma once

class Timer
{
private:
	float length, time;
	bool timeout;
public:
	Timer(float givenLength) : length(givenLength), time(0), timeout(false) {}

	void step(float deltaTime) {
		time += deltaTime;
		while (time >= length) {
			time -= length;
		}
	}

	bool isTimeout() const { return timeout; }
	float getTime() const { return time; }
	float getLength() const { return length; }
	void reset() { time = 0; }
};