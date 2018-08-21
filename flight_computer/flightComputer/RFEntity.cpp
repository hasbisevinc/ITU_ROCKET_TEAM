#include "RFEntity.h"

RFEntity::RFEntity(){
	packetId = 0;
	time = 0;
	temperature = 0;
	pressure = 0;
	acc_x = 0;
	acc_y = 0;
	acc_z = 0;
	altitude = 0;
	state = 0;
	lat = 0;
	lon = 0;
}

RFEntity::~RFEntity() {}