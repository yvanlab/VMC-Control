
#include "HourManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



HourManager::HourManager(unsigned int localPort, unsigned char pinLed) : BaseManager(pinLed) {
  m_localPort = localPort;

	//WiFi.onStationModeDisconnected(std::bind(&HourManager::onWifiDeConnectonEvent, this,_1));
}

void HourManager::onWifiConnectonEvent(WiFiEventStationModeGotIP ipInfo) {
  DEBUGLOG("Got IP:");
  NTP.begin("pool.ntp.org", 1, true);
  NTP.setInterval(63);

}
void HourManager::onWifiDeConnectonEvent(WiFiEventStationModeGotIP ipInfo){
  DEBUGLOG("+++++++++++++++++++++Disconnected from SSID");
  //DEBUGLOGf("Reason: %d\n", WiFi.reason);

}


boolean  HourManager::isNextDay() {
   if (day(NTP.getTime())!=m_today) {
     m_today = day(NTP.getTime());
     return true;
   }
   return false;
}


String HourManager::toDTString(boolean bJson) {
  if (bJson==STD_TEXT) {
    return NTP.getTimeDateString();
  } else {
    return String ( "\"date\":\""+NTP.getDateStr()+"\",\"time\":\""+NTP.getTimeStr()+"\"");
  }
  return "";
}


String HourManager::toUTString(){
  return NTP.getUptimeString();
  //return "";
}

bool HourManager::begin(String ntpServerName, int timeOffset, bool daylight) {
  //std::bind(&MyClass::Callback, this, _1)
  NTP.onNTPSyncEvent(std::bind(&HourManager::NTPsyncEvent, this,_1));
  /*WiFi.onStationModeGotIP(std::bind(&HourManager::onWifiConnectonEvent, this,_1));
  WiFi.onStationModeDisconnected(std::bind(&HourManager::onWifiDeConnectonEvent, this,_1));*/
  //DEBUGLOGf("Got IP: %s\r\n", WiFI.ip.toString().c_str());
  NTP.begin(ntpServerName, timeOffset, daylight);
  NTP.setInterval(60*60); //every hour

  return true;
}

void HourManager::NTPsyncEvent(NTPSyncEvent_t ntpEvent) {
  if (ntpEvent) {
    DEBUGLOG("**Time Sync error: ");
    if (ntpEvent == noResponse) {
      setStatus(millis(), ntpEvent, "NTP server not reachable");
      DEBUGLOG("NTP server not reachable");
    }
    else if (ntpEvent == invalidAddress){
      setStatus(millis(), ntpEvent, "Invalid NTP server address");
      DEBUGLOG("**Invalid NTP server address");
    }
  }
  else {
    DEBUGLOG("**Got NTP time: ");
    setStatus(0, "Synchronised");
    DEBUGLOG(NTP.getTimeDateString(NTP.getLastNTPSync()));
  }
}
