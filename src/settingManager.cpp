

#include "SettingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}

String SettingManager:: toString(boolean bJson = false){
  if (bJson ==STD_TEXT)
    return BaseSettingManager::toString(bJson) + " Start,stop["+m_hourStart+","+m_hourStop+"]" + " Hum["+m_HUM_SEUIL+"]" + " duration["+m_duration+"]" ;
  else
    return "\"humSeuil\":\""+String (m_HUM_SEUIL) + "\"," +
            "\"hourStart\":\""+String (m_hourStart) + "\"," +
            "\"hourStop\":\""+String (m_hourStop) + "\"," +
            "\"duration\":\"" + String(m_duration) +"\"";
}

unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
  char tmp[8];
  readEEPROM(tmp,4);
  //Serial.print("m_duration");Serial.print(":");DEBUGLOG(tmp);
  m_duration = atoi(tmp);
  //Serial.print("m_duration");Serial.print(":");DEBUGLOG(m_duration);
  readEEPROM(tmp,4);
  //Serial.print("m_HUM_SEUIL-Read");Serial.print(":");DEBUGLOG(tmp);
  m_HUM_SEUIL = atoi(tmp);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");DEBUGLOG(m_HUM_SEUIL);
  readEEPROM(tmp,4);
  //Serial.print("m_HUM_SEUIL-Read");Serial.print(":");DEBUGLOG(tmp);
  m_hourStart = atoi(tmp);
  readEEPROM(tmp,4);
  //Serial.print("m_HUM_SEUIL-Read");Serial.print(":");DEBUGLOG(tmp);
  m_hourStop = atoi(tmp);

  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();
  char tmp[8];

  itoa(m_duration,tmp,10);
  //Serial.print("WWm_duration");Serial.print(":");DEBUGLOG(tmp);
  writeEEPROM(tmp);

  itoa(m_HUM_SEUIL,tmp,10);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");DEBUGLOG(tmp);
  writeEEPROM(tmp);

  itoa(m_hourStart,tmp,10);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");DEBUGLOG(tmp);
  writeEEPROM(tmp);

  itoa(m_hourStop,tmp,10);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");DEBUGLOG(tmp);
  writeEEPROM(tmp);


  EEPROM.commit();
  switchOff();
}
