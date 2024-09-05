#include <arduino.h>
#include "ESPAsyncWebServer.h"
//#include <ESPmDNS.h>
#include "LittleFS.h"
#include "fsTools.h"
#include "esp_camera.h"
#include "_CONFIG.h"
#include <_global_vars.h>
#include "SaveSettings.h"
#include "Clock.h"

String ledState;
String currentStatus;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean takeNewPhoto = false;
boolean sendEmail = false;
boolean emailLiveImage = false;


// config web page
String placeholderPocessor(const String& var){
  // LED web page
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

  // config and other pages
  if(var == "wifiSsid")       { return MyConfig.wifiSsid; }
  if(var == "wifiPass")       { return MyConfig.wifiPass; }
  if(var == "senderEmail")    { return MyConfig.senderEmail; }
  if(var == "senderPassword") { return MyConfig.senderPassword; }
  if(var == "recipientEmail") { return MyConfig.recipientEmail; }
  if(var == "recipientName")  { return MyConfig.recipientName; }
  if(var == "deviceName")     { return MyConfig.deviceName; }
  if(var == "useFlash")       { return MyConfig.useFlash; }
  if(var == "autoSendEmail")  { return MyConfig.autoSendEmail; }
  if(var == "emailBodyTxt")   { return MyConfig.emailBodyTxt; }
  
  if(var == "SavedPhotoTimestamp") {
    time_t fileTime = getFileTime(PHOTO_FILE_wPATH);
    return timeToString(fileTime);
  }
  if(var == "LivePhotoTimestamp") {
    return timeToString(currentTime);
  }
  if(var == "LastEmailSentT") {
    return timeToString(LastEmailSentT);
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
    request->send(LittleFS, "/favicon.ico", "image/x-icon", false);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    log_i("Server Root request.");
    NVSReadData();
    //log_i("Client: ", request->client()->remoteIP());
    request->send(LittleFS, "/index.html", "text/html", false, placeholderPocessor);
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, PHOTO_FILE_wPATH, "image/jpg", false);
    time_t fileTime = getFileTime(PHOTO_FILE_wPATH);
    Serial.print("Saved file timestamp: ");
    Serial.println(timeToString(fileTime));
  });

  server.on("/live-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    // CAPTURE AND SEND FROM RAM
    camera_fb_t *fb = esp_camera_fb_get();

    // COPY data to the stream
    AsyncResponseStream *response =
        request->beginResponseStream("image/jpg", fb->len);
    response->write(fb->buf, fb->len);
    request->send(response);
    yield();
    esp_camera_fb_return(fb);
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
// following line causes "Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled."
//    if (request->hasParam("useFlash")) { MyConfig.useFlash = request->getParam("useFlash", true)->value(); }
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking photo...");
    currentStatus = "Taking photo in progress..";
  });


  server.on("/send-email", HTTP_GET, [](AsyncWebServerRequest * request) {
    sendEmail = true;
    emailLiveImage = false;
    request->send_P(200, "text/plain", "Sending email (saved)...");
    currentStatus = "Sending email in progress..";
  });

  server.on("/send-email-live", HTTP_GET, [](AsyncWebServerRequest * request) {
    sendEmail = true;
    emailLiveImage = true;
    request->send_P(200, "text/plain", "Sending email (live)...");
    currentStatus = "Sending email in progress..";
  });

  server.on("/getStatus", HTTP_GET, [](AsyncWebServerRequest * request) {
    String response;
    response = currentStatus + "   [wifi " + String((WiFi.RSSI() + 90)*2) + " %]"; // indicator approx 0..100
    request->send_P(200, "text/plain", response.c_str());
  });  

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
    NVSReadSettings();
    request->send(LittleFS, "/config.html", "text/html", false, placeholderPocessor);
  }); 


  // Handle a POST request to <IP>/saveConfig with listed form fields
  server.on("/saveConfig", HTTP_POST, [](AsyncWebServerRequest *request){
    log_i("Server /saveConfig POST request.");
    if (request->hasParam("wifiSsid", true))       { MyConfig.wifiSsid       = request->getParam("wifiSsid", true)->value(); }
    if (request->hasParam("wifiPass", true))       { MyConfig.wifiPass       = request->getParam("wifiPass", true)->value(); }
    if (request->hasParam("senderEmail", true))    { MyConfig.senderEmail    = request->getParam("senderEmail", true)->value(); }
    if (request->hasParam("senderPassword", true)) { MyConfig.senderPassword = request->getParam("senderPassword", true)->value(); }
    if (request->hasParam("recipientEmail", true)) { MyConfig.recipientEmail = request->getParam("recipientEmail", true)->value(); }
    if (request->hasParam("recipientName", true))  { MyConfig.recipientName  = request->getParam("recipientName", true)->value(); }
    if (request->hasParam("deviceName", true))     { MyConfig.deviceName     = request->getParam("deviceName", true)->value(); }
    if (request->hasParam("useFlash", true))       { MyConfig.useFlash       = request->getParam("useFlash", true)->value(); }
    if (request->hasParam("autoSendEmail", true))  { MyConfig.autoSendEmail  = request->getParam("autoSendEmail", true)->value(); }
    if (request->hasParam("emailBodyTxt", true))   { MyConfig.emailBodyTxt   = request->getParam("emailBodyTxt", true)->value(); }

    NVSWriteSettings();  
    request->send(200, "text/plain", "Settings saved.");
    currentStatus = "Settings saved.";
  });



  // Route for root / web page
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/indexLED.html", "text/html", false, placeholderPocessor);
  });
  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, LOW);    
    request->send(LittleFS, "/indexLED.html", "text/html", false, placeholderPocessor);
  });
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_RED_GPIO_NUM, HIGH);    
    request->send(LittleFS, "/indexLED.html", "text/html", false, placeholderPocessor);
  });



  // Start server
  log_i("Starting main web server...");
  server.begin();
}
 
