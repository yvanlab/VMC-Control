
#include "VTSManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

VTSManager::VTSManager(uint8_t Relais1, uint8_t Relais2, uint8_t pinLed)
  :BaseManager(pinLed) {
    setStatus(millis(), true,"OK");
    m_relais1 = Relais1;
    m_relais2 = Relais2;
    setVitesse(VTS_OFF);
    m_vitesse = VTS_OFF;
}

void VTSManager::setVitesse(uint8_t vitesse) {
  if (vitesse = VTS_LOW) {
    pinMode ( m_relais1, INPUT );
    //digitalWrite(m_relais1, LOW);

  } else {
    pinMode ( m_relais1, OUTPUT );
    digitalWrite(m_relais1, 0);
  }

}

String VTSManager::toString() {
  return "\"Vitesse\" : \""+String (m_vitesse) + "\"";
}
