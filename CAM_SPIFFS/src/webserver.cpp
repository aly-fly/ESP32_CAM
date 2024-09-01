#include <arduino.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "_CONFIG.h"


// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean takeNewPhoto = false;
boolean useFlash = false;


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
 
void webserverInit(){

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


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
    request->send_P(200, "text/plain", "Taking photo");
  });

  server.on("/capture-flash", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    useFlash = true;
    request->send_P(200, "text/plain", "Taking photo with flash");
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, PHOTO_FILE_PATH, "image/jpg", false);
  });



  // Start server
  Serial.println("Starting main web server...");
  server.begin();
}
 
