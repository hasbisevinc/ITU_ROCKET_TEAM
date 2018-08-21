#ifndef RFENTITIY_H
#define RFENTITIY_H

#include <arduino.h>

class RFEntity { 
public:
	int16_t packetId;
	double time;
	int16_t temperature;
	float pressure;
	float acc_x;
	float acc_y;
	float acc_z;
	float altitude;
	int state;
	double lat;
	double lon;

	RFEntity();
	~RFEntity();
};

#endif
