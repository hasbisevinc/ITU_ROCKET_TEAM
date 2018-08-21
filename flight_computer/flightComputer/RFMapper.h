#ifndef RFMAPPER_H
#define RFMAPPER_H

#include "RFEntity.h"
#include "Arduino.h"

class RFMapper { 
public:
	RFMapper();
	~RFMapper();

	String map(RFEntity entity);

private:
	String generatePacketId(String value);
	String generateTime(String value);
	String generateTemperature(String value);
	String generatePressure(String value);
	String generateAccX(String value);
	String generateAccY(String value);
	String generateAccZ(String value);
	String generateAltitude(String value);
	String generateState(String value);
	String generateLat(String value);
	String generateLon(String value);
	String commonGenerator(String start, String end, String value);
};

#endif