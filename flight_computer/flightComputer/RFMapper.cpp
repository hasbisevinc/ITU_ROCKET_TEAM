#include "RFMapper.h"

RFMapper::RFMapper(){
}

RFMapper::~RFMapper(){}

String RFMapper::map(RFEntity entity){
	String packetId = String(entity.packetId);
	String time = String(entity.time,1);
  float temp = entity.temperature;
	String temperature = String(temp / 100, 1);
	String pressure = String(entity.pressure,1);
	String acc_x = String(entity.acc_x, 2);
	String acc_y = String(entity.acc_y, 2);
	String acc_z = String(entity.acc_z, 2);
	String altitude = String(entity.altitude,1);
	String state = String(entity.state);
	String lat = String(entity.lat,1);
	String lon = String(entity.lon,1);

	String message = "";
	
	message.concat(generatePacketId(packetId));
	message.concat(generateTime(time));
	message.concat(generateTemperature(temperature));
	message.concat(generatePressure(pressure));
	message.concat(generateAccX(acc_x));
	message.concat(generateAccY(acc_y));
	message.concat(generateAccZ(acc_z));
	message.concat(generateAltitude(altitude));
	message.concat(generateState(state));
	message.concat(generateLat(lat));
	message.concat(generateLon(lon));
  
  message.concat("\n");

	return message;
}


String RFMapper::commonGenerator(String start, String end, String value) {
	String message = "";
	message.concat(start);
	message.concat(value);
	message.concat(end);

	return message;
}

String RFMapper::generatePacketId(String value) {
	return commonGenerator("a", "b", value);
}

String RFMapper::generateTime(String value) {
	return commonGenerator("c", "d", value);
}
	
String RFMapper::generateTemperature(String value) {
	return commonGenerator("e", "f", value);
}
	
String RFMapper::generatePressure(String value) {
	return commonGenerator("g", "h", value);
}

String RFMapper::generateAccX(String value) {
	return commonGenerator("i", "j", value);
}

String RFMapper::generateAccY(String value) {
	return commonGenerator("k", "l", value);
}

String RFMapper::generateAccZ(String value) {
	return commonGenerator("m", "n", value);
}
	
String RFMapper::generateAltitude(String value) {
	return commonGenerator("o", "p", value);
}
	
String RFMapper::generateState(String value) {
	return commonGenerator("r", "s", value);
}
	
String RFMapper::generateLat(String value) {
	return commonGenerator("t", "u", value);
}
	
String RFMapper::generateLon(String value) {
	return commonGenerator("v", "x", value);
}
