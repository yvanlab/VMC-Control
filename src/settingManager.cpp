

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
    return BaseSettingManager::toString(bJson) + " Start,stop["+m_hourStart+","+m_hourStop+"]" + " Hum["+m_HUM_SEUIL+"]" + " duration["+m_duration+"] Force,vitesse[" + m_force + "," + m_vitesse + "]" ;
  else
    return  /*BaseSettingManager::toString(bJson) + "," +*/
            "\"force\":\""+String (m_force) + "\"," +
            "\"vitesse\":\""+String (m_vitesse) + "\"," +
            "\"humSeuil\":\""+String (m_HUM_SEUIL) + "\"," +
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
  m_force =  readEEPROM();
  m_vitesse = readEEPROM();
  if (m_vitesse>2) m_vitesse = 0,


  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();

  writeEEPROM(m_duration);
  writeEEPROM(m_HUM_SEUIL);
  writeEEPROM(m_hourStart);
  writeEEPROM(m_hourStop);
  writeEEPROM(m_force);
  writeEEPROM(m_vitesse);
  EEPROM.commit();
  switchOff();
}
