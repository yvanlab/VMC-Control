
#include "BMPManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#include <DHT.h>
//http://leguidemeteo.com/prevoir-le-temps-a-partir-de-son-barometre/

// BPM_TEMPS_BEAU     :   > 1020               :  Beau ou assez beau. Journées chaudes, nuits fraîches.
// BPM_TEMPS_ORAGEUX  : De 1013 à 1020 hPa	  :  Assez beau ou ondées orageuses. Journées chaudes, nuits un peu fraîches.
// BPM_TEMPS_PLUIE    : De 1006 à 1013 hPa   :	 Pluie ou averses avec un peu de vent. Temps doux.
// BPM_TEMPS_TEMPETE  :  < 1006 hPa	          :  Pluie et vent assez fort. Températures douces.


BMPManager::BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed)
  :Sodaq_BMP085() , BaseManager(pinLed) {
    setStatus( true,"OK");
}

uint8_t getWeatherTrend(float fPression) {
  if  (fPression>102000) return BPM_TEMPS_BEAU;
  if  (fPression>101300 && fPression<102000) return BPM_TEMPS_ORAGEUX;
  if  (fPression>100600 && fPression<101300) return BPM_TEMPS_PLUIE;
  if  (fPression<100600 ) return BPM_TEMPS_TEMPETE;
}

String BMPManager::toString(boolean bJson = STD_TEXT) {
    if (bJson==JSON_TEXT)
    return "\"bmpTemp\" : \""+String (readTemperature()) +
        "\", \"bmpTempTrend\" : \"" + String(m_Temperature.m_trend)+
        "\", \"bmpPressTrend\" : \"" + String(m_Pression.m_trend)+
        "\", \"bmpWeatherForcast\" : \"" + String( getWeatherTrend(m_Pression.m_value))+
        "\", \"bmpPress\" : \"" + String(readPressure())+"\"";
  else
    return "Temperature["+String (readTemperature()) + "]["+String(m_Temperature.m_trend)+"] - Pressure[" + String(readPressure())+ "]["+String(m_Pression.m_trend)+"]";
}


float BMPManager::readTemperature() {
  switchOn();
  float temp;
  #if defined(MCPOC_MOCK)
  temp = random(40);
  setStatus( true,"Mocking");
  #else
  temp = Sodaq_BMP085::readTemperature();
  setStatus( true,"OK");
  #endif
  m_Temperature.set(temp);

  switchOff();
  return temp;
}

int32_t BMPManager::readPressure(){
  switchOn();
  float pressure;
  #if defined(MCPOC_MOCK)
  pressure = random(100000);
  setStatus( true,"Mocking");
  #else
  pressure = Sodaq_BMP085::readPressure();
  setStatus( true,"OK");
  #endif
  m_Pression.set(pressure);

  switchOff();
  return pressure;
}
