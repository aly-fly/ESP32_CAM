#include <arduino.h>
#include "ESPAsyncWebServer.h"
//#include <ESPmDNS.h>
#include "LittleFS.h"
#include "_CONFIG.h"

String ledState;
String currentStatus;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean takeNewPhoto = false;
boolean useFlash = false;
boolean sendEmail = false;


// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(LED_RED_GPIO_NUM)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.println(ledState);
    return ledState;
  }
  return String();
}

void notFound(AsyncWebServerRequest *request) {
    log_d("404 file not found.");
    request->send(404, "text/plain", "Not found");
}

void webserverInit(){

  // Initialize LittleFS
  if(!LittleFS.begin(true)){
    log_e("An Error has occurred while mounting LittleFS");
    return;
  }

/* LOOKS LIKE THIS BASTARD IS CAUSING THE TG1_WDT RESETS....
  // Set up mDNS responder:
  // - first argument is the domain name, in this example the fully-qualified domain name is "camera1.local"
  // - second argument is the IP address to advertise - we send our IP address on the WiFi network
  if (!MDNS.begin("camera1")) {
      log_e("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  // Add service to MDNS-SD
  MDNS.addService("http","tcp",80);
  log_i("mDNS responder started");
  */

  server.onNotFound(notFound);

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/favicon.ico", String(), false);
  });

  // Route for root / web page
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/indexLED.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, HIGH);    
    request->send(LittleFS, "/indexLED.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, LOW);    
    request->send(LittleFS, "/indexLED.html", String(), false, processor);
  });



  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    log_i("Server Root request.");
    request->send(LittleFS, "/index.html", String(), false);
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, PHOTO_FILE_PATH, "image/jpg", false);
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking photo...");
    currentStatus = "Taking photo in progress..";
  });

  server.on("/capture-flash", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    useFlash = true;
    request->send_P(200, "text/plain", "Taking photo with flash...");
    currentStatus = "Taking photo in progress..";
  });


  server.on("/send-email", HTTP_GET, [](AsyncWebServerRequest * request) {
    sendEmail = true;
    request->send_P(200, "text/plain", "Sending email...");
    currentStatus = "Sending email in progress..";
  });

  server.on("/getStatus", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", currentStatus.c_str());
  });  

  // Start server
  log_i("Starting main web server...");
  server.begin();
}
 
