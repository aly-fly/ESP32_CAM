#include <Arduino.h>

typedef struct {
  String wifiSsid,
    wifiPass,
    senderEmail,
    senderPassword,
    recipientEmail,
    recipientName,
    deviceName,
    useFlash,
    autoSendEmail,
    emailBodyTxt;
  int autoSendDay,
      autoSendHour;
  bool wifiValid;
} configStruct_t;

extern configStruct_t MyConfig;

extern time_t currentTime;
extern time_t LastEmailSentT;
extern time_t LastEmailRetryMin;
