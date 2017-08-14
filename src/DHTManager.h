

#ifndef DHTManager_h
#define DHTManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <DHT.h>
#include "BaseManager.h"


class DHTManager : public DHT, public BaseManager
{

  public:


    DHTManager(unsigned char pinDHT,unsigned char pinLed);

    String toString();

    float getTemperature();
    float getHumidity();
    String getClassName(){return "DHTManager";}

  private:
    //DHT  m_DHT;
    //uint8_t m_pinDHT;
    //int8_t read();

};

#endif
