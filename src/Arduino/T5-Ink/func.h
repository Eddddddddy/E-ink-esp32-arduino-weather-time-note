/*------------------------------------------------------------ink_func.h----------------------*/
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include "board_def.h"
#include "text.h"

#include <U8g2_for_Adafruit_GFX.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBoldOblique18pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>



//#define DEFALUT_FONT  FreeMono9pt7b
// #define DEFALUT_FONT  FreeMonoBoldOblique9pt7b
// #define DEFALUT_FONT FreeMonoBold9pt7b
// #define DEFALUT_FONT FreeMonoOblique9pt7b
#define DEFALUT_FONT FreeSans9pt7b
// #define DEFALUT_FONT FreeSansBold9pt7b
// #define DEFALUT_FONT FreeSansBoldOblique9pt7b
// #define DEFALUT_FONT FreeSansOblique9pt7b
// #define DEFALUT_FONT FreeSerif9pt7b
// #define DEFALUT_FONT FreeSerifBold9pt7b
// #define DEFALUT_FONT FreeSerifBoldItalic9pt7b
// #define DEFALUT_FONT FreeSerifItalic9pt7b

const GFXfont *fonts[] = {
  &FreeSansBoldOblique18pt7b,
  &FreeSerifBold9pt7b,//2 ////
};

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

void displayInit(void);
//uint32_t read32(File &f);
//uint16_t read16(File &f);
void showMianPage(void);
void flashWeather();
void LCD_print(struct tm timeinfo);
void LCD_one(int num, int loc);
void second_weather();
void first_weather();
void displayText(const String &str, int16_t y, uint8_t alignment);
void u8g2Font_init();
void ink_init();
/*-------------------------------------------------------------wifi_func.h----------------------*/
#include "esp_wifi.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

AsyncWebServer server(80);
WiFiClient wifiClient;

WiFiMulti wifiMulti;
void WebServerStart(void);
/*------------------------------------------------------notice_func.h----------------------*/
#include "EEPROM.h"

typedef struct {
  char notice_1[64];
  char notice_2[64];
  char notice_3[64];
  char notice_4[64];
  char notice_5[64];
  char notice_6[64];
} Notice_struct;

Notice_struct notice_str;

void saveBadgeInfo(Notice_struct *notice);
void loadDefaultInfo(void);
bool loadBadgeInfo(Notice_struct *notice);
void notice_init();
/*---------------------------------------------------------page_func.h----------------------*/

static int page_state = 1;

void switch_page (int page);
void firstPage();
void secondPage();
void thirdPage();
/*-------------------------------------------------------button_func.h----------------------*/
#include <Button2.h>

Button2 *pBtns = nullptr;
uint8_t g_btns[] = BUTTONS_MAP;

void button_handle(uint8_t gpio);
void button_callback(Button2 &b);
void button_init();
void button_loop();
/*----------------------------------------------------------bme_func.h----------------------*/
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Wire.h>

void showBme680();
void flashBme();
void GetGasReference();
int CalculateIAQ(int score);
int GetHumidityScore();
int GetGasScore();
bool bme680_read();
void bme_init();
/*---------------------------------------------------------timer_func.h----------------------*/

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

hw_timer_t * timer = NULL;
hw_timer_t * timer2 = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int timer_1 = 1, timer_2_weather = 1, timer_2_battery = 1, timer_2_bme680 = 1;

void IRAM_ATTR onTimer();
void IRAM_ATTR onTimer2();
void timerRec();
void setTime();
void setWeather();
void timer_init();
void timer_begin();
/*-------------------------------------------------------battery_func.h----------------------*/
#include "esp_adc_cal.h"


#define ADC_PIN 35
int vref = 1100;
int adc_data = 0;
int Battery_level = 0;

void showVoltage();
void showBattery();
void batteryValue_init();
/*-------------------------------------------------------------other_func.h----------------------*/
/*100 * 100 bmp fromat*/
//https://www.onlineconverter.com/jpg-to-bmp
#define BADGE_CONFIG_FILE_NAME "/badge.data"
#define DEFALUT_AVATAR_BMP "/avatar.bmp"
#define DEFALUT_QR_CODE_BMP "/qr.bmp"
#define CHANNEL_0 0
#define IP5306_ADDR 0X75
#define IP5306_REG_SYS_CTL0 0x00

typedef struct {
  char name[32];
  char link[64];
  char tel[64];
  char company[64];
  char email[64];
  char address[128];
} Badge_Info_t;

typedef enum {
  RIGHT_ALIGNMENT = 0,
  LEFT_ALIGNMENT,
  CENTER_ALIGNMENT,
} Text_alignment;

Badge_Info_t info;

static const uint16_t input_buffer_pixels = 20;       // may affect performance
static const uint16_t max_palette_pixels = 256;       // for depth <= 8
uint8_t mono_palette_buffer[max_palette_pixels / 8];  // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
uint8_t input_buffer[3 * input_buffer_pixels];        // up to depth 24
const char *path[2] = {DEFALUT_AVATAR_BMP, DEFALUT_QR_CODE_BMP};

bool setPowerBoostKeepOn(int en);
void other_init();
/*-----------------------------------------------------------esp_power_func.h----------------------*/
#include <esp_pm.h>
#include <esp_wifi_types.h>
#include "esp32-hal-cpu.h"

RTC_DATA_ATTR int bootCount = 0;

void boot_init();
void powerSave();
/*----------------------------------------------------------mqtt_func.h----------------------*/
#include <PubSubClient.h>

typedef struct {
  char code_1[8];
  char code_2[8];
  char wea_1[64];
  char wea_2[64];
  char tmp_1_min[8];
  char tmp_1_max[8];
  char tmp_2_min[8];
  char tmp_2_max[8];
} Weather_struct;

typedef struct {
  char cpu[8];
  char gpu[8];
} Msi_info_struct;

Weather_struct wea_str;
Msi_info_struct msi_info_str;

int msi_info_state = 0;

void mqtt_callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(MQTT_SERVER, MQTT_PORT, mqtt_callback, wifiClient);
bool msi_info_mqtt_json(char s[]);
bool weather_mqtt_json(char s[]);

void reconnect();
/*-----------------------------------------------------------------ntp_func.h----------------------*/
#include "time.h"

#define ntpServer "pool.ntp.org"
const long  gmtOffset_sec = 3600 * TIMEZONE;
const int   daylightOffset_sec = 0;

void ntp_init();
