
#include "BMPManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DHT.h>

BMPManager::BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed)
  :Sodaq_BMP085() , BaseManager(pinLed) {
    setStatus(millis(), true,"OK");
}


String BMPManager::toString() {
  //Serial.println("toString");
  //String mesure = "Temperature["+String (readTemperature()) + "] - Pressure[" + String(readPressure())+"]";
  String mesure = "\"bmpTemp\" : \""+String (readTemperature()) + "\", \"bmpPress\" : \"" + String(readPressure())+"\"";
  return mesure;
}

/*float BMPManager::getTemperature() {
  switchOn();
  m_temperature = readTemperature();
  setStatus(millis(), true,"OK");
  switchOff();
  return m_temperature;
}

int32_t BMPManager::getPressure(){
  switchOn();
  m_pressure = readPressure();
  setStatus(millis(), true,"OK");
  switchOff();
  return m_pressure;
}*/
