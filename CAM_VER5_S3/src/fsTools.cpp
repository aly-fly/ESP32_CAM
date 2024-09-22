
#include "fsTools.h"

size_t getFileSize(const char* filename) {
  auto file = LittleFS.open(filename, FILE_READ);
  size_t filesize = file.size();
  file.close();
  return filesize;
}

time_t getFileTime(const char* filename) {
  auto file = LittleFS.open(filename, FILE_READ);
  time_t fileTime = file.getLastWrite();
  file.close();
  return fileTime;
}

/*
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm *tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
*/

