

#ifndef SettingManager_h
#define SettingManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <EEPROM.h>
#include <BaseManager.h>
#include <BaseSettingManager.h>

class SettingManager : public BaseSettingManager
{
  public:
    uint8_t m_HUM_SEUIL; //float
    uint8_t m_duration;
    uint8_t m_hourStart;
    uint8_t m_hourStop;
    SettingManager(unsigned char pinLed);

    unsigned char readData();
    unsigned char writeData();
    String getClassName(){return "SettingManager";}
    String toString(boolean bJson);
  private:

};

#endif
