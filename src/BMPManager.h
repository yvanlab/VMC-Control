

#ifndef BMPManager_h
#define BMPManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <Wire.h>
#include <Sodaq_BMP085.h>
#include "BaseManager.h"


class BMPManager : public Sodaq_BMP085, public BaseManager
{

  public:
    BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed);

    String toString();

    /*float getTemperature();
    int32_t getPressure();*/
    String getClassName(){return "BMPManager";}

  private:
    //int32_t   m_pressure;
    //float     m_temperature;
    //uint8_t m_pinDHT;
    //int8_t read();

};

#endif
