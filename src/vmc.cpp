
#include <Arduino.h>
#ifdef esp32dev
#include <wifi.h>
#else
#include <ESP8266WebServer.h>

#endif

//https://thingspeak.com/channels/312851

//#include <ArduinoJson.h>
//#include <ESP8266HTTPUpdateServer.h>
//#include "ElectricManager.h"
#include <HourManager.h>
//#include "SparkfunManager.h"
//#include "ioManager.h"
#include <thingSpeakManager.h>
#include <WifiManagerV2.h>
#include "SettingManager.h"

#include <DHTManagerV2.h>
#include "BMPManager.h"
#include "BMPManagerV2.h"
#include "VTSManager.h"
#include <myTimer.h>

//#include <DateTime.h>
//#include "EmonLib.h"                   // Include Emon Library

#define MQTT_DEBUG
#define MQTT_ERROR

//#include <WiFiClient.h>

#define MODULE_NAME VMC_METEO_NAME
#define MODULE_MDNS VMC_METEO_MDNS
#define MODULE_MDNS_AP VMC_METEO_MDNS_AP
#define MODULE_IP   VMC_METEO_IP

/*
extern "C" {
#include "user_interface.h"
}
*/

  #define LOG_LABEL  "log"
  #define HUMIDITE_VMC_LABEL        1 //"vmcHUM"
  #define TEMPERATURE_VMC_LABEL     2 //"vmcTEMP"
  #define VITESSE_VMC_LABEL         3 //"vmcVTS"
  #define TEMPERATURE_EXT_LABEL     4 //"extTEMP"
  #define HUMIDITE_EXT_LABEL        5 //"extHUM"
  #define PRESSION_EXT_LABEL        6 //"extPRESS"

  #define HUMIDITE_TREND_VMC_LABEL  7 //"vmcHUM"

  #define TEMPERATUR_EXT_DHT_LABEL  8 //"extdhtTEMP"






//#endif


#define  pinLed           D8 //D4
#define  pin_VMC_DHT      D2
#define  pin_VMC_VITESSE_R1  D6 //D1 //D6 //8
#define  pin_VMC_VITESSE_R2  D1 //D6 //D1

#define  pin_EXT_BPM_SDA  D3
#define  pin_EXT_BPM_SCL  D4

#define  pin_EXT_EXT_DTH  D7

#define  pin_PRESENCE  A0



SettingManager          smManager(pinLed);
WifiManager             wfManager(pinLed,&smManager);
thingSpeakManager       sfManager(pinLed);
DHTManagerV2            dhtVMC(pinLed,pin_VMC_DHT);
//BMPManager              bmpEXT(pin_EXT_BPM_SDA,pin_EXT_BPM_SCL,pinLed);
BMPManagerV2            bmpEXTV2(pinLed);
DHTManagerV2            dhtEXT(pinLed,pin_EXT_EXT_DTH);
VTSManager              vtsVMC(pin_VMC_VITESSE_R1,pin_VMC_VITESSE_R2,&smManager,pinLed);

//extern MyTimer         mtTimer;

#ifdef MCPOC_TELNET
RemoteDebug             Debug;
#endif

#ifdef MCPOC_TELNET // Not in PRODUCTION
void processCmdRemoteDebug() {
    String lastCmd = Debug.getLastCommand();
    String minute,cmd;
    int8_t index = lastCmd.indexOf(',');
    if (index > 0) {
        cmd  = lastCmd.substring(0, index);
        minute = lastCmd.substring(index+1);
    } else {
        cmd = lastCmd;
        minute = "1";
    }
    uint8_t duration = atoi(minute.c_str());
    if (cmd == "v0") {
      vtsVMC.setVitesse(VTS_OFF,duration);
    } else if (cmd == "v1") {
      vtsVMC.setVitesse(VTS_LOW,duration);
    } else if (cmd == "v2") {
      vtsVMC.setVitesse(VTS_HIGH,duration);
    } else if (cmd == "restart") {
        ESP.restart();
    } else if (cmd == "reset") {
      WiFi.disconnect();
    }
}
#endif


String getJson()
{
  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(MODULE_VERSION) +"\"," ;
    tt += "\"uptime\":\"" +  wfManager.getHourManager()->toUTString() +"\"," ;
    tt += "\"status\":\"" + String(STATUS_PERIPHERIC_WORKING) +"\"," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"datetime\":{" + wfManager.getHourManager()->toDTString(JSON_TEXT) + "},";
    tt += "\"setting\":{" + smManager.toString(JSON_TEXT)  + "},";
    tt += "\"LOG\":["+wfManager.log(JSON_TEXT)  + "," +
                      dhtVMC.log(JSON_TEXT)  + "," +
                      dhtEXT.log(JSON_TEXT) + "," +
                      bmpEXTV2.log(JSON_TEXT) + "," +
                      sfManager.log(JSON_TEXT) + "," +
                      smManager.log(JSON_TEXT) + "," +
                      wfManager.getHourManager()->log(JSON_TEXT)+"],";
    tt += "\"VMC\":{"+dhtVMC.toString(JSON_TEXT) + "," + vtsVMC.toString(JSON_TEXT)+"},";
    tt += "\"EXT\":{" + bmpEXTV2.toString(JSON_TEXT) + "," + dhtEXT.toString(JSON_TEXT)+"}}";
    return tt;
}

/*
{
  "module" : {
    "nom":"blabla",
    "version" : "1.0.2",
    "date":"10/08/2017 18:15"
  },
"log" : [
    {
    "nom" : "class name",
    "code" : "10",
    "description" : "blabla"
    },
    {
    "nom" : "class name",
    "code" : "10",
    "description" : "blabla"
    }
  ],
  "VMC" : {
    "dhtTemp" : "10.25",
    "dhtHum"  : "40.25",
    "Vitesse" : "0"
  },
  "EXT" : {
    "bmpTemp" : "10.25",
    "bmpPress"  : "40.25",
    "dhtHum" : "41.70",
    "dhtTemp" : "27.70"
  }
}

*/

void dataSummaryJson() {
	digitalWrite ( pinLed, LOW );
  wfManager.getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}
void dataSummaryPage() {
	digitalWrite ( pinLed, LOW );
  wfManager.getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}

void displayData() {
	digitalWrite ( pinLed, LOW );

  char temp[400];

  String message =  "<html>\
    <head>\
      <title>Data page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>";

  message += "<form method='get' action='setData'>";
  message += "<label>Hum. Seuil:</label><input name='humSeuil' length=2 value=\""+String(smManager.m_HUM_SEUIL) +"\">";
  message += "<label>Duree:</label><input name='duration' length=2 value=\""+String(smManager.m_duration) +"\"><br>";
  message += "<label>Heure depart:</label><input name='start' length=2 value=\""+String(smManager.m_hourStart) +"\">";
  message += "<label>Heure stop:</label><input name='stop' length=2 value=\""+String(smManager.m_hourStop) +"\"><br>";

  message += "<label>Vitesse:</label><input name='vts' length=64 value=\""+String(vtsVMC.getVitesse()) +"\"><br>";



  message += "<input type='submit'></form>";
  message += "</body></html>";

  wfManager.getServer()->send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}

void setData(){
  bool bWriteData = false;
  String str = wfManager.getServer()->arg("humSeuil");
  if (str.length()>0) {
    smManager.m_HUM_SEUIL = atof(str.c_str());
    bWriteData = true;
  }
  str = wfManager.getServer()->arg("duration");
  if (str.length()>0) {
    smManager.m_duration = atoi(str.c_str());
    bWriteData = true;
  }

  str = wfManager.getServer()->arg("start");
  if (str.length()>0) {
    smManager.m_hourStart = atoi(str.c_str());
    bWriteData = true;
  }

  str = wfManager.getServer()->arg("stop");
  if (str.length()>0) {
    smManager.m_hourStop = atoi(str.c_str());
    bWriteData = true;
  }


  str = wfManager.getServer()->arg("vts");
  if (str.length()>0) {
    vtsVMC.setVitesse(atoi(str.c_str()), true);
  }

  if (bWriteData)
    smManager.writeData();

  wfManager.getServer()->send ( 200, "text/html", "data recorded");
}

void startWiFiserver() {
  if (wfManager.begin(IPAddress(MODULE_IP),MODULE_NAME, MODULE_MDNS, MODULE_MDNS_AP)==WL_CONNECTED) {
    wfManager.getServer()->on ( "/", dataSummaryPage );
    wfManager.getServer()->onNotFound ( dataSummaryPage );
  } /*else {
    DEBUGLOG("Not connected");
  }*/

  wfManager.getServer()->on ( "/setting", displayData );
  wfManager.getServer()->on ( "/setData", setData );
  wfManager.getServer()->on ( "/status", dataSummaryJson );

  Serial.println( "HTTP server started" );
  Serial.println(wfManager.toString(STD_TEXT));
}



void setup ( void ) {
  // Iniialise input
  Serial.begin ( 115200 );
  smManager.readData();
  DEBUGLOG("");DEBUGLOG(smManager.toString(STD_TEXT));
  startWiFiserver();

  #ifdef MCPOC_TELNET
    Debug.begin(MODULE_NAME);
    String helpCmd = "v0\n\rv1\n\rv2\n\rrestart\n\rreset\n\r";
    Debug.setHelpProjectsCmds(helpCmd);
    Debug.setCallBackProjectCmds(&processCmdRemoteDebug);

  #endif

  mtTimer.begin(timerFrequence);
  mtTimer.setCustomMS(10000);

  //pinMode(pin_PRESENCE, INPUT_PULLDOWN_16 );
}

int j = 0 ;
void loop ( void ) {

	wfManager.handleClient();
  if (mtTimer.isCustomPeriod()) {

    dhtVMC.mesure();
    //dhtVMC.mesureTemperature();
    //bmpEXT.mesurePressure();
    //bmpEXT.mesureTemperature();
    dhtEXT.mesure();
    bmpEXTV2.mesure();
    //dhtEXT.mesureTemperature();
  }
  /*if (mtTimer.is250MSPeriod())
    DEBUGLOG("debug mode");*/
  //WiFiClient client = server.available();
  if (mtTimer.is1MNPeriod()) {
      //DEBUGLOG(ESP_PLATFORM );
      DEBUGLOG("debug mode");
      DEBUGLOG(dhtVMC.toString(STD_TEXT));
      DEBUGLOG(vtsVMC.toString(STD_TEXT));
      //DEBUGLOG(bmpEXT.toString(STD_TEXT));
      DEBUGLOG(bmpEXTV2.toString(STD_TEXT));
      DEBUGLOG(dhtEXT.toString(STD_TEXT));
      DEBUGLOG(getJson());


      sfManager.addVariable(HUMIDITE_VMC_LABEL      , String(dhtVMC.getHumiditySensor()->getValue()));
      sfManager.addVariable(TEMPERATURE_VMC_LABEL   , String(dhtVMC.getTemperatureSensor()->getValue()));
      sfManager.addVariable(PRESSION_EXT_LABEL      , String(bmpEXTV2.getPressionSensor()->getValue()));
      sfManager.addVariable(TEMPERATURE_EXT_LABEL   , String(bmpEXTV2.getTemperatureSensor()->getValue()));
      sfManager.addVariable(HUMIDITE_EXT_LABEL      , String(dhtEXT.getHumiditySensor()->getValue()));
      sfManager.addVariable(TEMPERATUR_EXT_DHT_LABEL, String(dhtEXT.getTemperatureSensor()->getValue()));
      sfManager.addVariable(HUMIDITE_TREND_VMC_LABEL, String(dhtVMC.getHumiditySensor()->getTrend()));

      vtsVMC.setVitesse(dhtVMC.m_Humidity , dhtEXT.m_Humidity);
      sfManager.addVariable(VITESSE_VMC_LABEL     ,String(vtsVMC.getVitesse()));
      sfManager.sendIoT( smManager.m_privateKey, smManager.m_publicKey);
    }

    if (mtTimer.is5MNPeriod()) {
      if (wfManager.getHourManager()->isNextDay()) {
        // clear max/min
        dhtVMC.clear();
        dhtEXT.clear();
        bmpEXTV2.clear();
      }

      if (!WiFi.isConnected()) {
        ESP.restart();
      }
    }


    /*if (mtTimer.is1SPeriod()) {

      Serial.print(j++);Serial.print("-");
      Serial.println(analogRead(pin_PRESENCE));
    }*/

    mtTimer.clearPeriod();

  }
