#include <arduino.h>
#include "esp_camera.h"

#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#include <LittleFS.h>
#include <FS.h>

#include "_CONFIG.h"

void setupLedFlash(int pin);


//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE  // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_CAMS3_UNIT  // Has PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"


void CameraInit(void) {
  Serial.println("Camera start...");  

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 16000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      Serial.println("PSRAM found.");  
      // psramInit();
      Serial.println((String)"Memory available in PSRAM : " +ESP.getFreePsram());
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      Serial.println("PSRAM  NOT found.");  
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err == ESP_OK) {
    Serial.println("Camera init ok.");
  } else {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  // set frame size
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_SVGA); // changing this setting resets clock settings!
  }

  // custom optimizations
  int res;
  res = s->set_agc_gain(s, 1); // enable
  log_i("set_agc_gain -> %d", res);
  res = s->set_gainceiling(s, GAINCEILING_32X); // max AGC allowed
  log_i("set_gainceiling -> %d", res);
  res = s->set_aec_value(s, 1); // AEC sensor
  log_i("set_aec_value -> %d", res);
  res = s->set_aec2(s, 1); // AEC DSP
  log_i("set_aec2 -> %d", res);
  res = s->set_lenc(s, 0); // lens correction off
  log_i("set_lenc -> %d", res);
  /*
  // CLK 2X Set Register: reg: 0x111, mask: 0x80, value: 0x80
  res = s->set_reg(s, 0x111, 0x80, 0x80);
  log_i("CLK 2X set_reg -> %d", res);
  */
  /*
  // PCLK DIV 6 Set Register: reg: 0xd3, mask: 0x7f, value: 0x06
  res = s->set_reg(s, 0xd3, 0x7f, 0x06);
  log_i("PCLK DIV 6 set_reg -> %d", res);
  */




// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

}

//===========================================================================================================

size_t LittleFSFilesize(const char* filename) {
  auto file = LittleFS.open(filename, FILE_READ);
  size_t filesize = file.size();
  // Don't forget to clean up!
  file.close();
  return filesize;
}

// Capture Photo and Save it to LittleFS
void capturePhotoSaveToFilesystem( void ) {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly
  size_t imageSize = 0;
  size_t fileSize = 0;

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");
    int64_t Time1 = esp_timer_get_time();
    fb = esp_camera_fb_get();
    if (!fb) {
      log_e("Camera capture failed");
      return;
    }
    int64_t Time2 = esp_timer_get_time();
    imageSize = fb->len;    
    Serial.printf("Capture: %u B, %u us\n", (uint32_t)(imageSize), (uint32_t)((Time2 - Time1)));
    yield();

    // Photo file name
    Serial.printf("Picture file name: %s\n", PHOTO_FILE_PATH);
    Time1 = esp_timer_get_time();
    File file = LittleFS.open(PHOTO_FILE_PATH, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      log_e("Failed to open file in writing mode");
    }
    else {
      log_d("File open.");
      yield();
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved.");
    }
    // Close the file (reading file size des not work before this!)
    file.close();
    Time2 = esp_timer_get_time();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in LittleFS
    fileSize = LittleFSFilesize(PHOTO_FILE_PATH);
    
    ok = fileSize > 100;
    Serial.printf("File save ok: %d\n", ok);


    Serial.printf("Save: %u B, %u ms\n", (uint32_t)(fileSize), (uint32_t)((Time2 - Time1) / 1000));    

    if (!ok) { delay (1500); }
  } while ( !ok );
}

