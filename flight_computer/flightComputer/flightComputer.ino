#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "MPU9250.h"

#include "RFEntity.h"
#include "RFMapper.h"
#include "State.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

#define SECURITY_BYTE 0x55

RFEntity rfEntity;
RFMapper rfMapper;
State state;

MPU9250 mpu = MPU9250();

Adafruit_BMP280 bme;

double referenceLevel;// = 1013.25;
long startTime;
bool rfReceivedData = false;
char receivedSecurityByte = 0;
char receivedStateByte = 0;
bool hasBeenLaunched = false;

int16_t packetId = 0;

void setup(){
  Serial.begin(9600);
  mpu.begin();

  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    //TODO check here. should we use while(1)
    while (1);
  }
  state.mainState = 0;

  // GPIO, sensor and system setup
  initSystem();

  // get pressure 100 times
  referenceLevel = pressureReferenceCalculater();

  state.mainState = 1;
}

void loop(){
  startTime = millis();
  
  setPressureAndAltitude();
  setTemperature();
  setAcc();

  float altitudeDiff = getAltitude();
  //TODO: initial value of verticalSpeedDiff?
  float verticalSpeedDiff = 0;

  if (state.hizState == 1) {
    long timeDiff = millis() - startTime;
    setPressureAndAltitude();
    setTemperature();
    setAcc();

    altitudeDiff = getAltitude();
    verticalSpeedDiff = verticalSpeedCalculater(timeDiff, altitudeDiff);
  }

  increasePacketId();

  if (state.gpsState == 1) {
    setGPS();
  }

  if (rfReceivedData == true) {
    rfReceivedData = false;
    rfreceivedDataHandler();
  }

  switch(state.mainState) {
    case 0:
    default:
      stateZeroHandler();
      break;

    case 1:
      stateOneHandler();ğ
      break;

    case 2:
      stateTwoHandler(verticalSpeedDiff);
      break;
    case 3:
    case 4:
      // empty for future usages
      break;
    case 5:
      stateFiveHandler();
      break;

    case 6:
      stateSixHandler();
      break;

    case 7:
      stateSevenHandler();
      break;

    case 8:
      stateEightHandler();
      break;
      
  }

}

void stateEightHandler() {
  if (rfEntity.altitude < 30) {
    state.mainState = 8;
    state.gpsState = 1;
    rocketLandingHandler();
  }
}

void stateSevenHandler() {
  launchMainParachute();
  state.mainState = 8;
  state.gpsState = 1;
  state.hizState = 0;
}

void stateSixHandler() {
  if (rfEntity.altitude < 600) {
    state.mainState = 7;
    state.gpsState = 1;
    state.hizState = 0;
  } else {
    state.mainState = 6;
    state.gpsState = 1;
    state.hizState = 0;
  }
}

void stateFiveHandler() {
  launchDragParachute();
  state.mainState = 6;
  state.gpsState = 1;
  state.hizState = 0;
}

void stateTwoHandler(float verticalSpeedDiff) {
  if (verticalSpeedDiff < 0) {
    state.mainState = 5;
    state.gpsState = 0;
    state.hizState = 0;
  } else {
    state.mainState = 2;
    state.gpsState = 0;
    state.hizState = 1;
  }
}

void stateOneHandler() {
  if (rfEntity.altitude > 70) {
    hasBeenLaunched = true;
    state.mainState = 2;
    state.gpsState = 0;
    state.hizState = 1;
  } else {
    state.mainState = 1;
    state.gpsState = 1;
    state.hizState = 0;
  }
}

void stateZeroHandler() {
  state.mainState = 1;
  state.gpsState = 1;
  state.hizState = 0;
}

void initSystem() {
  //TODO
}

void timeInterruptHandler() {
  sendData(rfMapper.map(rfEntity));
}

void rocketLandingHandler() {
  openBuzzer();
}

void openBuzzer() {
  //TODO
}

void launchDragParachute() {
  // TODO
}

void launchMainParachute() {
  // TODO
}

long pressureReferenceCalculater(){
  int pressureCounter;
  long total = 0;
  for (pressureCounter = 0; pressureCounter < 100; pressureCounter ++) {
    total += getPressure();
  }
  return total/100;
}

void sendData(String message) {
  Serial.print(message);
}

void rfreceivedDataHandler() {
  if (receivedSecurityByte == SECURITY_BYTE) {
    state.mainState = receivedStateByte;
  }
}

void increasePacketId() {
  packetId ++;
}

void setGPS() {
  
}

void setTemperature() {
  //bme.readTemperature()
  rfEntity.temperature = mpu.get_temp();
}

float getPressure() {
   return bme.readPressure();
}

float getAltitude() {
   return bme.readAltitude(referenceLevel);
}

void setPressureAndAltitude() {
  rfEntity.pressure = getPressure();
  rfEntity.altitude = getAltitude();
}

void setAcc() {
  mpu.set_accel_range(RANGE_4G);
  mpu.get_accel_g();
  rfEntity.acc_x = mpu.x_g;
  rfEntity.acc_y = mpu.y_g;
  rfEntity.acc_z = mpu.z_g;
}

float getPressureDiff() {
  return rfEntity.pressure - referenceLevel;
}

float verticalSpeedCalculater(long timeDiff, float altitudeDiff) {
  return altitudeDiff/timeDiff;
}

