#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "MPU9250.h"

#include "RFEntity.h"
#include "RFMapper.h"
#include "State.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

#define buzzerPin 7
#define gpsRxPin 10
#define gpsTxPin 9
#define pyroDrag 6
#define pyroMain 8

#define SECURITY_CHAR 'X'
#define END_CHAR 'Y'
#define DRAG_PARACHUTE_CHAR 'D'
#define MAIN_PARACHUTE_CHAR 'M'
#define RE_REFERENCE_CHAR 'R'

#define XBEE_SERIAL Serial
#define DEBUG_SERIAL Serial


RFEntity rfEntity;
RFMapper rfMapper;
State state;

MPU9250 mpu = MPU9250();

Adafruit_BMP280 bme;

TinyGPSPlus gps;
SoftwareSerial ss(gpsRxPin, gpsTxPin);

double referenceLevel;  // before launch reference level
float seaLevelPressure = 1013.25;
long startTime;
long lastTimeRfSent = 0;
long lastTimeExAltitudeSet = 0;
long buzzerTime = 0;
bool rfReceivedData = false;
char receivedSecurityByte = 0;
char receivedStateByte = 0;
bool hasBeenLaunched = false;
float exAltitude = 0;
char receivedDataBuffer[2];
int receivedDataIndex = 0;
int verticalVelocityHowManyTimes = 0;

void setup(){
  ss.begin(9600);
  XBEE_SERIAL.begin(9600);
  DEBUG_SERIAL.begin(9600);

  mpu.begin();

  if (!bme.begin()) {  
    DEBUG_SERIAL.println("Could not find a valid BMP280 sensor, check wiring!");
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

  //TODO: initial value of verticalSpeedDiff?
  float verticalSpeedDiff = 0;

  if (state.hizState == 1) {
    long timeDiff = millis() - startTime;
    setPressureAndAltitude();
    setTemperature();
    setAcc();

    verticalSpeedDiff = verticalSpeedCalculater(timeDiff, rfEntity.altitude - exAltitude);
  }
  state.gpsState = 1;     // Test için burada SİLİNECEK
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
      stateOneHandler();
      break;

    case 2:
      stateTwoHandler(verticalSpeedDiff);
      break;
    case 3:
      break;
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

  sendDataIfTimeFlow();
  setExAltitudeIfTimeFlow();
  checkIfDataReceived();
}

void checkIfDataReceived() {
  if (XBEE_SERIAL.available() > 0) {
    char receivedData = XBEE_SERIAL.read();
    if (receivedData == SECURITY_CHAR) {
      receivedDataIndex = 0;
    }
    if (receivedData == END_CHAR) {
      rfreceivedDataHandler();
      receivedDataIndex = 0;
      return;
    }
    if (receivedDataIndex == 2) {
      receivedDataIndex = 0;
    }
    receivedDataBuffer[receivedDataIndex] = receivedData;
    receivedDataIndex ++;
  }
}

void setExAltitudeIfTimeFlow() {
  if (millis() - lastTimeExAltitudeSet > 500) {
    exAltitude = getAltitudeDiff();
    lastTimeExAltitudeSet = millis();  
  }
}

void sendDataIfTimeFlow() {
  if (millis()- lastTimeRfSent > 1000) {
    lastTimeRfSent = millis();
    increasePacketId();
    rfEntity.state = state.mainState;
    timeInterruptHandler();
  }
}

void timeInterruptHandler() {
  sendData(rfMapper.map(rfEntity));
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
    if(verticalVelocityHowManyTimes > 2){
      state.mainState = 5;
      state.gpsState = 0;
      state.hizState = 0;
    } else{
      verticalVelocityHowManyTimes ++;      
    }
  } else {
    state.mainState = 2;
    state.gpsState = 0;
    state.hizState = 1;
  }
  digitalWrite(pyroMain, LOW);    
  digitalWrite(pyroDrag, LOW); 
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

  digitalWrite(pyroMain, LOW);    
  digitalWrite(pyroDrag, LOW); 
}

void stateZeroHandler() {
  state.mainState = 1;
  state.gpsState = 1;
  state.hizState = 0;
}

void initSystem() {
  //TODO
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  
  pinMode(pyroMain, OUTPUT);
  digitalWrite(pyroMain, LOW);    
  pinMode(pyroDrag, OUTPUT);
  digitalWrite(pyroDrag, LOW);          
}

void rocketLandingHandler() {
  openBuzzer();
}

void openBuzzer() {
  if (millis()- buzzerTime > 100) {
    buzzerTime = millis();
    digitalWrite(buzzerPin, HIGH);
  } else{
    digitalWrite(buzzerPin, LOW);
  }
}

void launchDragParachute() {
  digitalWrite(pyroDrag, HIGH);
}

void launchMainParachute() {
  digitalWrite(pyroMain, HIGH);
}

long pressureReferenceCalculater(){
  int altitudeCounter;
  long total = 0;
  for (altitudeCounter = 0; altitudeCounter < 100; altitudeCounter ++) {
    total += getAltitude();
  }
  return total/100;
}

void sendData(String message) {
  XBEE_SERIAL.print(message);
}

void rfreceivedDataHandler() {
  if (receivedDataBuffer[0] == SECURITY_CHAR) {
    if (receivedDataBuffer[1] == DRAG_PARACHUTE_CHAR) {
      launchDragParachute();
    } else if (receivedDataBuffer[1] == MAIN_PARACHUTE_CHAR) {
      launchMainParachute();
    }else if (receivedDataBuffer[1] == RE_REFERENCE_CHAR) {
      referenceLevel = pressureReferenceCalculater();
      state.mainState = 0;
    }
  }
}

void increasePacketId() {
  rfEntity.packetId ++;
}

// TODO, will be check here
void setGPS() {
    while(ss.available() > 0) {
      if (gps.encode(ss.read())) {
          if (gps.location.isValid())
          {
            rfEntity.lat = gps.location.lat();
            rfEntity.lon = gps.location.lng();
          }
      }
    }  
}

void setTemperature() {
  //bme.readTemperature()
  rfEntity.temperature = mpu.get_temp();
}

float getPressure() {
   return bme.readPressure();
}

float getAltitude() {
   return bme.readAltitude(seaLevelPressure);
}

float getAltitudeDiff() {
   return bme.readAltitude(seaLevelPressure) - referenceLevel;
}

void setPressureAndAltitude() {
  rfEntity.pressure = getPressure();
  rfEntity.altitude = getAltitudeDiff();
}

void setAcc() {
  mpu.set_accel_range(RANGE_8G);  // 8G will be experienced by the sensor
  mpu.get_accel_g();
  rfEntity.acc_x = mpu.x_g;
  rfEntity.acc_y = mpu.y_g;
  rfEntity.acc_z = mpu.z_g;
}

float verticalSpeedCalculater(long timeDiff, float altitudeDiff) {
  return altitudeDiff/timeDiff;
}

