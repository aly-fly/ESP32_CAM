#include <Arduino.h>

typedef struct {
    String wifiSsid,
    wifiPass,
    senderEmail,
    senderPassword,
    recipientEmail,
    recipientName,
    deviceName,
    useFlash;
    bool wifiValid;
} configStruct_t;

extern configStruct_t MyConfig;

