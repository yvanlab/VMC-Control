

#ifndef VTSManager_h
#define VTSManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <Wire.h>
#include <Sodaq_BMP085.h>
#include "BaseManager.h"

#define VTS_OFF 0
#define VTS_LOW 1
#define VTS_HIGH 2


class VTSManager : public BaseManager
{

  public:
    VTSManager(uint8_t Relais1, uint8_t Relais2, uint8_t pinLed);

    void setVitesse(uint8_t vitesse);
    uint8_t getVitesse() {return m_vitesse;};

    String toString(boolean bJson);

    String getClassName(){return "VTSManager";}

  private:
    uint8_t m_vitesse;
    uint8_t m_relais1;
    uint8_t m_relais2;

};

#endif
