#include "RotaryDialer.h"

// https://github.com/markfickett/Rotary-Dial

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_NUMBER	-1

// Require DEBOUNCE_DELAY milliseconds between state changes. This value is
// noted in the class documentation.
#define DEBOUNCE_DELAY	0

RotaryDialer::RotaryDialer(int readyPin, int pulsePin) :
	pinReady(readyPin), pinPulse(pulsePin), hasCompletedNumber(false),
	state(WAITING)
{ }

void RotaryDialer::setup() {
	pinMode(pinReady, INPUT);
	pinMode(pinPulse, INPUT);
	digitalWrite(pinReady, HIGH);
	digitalWrite(pinPulse, HIGH);
	lastStateChangeMillis = millis();
}

bool RotaryDialer::changeStateIfDebounced(enum State newState) {
	unsigned long currentMillis = millis();
	if (currentMillis < lastStateChangeMillis) {
		// clock wrapped; ignore (but could figure it out in this case)
		lastStateChangeMillis = currentMillis;
		return false;
	} else if (currentMillis - lastStateChangeMillis > DEBOUNCE_DELAY) {
		state = newState;
		lastStateChangeMillis = currentMillis;
		return true;
	} else {
		return false;
	}
}

void RotaryDialer::completeDial() {
	if (!changeStateIfDebounced(WAITING)) {
		return;
	}
	if (number > 0 && number <= 10) {
		if (number == 10) {
			number = 0;
		}
		hasCompletedNumber = true;
	}
}

bool RotaryDialer::update(int pinReadyState, int pinPulseState) {

	switch(state) {
		case WAITING:
			if (pinReadyState == HIGH
				&& changeStateIfDebounced(LISTENING_NOPULSE))
			{
				hasCompletedNumber = false;
				number = 0;
			}
			break;
		case LISTENING_NOPULSE:
			if (pinReadyState == LOW) {
				completeDial();
			} else if (pinPulseState == LOW) {
				changeStateIfDebounced(LISTENING_PULSE);
			}
			break;
		case LISTENING_PULSE:
			if (pinReadyState == LOW) {
				completeDial();
			} else if (pinPulseState == HIGH
				&& changeStateIfDebounced(LISTENING_NOPULSE))
			{
				number++;
			}
			break;
	}
	return hasCompletedNumber;
}

bool RotaryDialer::hasNextNumber() {
	return hasCompletedNumber;
}

int RotaryDialer::getNextNumber() {
	if (hasCompletedNumber) {
		hasCompletedNumber = false;
		return number;
	} else {
		return NO_NUMBER;
	}
}
