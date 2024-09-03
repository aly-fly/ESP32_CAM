#include "SaveSettings.h"
#include <Preferences.h>
#include "_CONFIG.h"
#include "_global_vars.h"

Preferences preferences;

void NVSReadSettings() {
  // Read WiFi credentials from NVRAM
  log_i("Reading from NVS start.");
  preferences.begin(NVS_NAMESPACE, true);  // read-only mode

  MyConfig.wifiValid      = preferences.getBool  ("wifiValid", false);
  MyConfig.wifiSsid       = preferences.getString("wifiSsid", DEFAULT_WIFI_SSID);
  MyConfig.wifiPass       = preferences.getString("wifiPass", DEFAULT_WIFI_PASS);
  MyConfig.senderEmail    = preferences.getString("senderEmail", "@gmail.com");
  MyConfig.senderPassword = preferences.getString("senderPassword", "");
  MyConfig.recipientEmail = preferences.getString("recipientEmail", "");
  MyConfig.recipientName  = preferences.getString("recipientName", "");
  MyConfig.deviceName     = preferences.getString("deviceName", DEFAULT_DEVICENAME);
  MyConfig.useFlash       = preferences.getString("useFlash", "1");

  preferences.end();
  log_i("Reading from NVS finished.");
}

void NVSWriteSettings() {
  log_i("Writing to NVS start.");
  preferences.begin(NVS_NAMESPACE, false); // read-write mode

  preferences.putBool  ("wifiValid",      MyConfig.wifiValid); 
  preferences.putString("wifiSsid",       MyConfig.wifiSsid); 
  preferences.putString("wifiPass",       MyConfig.wifiPass); 
  preferences.putString("senderEmail",    MyConfig.senderEmail); 
  preferences.putString("senderPassword", MyConfig.senderPassword); 
  preferences.putString("recipientEmail", MyConfig.recipientEmail); 
  preferences.putString("recipientName",  MyConfig.recipientName); 
  preferences.putString("deviceName",     MyConfig.deviceName); 
  preferences.putString("useFlash",       MyConfig.useFlash); 
  
  preferences.end();        
  log_i("Writing to NVS finished.");
}
