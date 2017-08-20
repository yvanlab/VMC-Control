

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

    String toString(boolean bJson);
    String getClassName(){return "BMPManager";}
    /*float readTemperature(void);
    int32_t readPressure(void);*/
};

#endif
