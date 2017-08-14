/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <Arduino.h>
#ifdef esp32dev
#include <wifi.h>
#else
#include <ESP8266WebServer.h>

#endif

#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
//#include "ElectricManager.h"
//#include "HourManager.h"
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

  #define HIDDEN_KEY "*****"


//#endif


#define  pinLed           D8 //D4
#define  pin_VMC_DHT      D2
#define  pin_VMC_VITESSE  D6

#define  pin_EXT_BPM_SDA  D3
#define  pin_EXT_BPM_SCL  D4

#define  pin_EXT_EXT_DTH  D7


const char* host = "esp8266-webupdate";
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin";

ESP8266WebServer server ( 80 );
ESP8266HTTPUpdateServer httpUpdater;
SettingManager smManager(pinLed);
//HourManager hrManager(2390,pinLed);
WifiManager wfManager(pinLed);
ioManager  sfManager(pinLed);
DHTManager dhtVMC(pin_VMC_DHT,pinLed);
BMPManager bmpEXT(D3,D4,pinLed);
DHTManager dhtEXT(pin_EXT_EXT_DTH,pinLed);
VTSManager vtsVMC(pin_VMC_VITESSE,0,pinLed);
//WiFiClient client;

os_timer_t myTimer;
boolean tickOccured;
//EnergyMonitor emon1;

const unsigned long timerFrequence = 2000;//60000;//ms
//const unsigned long maxNbreMesure = 60000/timerFrequence; // send KPI every minute

//unsigned long m_timeReference=0;




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
  String tt("{\"module\":{\"nom\":\"VMC-meteo\",");
   tt += "\"version\":\"" + String("VERSION") + "\",";
    tt += "\"date\":\"29/10/2017\"},\"VMC\":{";
    tt += dhtVMC.toString() + "," + vtsVMC.toString();
    tt += "},\"EXT\":{";
    tt += /*bmpEXT.toString()*/ String("fakeee") + "," + dhtEXT.toString();
    tt += "}}";
    return tt;
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


void setup ( void ) {
  // Iniialise input
  pinMode ( pinLed, OUTPUT );
  //pinMode ( pin_VMC_VITESSE, INPUT );

	Serial.begin ( 115200 );
  Serial.println("6666 beforebgin");
  //bmpEXT.begin(BMP085_ULTRAHIGHRES);

  smManager.readData();
  Serial.println(smManager.toString());
  if (wfManager.connectSSID(smManager.m_ssid,smManager.m_password )==WL_CONNECTED) {
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, timerFrequence, true);
    server.on ( "/", dataSummaryPage );
    server.onNotFound ( dataSummaryPage );
  } else {
    os_timer_setfn(&myTimer, timerrestartESP, NULL);
    os_timer_arm(&myTimer, 5*60*1000, true);
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, timerFrequence, true);
    wfManager.connectAP();
    server.on ( "/", displayCredentialCollection );
    server.onNotFound ( displayCredentialCollection );
  }
  server.on ( "/clear", clearMemory );
  server.on ( "/restart", restartESP );
  server.on ( "/set", setCredential );
  server.on ( "/credential", displayCredentialCollection );
  httpUpdater.setup(&server, update_path, update_username, update_password);
  server.begin();
  Serial.println ( "HTTP server started" );
  Serial.println(wfManager.toString());





  //digitalWrite ( pin_VMC_VITESSE,0);


}


void loop ( void ) {
  /*delayMicroseconds(500000);
  pinMode ( pin_VMC_VITESSE, INPUT );
  digitalWrite(pin_VMC_VITESSE, 1); //On écrit l'état du relais
  sfManager.switchOn();
  delayMicroseconds(500000);
  pinMode ( pin_VMC_VITESSE, OUTPUT );
  digitalWrite(pin_VMC_VITESSE, 0);
  sfManager.switchOff();*/
	server.handleClient();
  //WiFiClient client = server.available();
  if (tickOccured) {

    Serial.print ("debug mode : ");
    Serial.println (millis());

/*
  #ifdef MCPOC_TEST
      //Serial.println (ESP_PLATFORM );
      Serial.println ("debug mode");
      Serial.println (dhtVMC.toString());
      Serial.println (vtsVMC.toString());
      Serial.println (bmpEXT.toString());
      Serial.println (dhtEXT.toString());
      Serial.println (getJson());
  #endif

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
      //Serial.println(dhtManager.log());
    //  if (smManager.newLog())   sfManager.addVariable(LOG_LABEL,smManager.log());
//      if (hrManager.newLog())   sfManager.addVariable(LOG_LABEL,hrManager.log());
    //  if (dhtManager.newLog())   sfManager.addVariable(LOG_LABEL,dhtManager.log());

      sfManager.sendKPIsToIO( smManager.m_privateKey, smManager.m_publicKey);

*/
    tickOccured = false;
  }

  }
