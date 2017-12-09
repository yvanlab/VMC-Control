
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
#include "HourManager.h"
//#include "SparkfunManager.h"
#include "WifiManagerV2.h"
#include "SettingManager.h"
#include "ioManager.h"
#include "DHTManager.h"
#include "BMPManager.h"
#include "VTSManager.h"
#include <myTimer.h>

//#include <DateTime.h>
//#include "EmonLib.h"                   // Include Emon Library

#define MQTT_DEBUG
#define MQTT_ERROR
#include <Adafruit_MQTT.h>                                 // Adafruit MQTT library
#include <Adafruit_MQTT_Client.h>                           // Adafruit MQTT library
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
  #define HUMIDITE_VMC_LABEL  "vmcHUM"
  #define TEMPERATURE_VMC_LABEL "vmcTEMP"
  #define VITESSE_VMC_LABEL "vmcVTS"
  #define TEMPERATURE_EXT_LABEL "extTEMP"
  #define TEMPERATUR_EXT_DHT_LABEL "extdhtTEMP"
  #define PRESSION_EXT_LABEL "extPRESS"
  #define HUMIDITE_EXT_LABEL  "extHUM"




//#endif


#define  pinLed           D8 //D4
#define  pin_VMC_DHT      D2
#define  pin_VMC_VITESSE_R1  D1 //D6 //8
#define  pin_VMC_VITESSE_R2  D6 //D1

#define  pin_EXT_BPM_SDA  D3
#define  pin_EXT_BPM_SCL  D4

#define  pin_EXT_EXT_DTH  D7


SettingManager          smManager(pinLed);
WifiManager             wfManager(pinLed,&smManager);
ioManager               sfManager(pinLed);
DHTManager              dhtVMC(pin_VMC_DHT,pinLed);
BMPManager              bmpEXT(pin_EXT_BPM_SDA,pin_EXT_BPM_SCL,pinLed);
DHTManager              dhtEXT(pin_EXT_EXT_DTH,pinLed);
VTSManager              vtsVMC(pin_VMC_VITESSE_R1,pin_VMC_VITESSE_R2,pinLed);

//extern MyTimer         mtTimer;

#ifdef MCPOC_TELNET
RemoteDebug             Debug;
#endif

#ifdef MCPOC_TELNET // Not in PRODUCTION
void processCmdRemoteDebug() {
    String lastCmd = Debug.getLastCommand();
    if (lastCmd == "v0") {
      vtsVMC.setVitesse(VTS_OFF,true);
    } else if (lastCmd == "v1") {
      vtsVMC.setVitesse(VTS_LOW,true);
    } else if (lastCmd == "v2") {
      vtsVMC.setVitesse(VTS_HIGH,true);
    } else if (lastCmd == "restart") {
        ESP.restart();
    } else if (lastCmd == "reset") {
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
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"datetime\":{" + wfManager.getHourManager()->toDTString(JSON_TEXT) + "},";

    tt += "\"LOG\":["+wfManager.log(JSON_TEXT)  + "," + dhtVMC.log(JSON_TEXT)  + "," + dhtEXT.log(JSON_TEXT) + ","+ bmpEXT.log(JSON_TEXT) + "," + sfManager.log(JSON_TEXT) + "," +  wfManager.getHourManager()->log(JSON_TEXT)+"],";
    tt += "\"VMC\":{"+dhtVMC.toString(JSON_TEXT) + "," + vtsVMC.toString(JSON_TEXT)+"},";
    tt += "\"EXT\":{"+bmpEXT.toString(JSON_TEXT) + "," + dhtEXT.toString(JSON_TEXT)+"}}";
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
  message += "<label>Hum. Seuil:</label><input name='humSeuil' length=64 value=\""+String(smManager.m_HUM_SEUIL) +"\"><br>";
  message += "<label>Vitesse:</label><input name='vts' length=64 value=\""+String(vtsVMC.getVitesse()) +"\"><br>";



  message += "<input type='submit'></form>";
  message += "</body></html>";

  wfManager.getServer()->send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}

void setData(){
  String str = wfManager.getServer()->arg("humSeuil");
  if (str.length()>0) {
    smManager.m_HUM_SEUIL = atof(str.c_str());
    smManager.writeData();
  }
  str = wfManager.getServer()->arg("vts");
  if (str.length()>0) {
    vtsVMC.setVitesse(atoi(str.c_str()), true);
  }


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
}


void loop ( void ) {

	wfManager.handleClient();

  /*if (mtTimer.is250MSPeriod())
    DEBUGLOG("debug mode");*/
  //WiFiClient client = server.available();
  if (mtTimer.is1MNPeriod()) {
      //DEBUGLOG(ESP_PLATFORM );
      DEBUGLOG("debug mode");
      DEBUGLOG(dhtVMC.toString(STD_TEXT));
      DEBUGLOG(vtsVMC.toString(STD_TEXT));
      DEBUGLOG(bmpEXT.toString(STD_TEXT));
      DEBUGLOG(dhtEXT.toString(STD_TEXT));
      DEBUGLOG(getJson());


      sfManager.addVariable(HUMIDITE_VMC_LABEL, String(dhtVMC.getHumidity()));
      sfManager.addVariable(TEMPERATURE_VMC_LABEL, String(dhtVMC.getTemperature()));
      sfManager.addVariable(PRESSION_EXT_LABEL, String(bmpEXT.readPressure()));
      sfManager.addVariable(TEMPERATURE_EXT_LABEL, String(bmpEXT.readTemperature()));
      sfManager.addVariable(HUMIDITE_EXT_LABEL, String(dhtEXT.getHumidity()));
      sfManager.addVariable(TEMPERATUR_EXT_DHT_LABEL, String(dhtEXT.getTemperature()));


      vtsVMC.setVitesse(dhtVMC.m_Humidity , dhtEXT.m_Humidity , smManager.m_HUM_SEUIL);
      sfManager.addVariable(VITESSE_VMC_LABEL,String(vtsVMC.getVitesse()));

      sfManager.sendKPIsToIO( smManager.m_privateKey, smManager.m_publicKey);
    }

    if (mtTimer.is5MNPeriod()) {
      if (!WiFi.isConnected()) {
        ESP.restart();
      }
    }

    mtTimer.clearPeriod();

  }
