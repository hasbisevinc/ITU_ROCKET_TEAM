#ifndef RFENTITIY_H
#define RFENTITIY_H

class RFEntity { 
public:
	double packetId;
	double time;
	float temperature;
	double pressure;
	double acc_x;
	double acc_y;
	double acc_z;
	double altitude;
	int state;
	double lat;
	double lon;

	RFEntity();
	~RFEntity();
};

#endif