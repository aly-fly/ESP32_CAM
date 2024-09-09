#include "SaveSettings.h"
#include <Preferences.h>
#include "_CONFIG.h"
#include "_global_vars.h"

Preferences preferences;

void NVSReadSettings() {
  // Read WiFi credentials from NVRAM
  log_i("Reading from NVS (config) start.");
  preferences.begin(NVS_NAMESPACE_CONFIG, true);  // read-only mode

  MyConfig.wifiValid      = preferences.getBool  ("wifiValid", false);
  MyConfig.wifiSsid       = preferences.getString("wifiSsid", DEFAULT_WIFI_SSID);
  MyConfig.wifiPass       = preferences.getString("wifiPass", DEFAULT_WIFI_PASS);
  MyConfig.senderEmail    = preferences.getString("senderEmail", "@gmail.com");
  MyConfig.senderPassword = preferences.getString("senderPassword", "");
  MyConfig.recipientEmail = preferences.getString("recipientEmail", "@");
  MyConfig.recipientName  = preferences.getString("recipientName", "");
  MyConfig.deviceName     = preferences.getString("deviceName", DEFAULT_DEVICENAME);
  MyConfig.useFlash       = preferences.getString("useFlash", "1");
  MyConfig.autoSendEmail  = preferences.getString("autoSendEmail", "1");
  MyConfig.emailBodyTxt   = preferences.getString("emailBodyTxt", "Emai txt.");
  MyConfig.autoSendDay    = preferences.getInt   ("autoSendDay", 28);
  MyConfig.autoSendHour   = preferences.getInt   ("autoSendHour", 18); 

  preferences.end();
  log_i("Reading from NVS (config) finished.");
}

void NVSWriteSettings() {
  log_i("Writing to NVS (config) start.");
  preferences.begin(NVS_NAMESPACE_CONFIG, false); // read-write mode

  preferences.putBool  ("wifiValid",      MyConfig.wifiValid); 
  preferences.putString("wifiSsid",       MyConfig.wifiSsid); 
  preferences.putString("wifiPass",       MyConfig.wifiPass); 
  preferences.putString("senderEmail",    MyConfig.senderEmail); 
  preferences.putString("senderPassword", MyConfig.senderPassword); 
  preferences.putString("recipientEmail", MyConfig.recipientEmail); 
  preferences.putString("recipientName",  MyConfig.recipientName); 
  preferences.putString("deviceName",     MyConfig.deviceName); 
  preferences.putString("useFlash",       MyConfig.useFlash); 
  preferences.putString("autoSendEmail",  MyConfig.autoSendEmail);
  preferences.putString("emailBodyTxt",   MyConfig.emailBodyTxt); 
  preferences.putInt   ("autoSendDay",    MyConfig.autoSendDay); 
  preferences.putInt   ("autoSendHour",   MyConfig.autoSendHour); 
  
  preferences.end();        
  log_i("Writing to NVS (config) finished. ");
}

// ==================================================================================================

void NVSReadData() {
  // Read WiFi credentials from NVRAM
  log_i("Reading from NVS (data) start. ");
  preferences.begin(NVS_NAMESPACE_DATA, true);  // read-only mode

  LastEmailSentT = preferences.getLong ("LastEmailSentT", 0);
  
  preferences.end();
  log_i("Reading from NVS (data) finished. ");
}

void NVSWriteData() {
  log_i("Writing to NVS (data) start. ");
  preferences.begin(NVS_NAMESPACE_DATA, false); // read-write mode

  preferences.putLong("LastEmailSentT", LastEmailSentT); 
  
  preferences.end();        
  log_i("Writing to NVS (data) finished. ");
}
