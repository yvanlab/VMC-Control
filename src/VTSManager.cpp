
#include "VTSManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

VTSManager::VTSManager(uint8_t Relais1, uint8_t Relais2, uint8_t pinLed)
  :BaseManager(pinLed) {
    setStatus( true,"OK");
    m_relais1 = Relais1;
    m_relais2 = Relais2;
    pinMode ( m_relais1, OUTPUT );
    pinMode ( m_relais2, OUTPUT );
    setVitesse(VTS_OFF);
    //m_vitesse = VTS_OFF;
}


void  VTSManager::setVitesse(capteurValue vmc, capteurValue ext, float seuil) {
    // eteint la nuit : 17:00 a 08:00
    // vitesse HIGH si humidite VMC > EXT et augmente vite ==> pendant une heure
    // vitesse LOW sinon

    if (m_lastForcedVitesse!=0 && (millis() - m_lastForcedVitesse ) < MAX_TIME_VTS_FORCED) {
      DEBUGLOGF("Not calculated : %d\n",m_vitesse);
      return;
    }
    bool bForce=false;
    uint8_t vitesse = VTS_OFF;
    if (hour()>8 && hour()<18)
      vitesse = VTS_LOW;
    else
      vitesse = VTS_OFF;

    if (vmc.m_value>ext.m_value) {
      if (vmc.m_trend > seuil) {
        vitesse = VTS_HIGH;
        bForce = true;
      }
    }
    setVitesse(vitesse, bForce);
    DEBUGLOGF("Calculated : %d\n",vitesse);
  }


void VTSManager::setVitesse(uint8_t vitesse, bool bForce /*=false*/) {
  pinMode ( m_relais1, OUTPUT );
  pinMode ( m_relais2, OUTPUT );

  if (vitesse == VTS_OFF) {
    digitalWrite(m_relais1, HIGH);
    digitalWrite(m_relais2, HIGH);
    //DEBUGLOGF("reliais %d:%d     relais %d:%d\n",m_relais1,LOW,m_relais2, LOW);
  } else if (vitesse == VTS_LOW) {
    digitalWrite(m_relais1, HIGH);
    digitalWrite(m_relais2, LOW);
    //DEBUGLOGF("reliais %d:%d     relais %d:%d\n",m_relais1,HIGH,m_relais2, LOW);
  } else if (vitesse == VTS_HIGH) {
    digitalWrite(m_relais1, LOW);
    digitalWrite(m_relais2, LOW);
    //DEBUGLOGF("reliais %d:%d     relais %d:%d\n",m_relais1,HIGH,m_relais2, HIGH);

  }
  m_vitesse = vitesse;
  if (bForce) m_lastForcedVitesse = millis();
}

String VTSManager::toString(boolean bJson = false) {
  if (bJson)
    return "\"Vitesse\" : \""+String (m_vitesse) + "\"";
  else
    return "Vitesse ["+String (m_vitesse) + "]";
}
