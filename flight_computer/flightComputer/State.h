#ifndef STATE_H
#define STATE_H

#include <arduino.h>

class State { 
public:
	int mainState;
	int gpsState;
	int hizState;

	State();
	~State();
};

#endif
