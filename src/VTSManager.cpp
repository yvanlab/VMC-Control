
#include "VTSManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

VTSManager::VTSManager(uint8_t Relais1, uint8_t Relais2, SettingManager *sm, uint8_t pinLed)
  :BaseManager(pinLed) {
    setStatus( true,"OK");
    m_relais1 = Relais1;
    m_relais2 = Relais2;
    pinMode ( m_relais1, OUTPUT );
    pinMode ( m_relais2, OUTPUT );
    setVitesse(VTS_OFF);
    m_sm = sm;
    //m_vitesse = VTS_OFF;
}

void  VTSManager::setVitesse(capteurValue vmc, capteurValue ext) {
      if (m_lastForcedVitesse!=0 && (millis() -m_lastForcedVitesse ) < m_forcedDurationVitesse) {
        DEBUGLOGF("Not calculated : %d\n",m_vitesse);
        return;
      }
      uint8_t calculatedDuration = 0;
      uint8_t vitesse = VTS_OFF;
      if (hour()>=m_sm->m_hourStart && hour()<m_sm->m_hourStop)
        vitesse = VTS_LOW;
        vitesse = VTS_OFF;

      if (vmc.m_trend > m_sm->m_HUM_SEUIL) {
        vitesse = VTS_HIGH;
        calculatedDuration = m_sm->m_duration;
      }

      if (vitesse == VTS_OFF && ((m_vitesse == VTS_HIGH) ||(m_vitesse == VTS_LOW)) ) {
          calculatedDuration = 10;
      } else if (vitesse == VTS_LOW && m_vitesse == VTS_HIGH ) {
          calculatedDuration = 10;
      }

      setVitesse(vitesse, calculatedDuration);
      DEBUGLOGF("Calculated : %d,%d\n",vitesse,calculatedDuration);
}


void  VTSManager::setVitesse(capteurValue vmc, capteurValue ext, uint8_t seuil, uint8_t maxDuration) {
    // eteint la nuit : 17:00 a 08:00
    // vitesse HIGH si humidite VMC > EXT et augmente vite ==> pendant une heure
    // vitesse LOW sinon

    if (m_lastForcedVitesse!=0 && (millis() -m_lastForcedVitesse ) < m_forcedDurationVitesse) {
      DEBUGLOGF("Not calculated : %d\n",m_vitesse);
      return;
    }
    uint8_t calculatedDuration = 0;
    uint8_t vitesse = VTS_OFF;
    if (hour()>6 && hour()<19)
      vitesse = VTS_LOW;
    else
      vitesse = VTS_OFF;

    if (vmc.m_trend > seuil) {
      vitesse = VTS_HIGH;
      calculatedDuration = maxDuration;
    }

    if (vitesse == VTS_OFF && ((m_vitesse == VTS_HIGH) ||(m_vitesse == VTS_LOW)) ) {
        calculatedDuration = 10;
    } else if (vitesse == VTS_LOW && m_vitesse == VTS_HIGH ) {
        calculatedDuration = 10;
    }

    setVitesse(vitesse, calculatedDuration);
    DEBUGLOGF("Calculated : %d,%d\n",vitesse,calculatedDuration);
  }


void VTSManager::setVitesse(uint8_t vitesse, uint8_t duration /*=0*/) {
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
  if (duration != 0) {
    m_lastForcedVitesse = millis() ;
    m_forcedDurationVitesse = duration*60*1000;
  }
}

String VTSManager::toString(boolean bJson = false) {
  if (bJson)
    return "\"Vitesse\" : \""+String (m_vitesse) + "\"";
  else
    return "Vitesse ["+String (m_vitesse) + "]";
}
