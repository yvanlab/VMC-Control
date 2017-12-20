

#ifndef VTSManager_h
#define VTSManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
/*#include <Wire.h>
#include <Sodaq_BMP085.h>*/
#include "BaseManager.h"
#include "SettingManager.h"

#define VTS_OFF 0
#define VTS_LOW 1
#define VTS_HIGH 2
#define MAX_TIME_VTS_FORCED 60*2*1000// 60*60*1000 //1 heure

class VTSManager : public BaseManager
{

  public:
    VTSManager(uint8_t Relais1, uint8_t Relais2, SettingManager *sm, uint8_t pinLed);

    void setVitesse(uint8_t vitesse, uint8_t duration = 0 );
    void setVitesse(capteurValue vmc, capteurValue ext, uint8_t seuil, uint8_t maxDuration);
    void setVitesse(capteurValue vmc, capteurValue ext);
    uint8_t getVitesse() {return m_vitesse;};

    String toString(boolean bJson);

    String getClassName(){return "VTSManager";}

  private:
    uint8_t m_vitesse;
    uint8_t m_relais1;
    uint8_t m_relais2;
    uint32_t m_lastForcedVitesse;
    uint32_t m_forcedDurationVitesse;
    SettingManager *m_sm;

};

#endif
