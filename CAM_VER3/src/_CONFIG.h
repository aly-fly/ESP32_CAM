
// #define WEBSERVER_TEST  1
#define DEFAULT_DEVICENAME "ESP32 camera"
#define DEFAULT_WIFI_SSID  "ConfigWifi"
#define DEFAULT_WIFI_PASS  "ConfigWifiPass"

#define NVS_NAMESPACE_CONFIG  "MyConfig"
#define NVS_NAMESPACE_DATA    "RuntimeData"

// Photo File Name to save in the file system
#define PHOTO_FILE_NAME "photo.jpg"
#define PHOTO_FILE_wPATH "/photo.jpg"

// Set small red LED GPIO
#define LED_RED_GPIO_NUM  33
#define LED_FLASH_GPIO_NUM  4


#define TIME_SERVER  "si.pool.ntp.org"  // "pool.ntp.org"
#define GMT_OFFSET  1
#define DST_OFFSET  1


// check that settings are correct in this file:
#include <ESP_Mail_FS.h>

#define SMTP_HOST "smtp.gmail.com"
#define MSG_HEADER "Message-ID: <user1@gmail.com>"

