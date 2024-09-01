#include <arduino.h>
#include <WiFi.h>
#include "chip_info.h"
#include "_USER_DEFINES.h"
#include "_CONFIG.h"
#include "camera.h"
#include "SPIFFS.h"
#include "Clock.h"
#ifdef WEBSERVER_TEST
  #include "webserver_test.h"
#endif
#include "webserver.h"
#include "email_send.h"


void setup() {
  Serial.begin(115200);
  pinMode(LED_FLASH_GPIO_NUM, OUTPUT);
  pinMode(LED_RED_GPIO_NUM, OUTPUT);  

  delay(1000);
  GetPrintChipInfo();
  Serial.setDebugOutput(true);
  Serial.println();

  // Initialize SPIFFS
  Serial.println("Mounting SPIFFS..");
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS.");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  CameraInit();

  Serial.println();
  Serial.println("WiFi start...");  

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  setClock();
  
#ifdef WEBSERVER_TEST
  Serial.println("Starting config & test servers...");
  startCameraServer();
#endif

  webserverInit();

  Serial.print("System Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect.");
}

void loop() {

  if (takeNewPhoto) {
    if (useFlash) {
      digitalWrite(LED_FLASH_GPIO_NUM, HIGH);
    }
    delay(250); 
    capturePhotoSaveSpiffs();
    digitalWrite(LED_FLASH_GPIO_NUM, LOW); 
    takeNewPhoto = false;
    useFlash = false;
    delay(500);
    emailSend();
  }
  delay(50);

}
