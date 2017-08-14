
#include "DHTManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DHT.h>

DHTManager::DHTManager( unsigned char pinDHT,unsigned char pinLed)
  : BaseManager(pinLed) {
  //m_pinDHT=pinDHT;
  setup(pinDHT,DHT::AUTO_DETECT);
}

prog_char P_TEMP[]       PROGMEM = "Temperature";

String DHTManager::toString() {
  //String mesure = "Temperature["+String (getTemperature()) + "] - Humidity[" + String(getHumidity())+"] - Status ["+getStatusString()+"]";
  String mesure = "\"dhtTemp\" : \""+String (getTemperature()) + "\", \"dhtHum\" : \"" + String(getHumidity())+"\"";// - Status ["+getStatusString()+"]";
  return mesure;
}

float DHTManager::getTemperature() {
  switchOn();
  float res = DHT::getTemperature();
  setStatus(millis(), getStatus(),getStatusString());
  switchOff();
  return res;
}

float DHTManager::getHumidity(){
  switchOn();
  float res = DHT::getHumidity();
  setStatus(millis(), getStatus(),getStatusString());
  switchOff();
  return res;
}

/*
int8_t DHTManager::read(){
  // read during 2 periods 50 Hz :
  if ((millis() - getLastUpdate()) < 2000UL)
    return 1;

  switchOn();
  DHT::DHT_ERROR_t chk = m_DHT.read();
  if (chk != DTH::ERROR_NONE) {
    setStatus(millis(), chk, "KO",true);
  } else {
    setStatus(millis(), chk, "OK",false);
  }

#ifdef MCPOC_TEST
  Serials.println(m_DHT.toString());

#endif
  switchOff();
  return 0;
}*/
