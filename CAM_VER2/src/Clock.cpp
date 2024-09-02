#include <Arduino.h>
#include <stdint.h>
#include <esp_sntp.h>
#include <time.h>
#include "_CONFIG.h"

unsigned long LastTimeClockSynced = 0; // data is not valid


// Required for WiFiClientSecure and for checks the validity date of the certificate. 
// Setting clock for CA authorization...
void setClock(void) {
  Serial.println("setClock()");

  if ((millis() < (LastTimeClockSynced + 60*60*1000)) && (LastTimeClockSynced != 0)) {  // check every hour
    return;  // clock is already synced
  }

  configTime(GMT_OFFSET*60*60, DST_OFFSET*60*60, TIME_SERVER);

  Serial.print("NTP time sync");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  
  LastTimeClockSynced = millis();
}

/* https://cplusplus.com/reference/ctime/tm/
Member	Type	Meaning	Range
tm_sec	int	seconds after the minute	0-60*
tm_min	int	minutes after the hour	0-59
tm_hour	int	hours since midnight	0-23
tm_mday	int	day of the month	1-31
tm_mon	int	months since January	0-11
tm_year	int	years since 1900	
tm_wday	int	days since Sunday	0-6
tm_yday	int	days since January 1	0-365
tm_isdst	int	Daylight Saving Time flag	
*/
bool GetCurrentTime(int &Month, int &Day, int &Hour, int &Minute) {
  struct tm timeinfo;
  bool result = getLocalTime(&timeinfo);
  Month = timeinfo.tm_mon + 1; // starts with 0
  Day   = timeinfo.tm_mday;
  Hour  = timeinfo.tm_hour;
  Minute  = timeinfo.tm_min;
  return result;
}

