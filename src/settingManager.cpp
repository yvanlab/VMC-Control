

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

  m_duration = readEEPROM();
  //Serial.print("m_duration");Serial.print(":");DEBUGLOG(m_duration);
  m_HUM_SEUIL = readEEPROM();
  if (m_version != EEPROM_VERSION) {
    m_hourStart = 7;
    m_hourStop  = 18;
  } else {
    m_hourStart = readEEPROM();
    m_hourStop = readEEPROM();
  }

  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();

  writeEEPROM(m_duration);
  writeEEPROM(m_HUM_SEUIL);
  writeEEPROM(m_hourStart);
  writeEEPROM(m_hourStop);


  EEPROM.commit();
  switchOff();
}
