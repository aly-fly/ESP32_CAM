#include <arduino.h>
#include <WiFi.h>
#include "chip_info.h"
#include "_CONFIG.h"
#include "_global_vars.h"
#include "SaveSettings.h"
#include "camera.h"
#include "LittleFS.h"
#include "Clock.h"
#ifdef WEBSERVER_TEST
  #include "webserver_test.h"
#endif
#include "webserver.h"
#include "email_send.h"


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(100);

  pinMode(LED_FLASH_GPIO_NUM, OUTPUT);
  pinMode(LED_RED_GPIO_NUM, OUTPUT);  

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE // debug level 5
  GetPrintChipInfo();
  Serial.println();
#endif

  NVSReadSettings();

  // Initialize LittleFS
  Serial.println("Mounting LittleFS..");
  if(!LittleFS.begin()){
    log_e("An Error has occurred while mounting LittleFS.");
    while (1) { delay(10); yield(); } // Stay here twiddling thumbs waiting
  }

  CameraInit();

  Serial.println();
  Serial.println("WiFi start...");  

  WiFi.begin(MyConfig.wifiSsid, MyConfig.wifiPass);
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

  currentStatus = "Boot finished";
}

void loop() {

  if (takeNewPhoto) {
    if (MyConfig.useFlash == "1") {
      digitalWrite(LED_FLASH_GPIO_NUM, HIGH);
    }
    delay(250); 
    capturePhotoSaveToFilesystem();
    digitalWrite(LED_FLASH_GPIO_NUM, LOW); 
    takeNewPhoto = false;
    currentStatus = "Photo saved.";
  }

  delay(500);

  if (sendEmail) {
    currentStatus = emailSend(emailLiveImage);
    sendEmail = false;
  }

  delay(500);
}

// C:\Users\aljaz\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\dio_qspi\include\sdkconfig.h
// #define CONFIG_ESP_INT_WDT_TIMEOUT_MS 500  // original 300