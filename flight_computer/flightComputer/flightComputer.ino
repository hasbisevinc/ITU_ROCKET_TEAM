#include "RFEntity.h"
#include "RFMapper.h"

RFEntity rfEntity;
RFMapper rfMapper;

void setup(){
  Serial.begin(9600);
}

void loop(){
  rfEntity.packetId = 1;
  rfEntity.time = 2;
  rfEntity.temperature = 3;
  rfEntity.pressure = 4;
  rfEntity.acc_x = 5;
  rfEntity.acc_y = 6;
  rfEntity.acc_z = 7;
  rfEntity.altitude = 8;
  rfEntity.state = 9;
  rfEntity.lat = 10;
  rfEntity.lon = 11;

  Serial.print(rfMapper.map(rfEntity));
  delay(1000);
}
