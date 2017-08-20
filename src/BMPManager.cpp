
#include "BMPManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DHT.h>

BMPManager::BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed)
  :Sodaq_BMP085() , BaseManager(pinLed) {
    setStatus( true,"OK");
}


String BMPManager::toString(boolean bJson = STD_TEXT) {
    if (bJson==JSON_TEXT)
    return "\"bmpTemp\" : \""+String (readTemperature()) + "\", \"bmpPress\" : \"" + String(readPressure())+"\"";
  else
    return "Temperature["+String (readTemperature()) + "] - Pressure[" + String(readPressure())+"]";
}
/*
float BMPManager::readTemperature() {
  switchOn();
  //m_temperature = readTemperature();
  setStatus( true,"OK");
  switchOff();
  return 12.34;
}

int32_t BMPManager::readPressure(){
  switchOn();
  //m_pressure = readPressure();
  setStatus( true,"OK");
  switchOff();
  return 1234567;
}*/
