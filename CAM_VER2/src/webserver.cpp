#include <arduino.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "_CONFIG.h"


// Stores LED state
String ledState;

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
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void webserverInit(){

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Set up mDNS responder:
  // - first argument is the domain name, in this example the fully-qualified domain name is "camera1.local"
  // - second argument is the IP address to advertise - we send our IP address on the WiFi network
  if (!MDNS.begin("camera1")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  // Add service to MDNS-SD
  MDNS.addService("http","tcp",80);
  Serial.println("mDNS responder started");

  server.onNotFound(notFound);

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", String(), false);
  });

  // Route for root / web page
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/indexLED.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, HIGH);    
    request->send(SPIFFS, "/indexLED.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, LOW);    
    request->send(SPIFFS, "/indexLED.html", String(), false, processor);
  });



  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking photo...");
  });

  server.on("/capture-flash", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    useFlash = true;
    request->send_P(200, "text/plain", "Taking photo with flash...");
  });


  server.on("/send-email", HTTP_GET, [](AsyncWebServerRequest * request) {
    sendEmail = true;
    request->send_P(200, "text/plain", "Sending email...");
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, PHOTO_FILE_PATH, "image/jpg", false);
  });

  // Start server
  Serial.println("Starting main web server...");
  server.begin();
}
 
