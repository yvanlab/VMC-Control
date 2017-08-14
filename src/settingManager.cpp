

#include "SettingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

SettingManager::SettingManager(unsigned char pinLed) : BaseManager(pinLed){
  m_iEEprom = 0;
  EEPROM.begin(512);
}

String SettingManager:: toString(){
  //return "SSID[" + String(m_ssid) + "] PASS[" + m_password + "] privateKey[" + m_privateKey + "] publicKey[" + m_publicKey + "] Hum["+m_HUM_SEUIL+"]";
  return "SSID[" + String(m_ssid) + "] PASS[" + m_password + "] privateKey[" + m_privateKey + "] publicKey[" + m_publicKey + "] Hum["+m_HUM_SEUIL+"]";
}
unsigned char SettingManager::readData(){
  switchOn();
  m_iEEprom=0;
  readEEPROM(m_ssid);
  readEEPROM(m_password);
  readEEPROM(m_privateKey);
  readEEPROM(m_publicKey);
  char tmp[8];
  readEEPROM(tmp);
  //Serial.print("m_HUM_SEUIL-Read");Serial.print(":");Serial.println(tmp);
  m_HUM_SEUIL = atof(tmp);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");Serial.println(m_HUM_SEUIL);
  //writeEEPROM(tmp);

  setStatus(millis(), m_iEEprom, "read");
  switchOff();
}
unsigned char SettingManager::writeData(){
  switchOn();
  m_iEEprom=0;
  writeEEPROM(m_ssid);
  writeEEPROM(m_password);
  writeEEPROM(m_privateKey);
  writeEEPROM(m_publicKey);
  //
  char tmp[8];
  dtostrf(m_HUM_SEUIL, 8, 6, &tmp[0]);
  //Serial.print("m_HUM_SEUIL");Serial.print(":");Serial.println(tmp);
  writeEEPROM(tmp);

  EEPROM.commit();
  setStatus(millis(), m_iEEprom, "written");
  switchOff();
}

unsigned char SettingManager::clearData(){
  switchOn();
  for (int i = 0; i < 150; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  switchOff();
}

unsigned char SettingManager::writeEEPROM(char *str){
  for (int iString = 0; str[iString] != 0; iString++,m_iEEprom++ )  {
    EEPROM.write(m_iEEprom, str[iString]);
    delay(50);
    //Serial.print(m_iEEprom);Serial.print(":");Serial.println(str[iString]);
  }
  delay(50);
  EEPROM.write(m_iEEprom,0);
  //Serial.print(m_iEEprom);Serial.println(":--");
  m_iEEprom++;
}

unsigned char SettingManager::readEEPROM(char *str){
  int iString = 0;

  while (iString < 150) {
    str[iString] = EEPROM.read(m_iEEprom);
    //Serial.print(m_iEEprom);Serial.print(":");
    m_iEEprom++;
    if (str[iString] == 0) {
      //Serial.println("--");
      break;
    }/*else {
      Serial.println(str[iString]);
    }*/
    iString++;
    delay(50);
  }

}
