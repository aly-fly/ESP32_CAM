#include <WiFi.h>
//#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "_CONFIG.h"

bool OTAactive = false;

void OTAstart() {
  log_n("Starting OTA server...");
  /*
  Serial.print("OTA port: ");  
  Serial.println(OTA_PORT);
  ArduinoOTA.setPort(OTA_PORT);     // Port defaults to 3232
*/
  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  #ifdef OTA_PASS
  ArduinoOTA.setPassword(OTA_PASS);
  #endif

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      }
      else { // U_SPIFFS
        type = "filesystem";
        LittleFS.end();
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("OTA Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nUpdate finished.");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
    });

  ArduinoOTA.begin();
  OTAactive = true;
  Serial.println("OTA Ready.");
}

void OTAloop() {
  if (OTAactive) {
    ArduinoOTA.handle();
  }
}
