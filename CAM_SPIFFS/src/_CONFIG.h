
// #define WEBSERVER_TEST  1

// Photo File Name to save in SPIFFS
#define PHOTO_FILE_NAME "photo.jpg"
#define PHOTO_FILE_PATH "/photo.jpg"

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

