#include <arduino.h>
#include "esp_camera.h"
#include <LittleFS.h>
#include <FS.h>

size_t getFileSize(const char* filename);
time_t getFileTime(const char* filename);
