
#include <Arduino.h>
#ifdef esp32dev
#include <wifi.h>
#else
#include <ESP8266WebServer.h>

#endif

#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
//#include "ElectricManager.h"
#include "HourManager.h"
//#include "SparkfunManager.h"
#include "WifiManager.h"
#include "SettingManager.h"
#include "ioManager.h"
#include "DHTManager.h"
#include "BMPManager.h"
#include "VTSManager.h"

//#include <DateTime.h>
//#include "EmonLib.h"                   // Include Emon Library

#define MQTT_DEBUG
#define MQTT_ERROR
#include <Adafruit_MQTT.h>                                 // Adafruit MQTT library
#include <Adafruit_MQTT_Client.h>                           // Adafruit MQTT library
#include <WiFiClient.h>

#define MODULE_NAME VMC_METEO_NAME
#define MODULE_MDNS VMC_METEO_MDNS
#define MODULE_MDNS_AP VMC_METEO_MDNS_AP
#define MODULE_IP   VMC_METEO_IP


extern "C" {
#include "user_interface.h"
}


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
#define  pin_VMC_VITESSE  D6

#define  pin_EXT_BPM_SDA  D3
#define  pin_EXT_BPM_SCL  D4

#define  pin_EXT_EXT_DTH  D7


ESP8266WebServer server ( 80 );
ESP8266HTTPUpdateServer httpUpdater;
SettingManager smManager(pinLed);
HourManager hrManager(2390,pinLed);
WifiManager wfManager(pinLed);
ioManager  sfManager(pinLed);
DHTManager dhtVMC(pin_VMC_DHT,pinLed);
BMPManager bmpEXT(pin_EXT_BPM_SDA,pin_EXT_BPM_SCL,pinLed);
DHTManager dhtEXT(pin_EXT_EXT_DTH,pinLed);
VTSManager vtsVMC(pin_VMC_VITESSE,0,pinLed);

os_timer_t myTimer;
boolean tickOccured;


const unsigned long timerFrequence = 60000;//ms


/************ Global State (you don't need to change this!) ******************/


// start of timerCallback
void timerCallback(void *pArg) {
     tickOccured = true;
} // End of timerCallback



void restartESP() {
  ESP.restart();
}

void timerrestartESP(void *pArg) {
    restartESP();
}

String getJson()
{
  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(VMC_VERSION) +"\"," ;
    tt += "\"uptime\":\"" + NTP.getUptimeString() +"\"," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"datetime\":{" + hrManager.toDTString(JSON_TEXT) + "},";

    tt += "\"LOG\":["+wfManager.log(JSON_TEXT)  + "," + dhtVMC.log(JSON_TEXT)  + "," + dhtEXT.log(JSON_TEXT) + ","+ bmpEXT.log(JSON_TEXT) + "," + sfManager.log(JSON_TEXT) + "," + hrManager.log(JSON_TEXT)+"],";
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
  server.send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}
void dataSummaryPage() {
	digitalWrite ( pinLed, LOW );
  server.send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );

}

void displayCredentialCollection() {
	digitalWrite ( pinLed, LOW );

  char temp[400];

  String message =  "<html>\
    <head>\
      <title>Credentials page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>";
  message += "<p>";
  message +="<ul>";
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      message += "<li>";
      message +=i + 1;
      message += ": ";
      message += WiFi.SSID(i);
      message += " (";
      message += WiFi.RSSI(i);
      message += ")";
      message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      message += "</li>";
    }
  message += "</ul>";
  message += "<form method='get' action='set'>";
  message += "<label>SSID:</label><input name='ssid' test length=32 value=\""+String(smManager.m_ssid) +"\"><br>";
  message += "<label>Pass:</label><input name='pass' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>PrivateKey:</label><input name='sparkPrivate' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>PublicKey:</label><input name='sparkPublic' length=64 value=\""+String(smManager.m_publicKey) +"\"><br>";
  message += "<label>Hum. Seuil:</label><input name='humSeuil' length=64 value=\""+String(smManager.m_HUM_SEUIL) +"\"><br>";



  message += "<input type='submit'></form>";
  message += "</body></html>";

  server.send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}

void setCredential(){
  String str = server.arg("ssid");
  if (str.length()>0)
    strcpy(smManager.m_ssid, str.c_str());
  str = server.arg("pass");
  if (str.length()>0 && str != HIDDEN_KEY)
    strcpy(smManager.m_password,str.c_str());
  str = server.arg("sparkPrivate");
  if (str.length()>0 && str != HIDDEN_KEY)
      strcpy(smManager.m_privateKey,str.c_str());
  str = server.arg("sparkPublic");
  if (str.length()>0)
      strcpy(smManager.m_publicKey,str.c_str());
  str = server.arg("humSeuil");
  if (str.length()>0)
    smManager.m_HUM_SEUIL = atof(str.c_str());
  smManager.writeData();
  server.send ( 200, "text/html", "data recorded.restart board");
}

void clearMemory(){
  smManager.clearData();
  server.send ( 200, "text/html", "ok");
}


void startWiFiserver() {
  if (wfManager.connectSSID(smManager.m_ssid,smManager.m_password,IPAddress(MODULE_IP), MODULE_MDNS )==WL_CONNECTED) {
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, timerFrequence, true);
    server.on ( "/", dataSummaryPage );
    server.onNotFound ( dataSummaryPage );
    hrManager.begin("pool.ntp.org", 1, true);
  } else {
    os_timer_setfn(&myTimer, timerrestartESP, NULL);
    os_timer_arm(&myTimer, 5*60*1000, true);
    wfManager.connectAP(MODULE_MDNS_AP);
    server.on ( "/", displayCredentialCollection );
    server.onNotFound ( displayCredentialCollection );
  }
  server.on ( "/clear", clearMemory );
  server.on ( "/restart", restartESP );
  server.on ( "/set", setCredential );
  server.on ( "/credential", displayCredentialCollection );
  server.on ( "/status", dataSummaryJson );
  httpUpdater.setup(&server, ((const char *)"/firmware"), MODULE_UPDATE_LOGIN, MODULE_UPDATE_PASS);

  server.begin();
  Serial.println( "HTTP server started" );
  Serial.println(wfManager.toString(STD_TEXT));
}


void setup ( void ) {
  // Iniialise input
  Serial.begin ( 115200 );
  smManager.readData();
  DEBUGLOG("");DEBUGLOG(smManager.toString(STD_TEXT));
  startWiFiserver();
}


void loop ( void ) {

	server.handleClient();
  //WiFiClient client = server.available();
  if (tickOccured) {


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

      if (dhtVMC.getHumidity()>60.0) {
        vtsVMC.setVitesse(VTS_LOW);
        sfManager.addVariable(VITESSE_VMC_LABEL,String(VTS_LOW));
        //sfManager.switchOn();
      } else {
        vtsVMC.setVitesse(VTS_OFF);
        pinMode ( pin_VMC_VITESSE, INPUT );
        sfManager.addVariable(VITESSE_VMC_LABEL,String(VTS_OFF));

      }
      //DEBUGLOG(dhtManager.log());
    //  if (smManager.newLog())   sfManager.addVariable(LOG_LABEL,smManager.log());
//      if (hrManager.newLog())   sfManager.addVariable(LOG_LABEL,hrManager.log());
    //  if (dhtManager.newLog())   sfManager.addVariable(LOG_LABEL,dhtManager.log());

      sfManager.sendKPIsToIO( smManager.m_privateKey, smManager.m_publicKey);

    tickOccured = false;
  }

  }
