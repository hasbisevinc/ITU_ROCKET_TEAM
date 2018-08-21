#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "MPU9250.h"

#include "RFEntity.h"
#include "RFMapper.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

RFEntity rfEntity;
RFMapper rfMapper;

MPU9250 mpu = MPU9250();

Adafruit_BMP280 bme;
double referenceLevel = 1013.25;

int16_t packetId = 0;

void setup(){
  Serial.begin(9600);
  mpu.begin();

  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    //TODO check here. should we use while(1)
    while (1);
  }
}

void loop(){
  rfEntity.packetId = packetId;
  rfEntity.time = 2;
  rfEntity.state = 9;
  rfEntity.lat = 10;
  rfEntity.lon = 11;

  setPressureAndAltitude();
  setTemperature();
  setAcc();

  sendData(rfMapper.map(rfEntity));
  
  increasePacketId();
  delay(1000);
}

void sendData(String message) {
  Serial.print(message);
}

void increasePacketId() {
  packetId ++;
}

void setTemperature() {
  //bme.readTemperature()
  rfEntity.temperature = mpu.get_temp();
}

void setPressureAndAltitude() {
  rfEntity.pressure = bme.readPressure();
  rfEntity.altitude = bme.readAltitude(referenceLevel);
}

void setAcc() {
  mpu.set_accel_range(RANGE_4G);
  mpu.get_accel_g();
  rfEntity.acc_x = mpu.x_g;
  rfEntity.acc_y = mpu.y_g;
  rfEntity.acc_z = mpu.z_g;
}

