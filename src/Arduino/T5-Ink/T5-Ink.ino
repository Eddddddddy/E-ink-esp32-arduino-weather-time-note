// include library, include base class, make path known
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

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

#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "SD.h"
#include "SPI.h"
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "esp_wifi.h"
#include "Esp.h"
#include "board_def.h"
#include <Button2.h>
#include <PubSubClient.h>
#include "time.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "esp_adc_cal.h"
#include <esp_pm.h>
#include <esp_wifi_types.h>
#include "esp32-hal-cpu.h"
#include "config.h"

#define FILESYSTEM SPIFFS

/*100 * 100 bmp fromat*/
//https://www.onlineconverter.com/jpg-to-bmp
#define BADGE_CONFIG_FILE_NAME "/badge.data"
#define DEFALUT_AVATAR_BMP "/avatar.bmp"
#define DEFALUT_QR_CODE_BMP "/qr.bmp"
#define CHANNEL_0 0
#define IP5306_ADDR 0X75
#define IP5306_REG_SYS_CTL0 0x00
#define SEALEVELPRESSURE_HPA (1002.2)
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

RTC_DATA_ATTR int bootCount = 0;

void showMianPage(void);
void showQrPage(void);
void displayInit(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);

typedef struct {
  char name[32];
  char link[64];
  char tel[64];
  char company[64];
  char email[64];
  char address[128];
} Badge_Info_t;

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
  char notice_1[64];
  char notice_2[64];
  char notice_3[64];
  char notice_4[64];
  char notice_5[64];
  char notice_6[64];
} Notice_struct;

typedef struct {
  char cpu[8];
  char gpu[8];
} Msi_info_struct;

typedef enum {
  RIGHT_ALIGNMENT = 0,
  LEFT_ALIGNMENT,
  CENTER_ALIGNMENT,
} Text_alignment;

typedef struct {
  int humidity;
  int pressure;
  int temperature;
  int iaq;
} bme680_struct;

AsyncWebServer server(80);
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, MQTT_PORT, mqtt_callback, wifiClient);

WiFiMulti wifiMulti;

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

Badge_Info_t info;
Weather_struct wea_str;
Notice_struct notice_str;
Msi_info_struct msi_info_str;
bme680_struct bme680_str;

static const uint16_t input_buffer_pixels = 20;       // may affect performance
static const uint16_t max_palette_pixels = 256;       // for depth <= 8
uint8_t mono_palette_buffer[max_palette_pixels / 8];  // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
uint8_t input_buffer[3 * input_buffer_pixels];        // up to depth 24
const char *path[2] = {DEFALUT_AVATAR_BMP, DEFALUT_QR_CODE_BMP};

Button2 *pBtns = nullptr;
uint8_t g_btns[] = BUTTONS_MAP;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * TIMEZONE;
const int   daylightOffset_sec = 0;

hw_timer_t * timer = NULL;
hw_timer_t * timer2 = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

Adafruit_BME680 bme; // I2C


static int page_state = 1;

int timer_1 = 1, timer_2_weather = 1, timer_2_battery = 1, timer_2_bme680 = 1;
int msi_info_state = 0;

#define ADC_PIN         35
int vref = 1100;
int adc_data = 0;
int Battary_level = 0;

//bme680
float hum_weighting = 0.25; // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75; // so gas effect is 75% of the total air quality score
int   humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int   getgasreference_count = 0;
int   gas_lower_limit = 10000;  // Bad air quality limit
int   gas_upper_limit = 300000; // Good air quality limit
int   bme680_errTime = 0;

void button_handle(uint8_t gpio)
{
  switch (gpio) {
#if BUTTON_1
    case BUTTON_1: {
        //        // esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1, LOW);
        //        esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);
        //        Serial.println("Going to sleep now");
        //        delay(2000);
        //        esp_deep_sleep_start();

        //      esp_sleep_enable_timer_wakeup(5 * uS_TO_S_FACTOR);
        //      Serial.println("Going to sleep now");
        //      Serial.flush();
        //      //Wire.end();
        //      display.powerDown();
        //      esp_deep_sleep_start();

        client.publish("to_led", "2");

      }
      break;
#endif

#if BUTTON_2
    case BUTTON_2: {
        //static int i = 0;
        page_state = ((page_state + 1) > 3) ? 1 : page_state + 1;
        Serial.printf("Show Num: %d page\n", page_state);

        if (page_state == 3) {
          //client.publish("msi_info_call", "on");
          msi_info_state = 1;
        } else if (page_state == 1) {
          //client.publish("msi_info_call", "off");
          msi_info_state = 0;
          display.fillRect(99, 0, 172, 128, GxEPD_BLACK);
          display.updateWindow(99, 0, 172, 128, true);
          display.fillRect(99, 0, 172, 128, GxEPD_WHITE);
          display.updateWindow(99, 0, 172, 128, true);
          //            display.fillRect(101, 0, 170, 127, GxEPD_BLACK);
          //            display.updateWindow(101, 0, 170, 127, true);
        }
        switch_page(page_state);
      }
      break;
#endif

#if BUTTON_3
    case BUTTON_3: {
        // static bool index = 1;
        // if (!index) {
        //     showMianPage();
        //     index = true;
        // } else {
        //     showQrPage();
        //     index = false;
        // }
        client.publish("to_printer", "2");
      }
      break;
#endif
    default:
      break;
  }
}

void button_callback(Button2 &b)
{
  for (int i = 0; i < sizeof(g_btns) / sizeof(g_btns[0]); ++i) {
    if (pBtns[i] == b) {
      Serial.printf("btn: %u press\n", pBtns[i].getAttachPin());
      button_handle(pBtns[i].getAttachPin());
    }
  }
}

void button_init()
{
  uint8_t args = sizeof(g_btns) / sizeof(g_btns[0]);
  pBtns = new Button2[args];
  for (int i = 0; i < args; ++i) {
    pBtns[i] = Button2(g_btns[i]);
    pBtns[i].setPressedHandler(button_callback);
  }
}

void button_loop()
{
  for (int i = 0; i < sizeof(g_btns) / sizeof(g_btns[0]); ++i) {
    pBtns[i].loop();
  }
}

void displayText(const String &str, int16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

  switch (alignment) {
    case RIGHT_ALIGNMENT:
      display.setCursor(display.width() - w - x1, y);
      break;
    case LEFT_ALIGNMENT:
      display.setCursor(0, y);
      break;
    case CENTER_ALIGNMENT:
      display.setCursor(display.width() / 2 - ((w + x1) / 2), y);
      break;
    default:
      break;
  }
  display.println(str);
}

void saveBadgeInfo(Notice_struct *notice)
{
  // Open file for writing
  File file = FILESYSTEM.open(BADGE_CONFIG_FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
#endif
  // Set the values
  root["notice_1"] = notice->notice_1;
  root["notice_2"] = notice->notice_2;
  root["notice_3"] = notice->notice_3;
  root["notice_4"] = notice->notice_4;
  root["notice_5"] = notice->notice_5;
  root["notice_6"] = notice->notice_6;

#if ARDUINOJSON_VERSION_MAJOR == 5
  if (root.printTo(file) == 0)
#elif ARDUINOJSON_VERSION_MAJOR == 6
  if (serializeJson(root, file) == 0)
#endif
  {
    Serial.println(F("Failed to write to file"));
  }
  // Close the file (File's destructor doesn't close the file)
  file.close();
}

void loadDefaultInfo(void)
{
  strlcpy(notice_str.notice_1, "无消息", sizeof(notice_str.notice_1));
  strlcpy(notice_str.notice_2, "无消息", sizeof(notice_str.notice_2));
  strlcpy(notice_str.notice_3, "无消息", sizeof(notice_str.notice_3));
  strlcpy(notice_str.notice_4, "无消息", sizeof(notice_str.notice_4));
  strlcpy(notice_str.notice_5, "无消息", sizeof(notice_str.notice_5));
  strlcpy(notice_str.notice_6, "无消息", sizeof(notice_str.notice_6));
  //    strlcpy(info.name, "Lilygo", sizeof(info.name));
  //    strlcpy(info.address, "ShenZhen", sizeof(info.address));
  //    strlcpy(info.email, "lily@lilygo.cc", sizeof(info.email));
  //    strlcpy(info.link, "http://www.lilygo.cn", sizeof(info.link));
  //    strlcpy(info.tel, "0755-83380665", sizeof(info.tel));
  saveBadgeInfo(&notice_str);
}

bool loadBadgeInfo(Notice_struct *notice)
{
  if (!FILESYSTEM.exists(BADGE_CONFIG_FILE_NAME)) {
    Serial.println("load configure fail");
    return false;
  }

  File file = FILESYSTEM.open(BADGE_CONFIG_FILE_NAME);
  if (!file) {
    Serial.println("Open Fial -->");
    return false;
  }

#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(file);
  if (!root.success()) {
    Serial.println(F("Failed to read file, using default configuration"));
    file.close();
    return false;
  }
  root.printTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
  DeserializationError error = deserializeJson(root, file);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
    file.close();
    return false;
  }
#endif
  if ((const char *)root["notice_6"] == NULL) {
    return false;
  }

  strlcpy(notice->notice_1, root["notice_1"], sizeof(notice->notice_1));
  strlcpy(notice->notice_2, root["notice_2"], sizeof(notice->notice_2));
  strlcpy(notice->notice_3, root["notice_3"], sizeof(notice->notice_3));
  strlcpy(notice->notice_4, root["notice_4"], sizeof(notice->notice_4));
  strlcpy(notice->notice_5, root["notice_5"], sizeof(notice->notice_5));
  strlcpy(notice->notice_6, root["notice_6"], sizeof(notice->notice_6));
  //    strlcpy(info->name, root["name"], sizeof(info->name));
  //    strlcpy(info->address, root["address"], sizeof(info->address));
  //    strlcpy(info->email, root["email"], sizeof(info->email));
  //    strlcpy(info->link, root["link"], sizeof(info->link));
  //    strlcpy(info->tel, root["tel"], sizeof(info->tel));
  file.close();
  return true;
}

void WebServerStart(void)
{

#ifdef USE_AP_MODE
  uint8_t mac[6];
  char apName[18] = {0};
  IPAddress apIP = IPAddress(192, 168, 1, 1);

  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  esp_wifi_get_mac(WIFI_IF_STA, mac);

  sprintf(apName, "TTGO-Badge-%02X:%02X", mac[4], mac[5]);

  if (!WiFi.softAP(apName)) {
    Serial.println("AP Config failed.");
    return;
  } else {
    Serial.println("AP Config Success.");
    Serial.print("AP MAC: ");
    Serial.println(WiFi.softAPmacAddress());
  }
#else
  WiFi.mode(WIFI_STA);
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);
  int tryTime = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    //while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tryTime++;
    if (tryTime > 10) {
      esp_restart();
    }
  }
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(WiFi.localIP());
#endif

#ifdef WEBSERVER
  {
    if (MDNS.begin("ttgo")) {
      Serial.println("MDNS responder started");
    }

    server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.html");

    server.on("css/main.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "css/main.css", "text/css");
    });
    server.on("js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "js/jquery.min.js", "application/javascript");
    });
    server.on("js/tbdValidate.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "js/tbdValidate.js", "application/javascript");
    });
    server.on("/data", HTTP_POST, [](AsyncWebServerRequest * request) {
      request->send(200, "text/plain", "");

      for (int i = 0; i < request->params(); i++) {
        String name = request->getParam(i)->name();
        String params = request->getParam(i)->value();
        Serial.println(name + " : " + params);
        if (name == "company") {
          strlcpy(info.company, params.c_str(), sizeof(info.company));
        } else if (name == "name") {
          strlcpy(info.name, params.c_str(), sizeof(info.name));
        } else if (name == "address") {
          strlcpy(info.address, params.c_str(), sizeof(info.address));
        } else if (name == "email") {
          strlcpy(info.email, params.c_str(), sizeof(info.email));
        } else if (name == "link") {
          strlcpy(info.link, params.c_str(), sizeof(info.link));
        } else if (name == "tel") {
          strlcpy(info.tel, params.c_str(), sizeof(info.tel));
        }
      }
      saveBadgeInfo(&info);
    });

    server.onFileUpload([](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
      static File file;
      static int pathIndex = 0;
      if (!index) {
        Serial.printf("UploadStart: %s\n", filename.c_str());
        file = FILESYSTEM.open(path[pathIndex], FILE_WRITE);
        if (!file) {
          Serial.println("Open FAIL");
          request->send(500, "text/plain", "hander error");
          return;
        }
      }
      if (file.write(data, len) != len) {
        Serial.println("Write fail");
        request->send(500, "text/plain", "hander error");
        file.close();
        return;
      }

      if (final) {
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
        file.close();
        request->send(200, "text/plain", "");
        if (++pathIndex >= 2) {
          pathIndex = 0;
          showMianPage();
        }
      }
    });

    server.onNotFound([](AsyncWebServerRequest * request) {
      request->send(404, "text/plain", "Not found");
    });

    MDNS.addService("http", "tcp", 80);

    server.begin();
  }
#endif
}

void first_weather() {
  int low_l, low_h, high_l, high_h;
  low_l = abs(atoi(wea_str.tmp_1_min) % 10);
  low_h = abs(atoi(wea_str.tmp_1_min) / 10);
  high_l = abs(atoi(wea_str.tmp_1_max) % 10);
  high_h = abs(atoi(wea_str.tmp_1_max) / 10);

  display.fillRect(0, 65, 100, 31, GxEPD_BLACK);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

  display.drawBitmap(3, 68, weather_icon[atoi(wea_str.code_1)], 25, 25, GxEPD_WHITE); //weather_icon
  //display.drawBitmap(33, 68, weather_text[num_code], 24, 12,GxEPD_WHITE);//天气
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setCursor(33, 80);
  u8g2_for_adafruit_gfx.print(wea_str.wea_1);
  if (atoi(wea_str.tmp_1_min) > 0) {
    display.drawBitmap(30, 80, num[low_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 8, 80, num[low_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_1_min) < -9) {
    display.drawBitmap(30, 80, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 8, 80, num[low_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 16, 80, num[low_l], 8, 16, GxEPD_WHITE); //个位
  } else if (atoi(wea_str.tmp_1_min) < 0) {
    display.drawBitmap(30, 80, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 8, 80, num[low_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_1_min) == 0) {
    display.drawBitmap(30 + 8, 80, num[0], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  }

  display.drawBitmap(30 + 24, 80, num_, 16, 16, GxEPD_WHITE); //--

  if (atoi(wea_str.tmp_1_max) > 0) {
    display.drawBitmap(30 + 40, 80, num[high_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 48, 80, num[high_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_1_max) < -9) {
    display.drawBitmap(30 + 40, 80, num[11], 8, 16, GxEPD_WHITE); //--
    display.drawBitmap(30 + 48, 80, num[high_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 56, 80, num[high_l], 8, 16, GxEPD_WHITE); //个位
  } else if (atoi(wea_str.tmp_1_max) < 0) {
    display.drawBitmap(30 + 40, 80, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 48, 80, num[high_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_1_max) == 0) {
    display.drawBitmap(30 + 48, 80, num[0], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80, num[10], 8, 16, GxEPD_WHITE); //℃
  }


  display.updateWindow(0, 65, 100, 31, true);
}

void second_weather() {
  int low_l, low_h, high_l, high_h;
  low_l = abs(atoi(wea_str.tmp_2_min) % 10);
  low_h = abs(atoi(wea_str.tmp_2_min) / 10);
  high_l = abs(atoi(wea_str.tmp_2_max) % 10);
  high_h = abs(atoi(wea_str.tmp_2_max) / 10);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);

  display.fillRect(0, 98, 100, 30, GxEPD_BLACK);
  display.drawBitmap(3, 68 + 32, weather_icon[atoi(wea_str.code_2)], 25, 25, GxEPD_WHITE); //weather_icon
  //display.drawBitmap(33, 68+32, weather_text[num_code], 24, 12,GxEPD_WHITE);//天气
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setCursor(33, 80 + 32);
  u8g2_for_adafruit_gfx.print(wea_str.wea_2);
  if (atoi(wea_str.tmp_2_min) > 0) {
    display.drawBitmap(30, 80 + 32, num[low_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 8, 80 + 32, num[low_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_2_min) < -9) {
    display.drawBitmap(30, 80 + 32, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 8, 80 + 32, num[low_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 16, 80 + 32, num[low_l], 8, 16, GxEPD_WHITE); //个位
  } else if (atoi(wea_str.tmp_2_min) < 0) {
    display.drawBitmap(30, 80 + 32, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 8, 80 + 32, num[low_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_2_min) == 0) {
    display.drawBitmap(30 + 8, 80 + 32, num[0], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 16, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  }

  display.drawBitmap(30 + 24, 80 + 32, num_, 16, 16, GxEPD_WHITE); //--

  if (atoi(wea_str.tmp_2_max) > 0) {
    display.drawBitmap(30 + 40, 80 + 32, num[high_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 48, 80 + 32, num[high_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_2_max) < -9) {
    display.drawBitmap(30 + 40, 80 + 32, num[11], 8, 16, GxEPD_WHITE); //--
    display.drawBitmap(30 + 48, 80 + 32, num[high_h], 8, 16, GxEPD_WHITE); //十位
    display.drawBitmap(30 + 56, 80 + 32, num[high_l], 8, 16, GxEPD_WHITE); //个位
  } else if (atoi(wea_str.tmp_2_max) < 0) {
    display.drawBitmap(30 + 40, 80 + 32, num[11], 8, 16, GxEPD_WHITE); //-
    display.drawBitmap(30 + 48, 80 + 32, num[high_l], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  } else if (atoi(wea_str.tmp_2_max) == 0) {
    display.drawBitmap(30 + 48, 80 + 32, num[0], 8, 16, GxEPD_WHITE); //个位
    display.drawBitmap(30 + 56, 80 + 32, num[10], 8, 16, GxEPD_WHITE); //℃
  }


  display.updateWindow(0, 98, 100, 29, true);
}

void switch_page (int page) {
  switch (page) {
    case 1: {
        display.fillRect(271, 0, 26, 42, GxEPD_WHITE);
        display.drawBitmap(272, 10, home_icon, 23, 23, GxEPD_BLACK);
        display.fillRect(271, 44, 26, 42, GxEPD_BLACK);
        display.drawBitmap(273, 54, txt_icon, 23, 23, GxEPD_WHITE);
        display.fillRect(271, 88, 26, 44, GxEPD_BLACK);
        display.drawBitmap(272, 97, inf_icon, 23, 23, GxEPD_WHITE);
        firstPage();
      } break;
    case 2: {
        display.fillRect(271, 0, 26, 42, GxEPD_BLACK);
        display.drawBitmap(272, 10, home_icon, 23, 23, GxEPD_WHITE);
        display.fillRect(271, 44, 26, 42, GxEPD_WHITE);
        display.drawBitmap(273, 54, txt_icon, 23, 23, GxEPD_BLACK);
        display.fillRect(271, 88, 26, 44, GxEPD_BLACK);
        display.drawBitmap(272, 97, inf_icon, 23, 23, GxEPD_WHITE);
        secondPage();
      } break;
    case 3: {
        display.fillRect(271, 0, 26, 42, GxEPD_BLACK);
        display.drawBitmap(272, 10, home_icon, 23, 23, GxEPD_WHITE);
        display.fillRect(271, 44, 26, 42, GxEPD_BLACK);
        display.drawBitmap(273, 54, txt_icon, 23, 23, GxEPD_WHITE);
        display.fillRect(271, 88, 26, 44, GxEPD_WHITE);
        display.drawBitmap(272, 97, inf_icon, 23, 23, GxEPD_BLACK);
        thirdPage();
      } break;
  }
  //display.fillScreen(GxEPD_BLACK);
  //display.updateWindow(271, 0, 26, 127, true);
  // display.updateWindow(101, 0, 196, 127, true);
}

void LCD_one(int num, int loc) {
  uint16_t show_1, show_2, show_3, show_4, show_5, show_6, show_7;
  int x;
  switch (loc) {
    case 1: x = 0; break;
    case 2: x = 34; break;
    case 3: x = 77; break;
    case 4: x = 111; break;
  }
  if (num == 4 || num == 5 || num == 6 || num == 8 || num == 9 || num == 0)show_1 = GxEPD_BLACK; else show_1 = GxEPD_WHITE;
  if (num == 2 || num == 6 || num == 8 || num == 0)show_2 = GxEPD_BLACK; else show_2 = GxEPD_WHITE;
  if (num == 1 || num == 2 || num == 3 || num == 4 || num == 7 || num == 8 || num == 9 || num == 0)show_3 = GxEPD_BLACK; else show_3 = GxEPD_WHITE;
  if (num == 1 || num == 3 || num == 4 || num == 5 || num == 6 || num == 7 || num == 8 || num == 9 || num == 0)show_4 = GxEPD_BLACK; else show_4 = GxEPD_WHITE;
  if (num == 2 || num == 3 || num == 5 || num == 6 || num == 7 || num == 8 || num == 9 || num == 0)show_5 = GxEPD_BLACK; else show_5 = GxEPD_WHITE;
  if (num == 2 || num == 3 || num == 4 || num == 5 || num == 6 || num == 8 || num == 9)show_6 = GxEPD_BLACK; else show_6 = GxEPD_WHITE;
  if (num == 2 || num == 3 || num == 5 || num == 6 || num == 8 || num == 9 || num == 0)show_7 = GxEPD_BLACK; else show_7 = GxEPD_WHITE;
  display.drawBitmap(115 + x, 20, LCD_l, 5, 24, show_1); //数码管
  display.drawBitmap(115 + x, 47, LCD_l, 5, 24, show_2); //数码管
  display.drawBitmap(140 + x, 20, LCD_l, 5, 24, show_3); //数码管
  display.drawBitmap(140 + x, 47, LCD_l, 5, 24, show_4); //数码管
  display.drawBitmap(119 + x, 16, LCD_h, 22, 5, show_5); //数码管
  display.drawBitmap(119 + x, 43, LCD_h, 22, 5, show_6); //数码管
  display.drawBitmap(119 + x, 70, LCD_h, 22, 5, show_7); //数码管
}

void LCD_print(struct tm timeinfo) {
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  int hour_h, hour_l, min_h, min_l;
  hour_l = timeinfo.tm_hour % 10;
  hour_h = timeinfo.tm_hour / 10;
  min_l = timeinfo.tm_min % 10;
  min_h = timeinfo.tm_min / 10;
  LCD_one(hour_h, 1);
  LCD_one(hour_l, 2);
  LCD_one(min_h, 3);
  LCD_one(min_l, 4);
  display.drawBitmap(183, 31, LCD_d, 5, 5, GxEPD_BLACK); //数码管
  display.drawBitmap(183, 55, LCD_d, 5, 5, GxEPD_BLACK); //数码管
}

void firstPage() {
  display.fillRect(99, 0, 172, 128, GxEPD_WHITE);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  LCD_print(timeinfo);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(111, 105);
  u8g2_for_adafruit_gfx.print(notice_str.notice_1);

  display.updateWindow(99, 0, 196, 128, true);//196 127
  //display.updateWindow(101, 0, 170, 127, true);
}

void secondPage() {



  display.fillRect(99, 0, 172, 128, GxEPD_WHITE);
  display.fillRect(111, 22, 150, 1, GxEPD_BLACK);
  display.fillRect(111, 43, 150, 1, GxEPD_BLACK);
  display.fillRect(111, 64, 150, 1, GxEPD_BLACK);
  display.fillRect(111, 86, 150, 1, GxEPD_BLACK);
  display.fillRect(111, 107, 150, 1, GxEPD_BLACK);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);

  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(111, 18);
  //u8g2_for_adafruit_gfx.print(notice_str.notice_1);
  u8g2_for_adafruit_gfx.print(notice_str.notice_1);
  u8g2_for_adafruit_gfx.setCursor(111, 39);
  u8g2_for_adafruit_gfx.print(notice_str.notice_2);

  u8g2_for_adafruit_gfx.setCursor(111, 60);
  u8g2_for_adafruit_gfx.print(notice_str.notice_3);
  u8g2_for_adafruit_gfx.setCursor(111, 81);
  u8g2_for_adafruit_gfx.print(notice_str.notice_4);

  u8g2_for_adafruit_gfx.setCursor(111, 103);
  u8g2_for_adafruit_gfx.print(notice_str.notice_5);
  u8g2_for_adafruit_gfx.setCursor(111, 124);
  u8g2_for_adafruit_gfx.print(notice_str.notice_6);

  display.updateWindow(99, 0, 196, 128, true);
  //display.updateWindow(101, 0, 170, 127, true);
}

void thirdPage() {
  int cpu = 50, gpu = 50;
  display.fillRect(99, 0, 172, 128, GxEPD_WHITE);

  cpu = map(atoi(msi_info_str.cpu), 0, 100, 0, 118);
  gpu = map(atoi(msi_info_str.gpu), 0, 100, 0, 118);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);

  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(123, 35);
  u8g2_for_adafruit_gfx.print("CPU");
  u8g2_for_adafruit_gfx.setCursor(215, 35);
  u8g2_for_adafruit_gfx.print(msi_info_str.cpu);

  display.fillRect(123, 38, 124, 19, GxEPD_BLACK);
  display.fillRect(125, 40, 120, 15, GxEPD_WHITE);
  display.fillRect(126, 41, cpu, 13, GxEPD_BLACK);

  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(123, 85);
  u8g2_for_adafruit_gfx.print("GPU");
  u8g2_for_adafruit_gfx.setCursor(215, 85);
  u8g2_for_adafruit_gfx.print(msi_info_str.gpu);

  display.fillRect(123, 88, 124, 19, GxEPD_BLACK);
  display.fillRect(125, 90, 120, 15, GxEPD_WHITE);
  display.fillRect(126, 91, gpu, 13, GxEPD_BLACK);


  display.updateWindow(99, 0, 196, 128, true);
  //display.updateWindow(101, 0, 170, 127, true);
}

void showBattary() {

  if (Battary_level > 33)Battary_level = 33;
  else if (Battary_level < 0)Battary_level = 0;
  display.fillRect(4, 46, 39, 13, GxEPD_WHITE);
  display.fillRect(43, 49, 3, 8, GxEPD_WHITE);
  display.fillRect(6, 48, 35, 9, GxEPD_BLACK);
  //display.fillRect(7,49,33,7,GxEPD_WHITE);
  display.fillRect(7, 49, Battary_level, 7, GxEPD_WHITE);
  //display.updateWindow(7,49,33,7);
}

void showBme680() {
  static int i = 0;
  if (i >= BME680_FLASHTIME) {
    i = 0;
    flashBme();
  }
  //    display.fillRect(0, 0, 49, 63, GxEPD_BLACK);
  //    display.setCursor(2, 35);
  //    display.setTextColor(GxEPD_WHITE);
  //    display.setFont(fonts[0]);
  //    display.setTextSize(1);
  //    display.print(bme680_str.temperature);

  display.fillRect(0, 0, 49, 63, GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB24_tn);
  u8g2_for_adafruit_gfx.setCursor(4, 35);
  u8g2_for_adafruit_gfx.print(bme680_str.temperature / 10);

  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_7x14B_tf);
  u8g2_for_adafruit_gfx.setCursor(41, 34);
  u8g2_for_adafruit_gfx.print(bme680_str.temperature % 10);



  display.fillRect(51, 0, 49, 20, GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  u8g2_for_adafruit_gfx.setCursor(73, 17);
  display.drawBitmap(52, 1, p_icon, 18, 18, GxEPD_WHITE);
  u8g2_for_adafruit_gfx.print(bme680_str.pressure);

  display.fillRect(51, 22, 49, 20, GxEPD_BLACK);
  display.drawBitmap(52, 23, h_icon, 18, 18, GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  //u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  u8g2_for_adafruit_gfx.setCursor(73, 38);
  u8g2_for_adafruit_gfx.print(String(bme680_str.humidity) + "%");

  display.fillRect(51, 44, 49, 19, GxEPD_BLACK);
  display.drawBitmap(52, 45, a_icon, 18, 18, GxEPD_WHITE);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  //u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  u8g2_for_adafruit_gfx.setCursor(73, 60);
  u8g2_for_adafruit_gfx.print(bme680_str.iaq);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

  showBattary();

  display.updateWindow(0, 0, 100, 64);
  i++;
}

void flashBme() {
  display.fillRect(0, 0, 49, 63, GxEPD_BLACK);
  display.fillRect(51, 0, 49, 20, GxEPD_BLACK);
  display.fillRect(51, 22, 49, 20, GxEPD_BLACK);
  display.fillRect(51, 44, 49, 19, GxEPD_BLACK);
  display.updateWindow(0, 0, 100, 64);
  display.fillRect(0, 0, 49, 63, GxEPD_WHITE);
  display.fillRect(51, 0, 49, 20, GxEPD_WHITE);
  display.fillRect(51, 22, 49, 20, GxEPD_WHITE);
  display.fillRect(51, 44, 49, 19, GxEPD_WHITE);
  display.updateWindow(0, 0, 100, 64);
  //showBattary();
}

void flashWeather() {
  display.fillRect(0, 65, 100, 31, GxEPD_BLACK);
  display.fillRect(0, 98, 100, 30, GxEPD_BLACK);
  display.updateWindow(0, 65, 100, 62, true);
  display.fillRect(0, 65, 100, 31, GxEPD_WHITE);
  display.fillRect(0, 98, 100, 30, GxEPD_WHITE);
  display.updateWindow(0, 65, 100, 62, true);
}

void showMianPage(void)
{
  static int i = 0;
  displayInit();
  display.fillScreen(GxEPD_WHITE);

  //display.update();

  display.updateWindow(0, 0, 295, 128, true);

  //    display.fillRect(0, 0, 49, 63, GxEPD_BLACK);
  //    display.setCursor(2, 35);
  //    display.setTextColor(GxEPD_WHITE);
  //    display.setFont(fonts[0]);
  //    display.setTextSize(1);
  //    display.print("27");
  //
  //    display.fillRect(4,46,39,13,GxEPD_WHITE);
  //    display.fillRect(43, 49, 3, 8, GxEPD_WHITE);
  //    display.fillRect(6,48,35,9,GxEPD_BLACK);
  //    //display.fillRect(7,49,33,7,GxEPD_WHITE);
  //    display.fillRect(7,49,28,7,GxEPD_WHITE);
  //
  //    display.fillRect(51, 0, 49, 20, GxEPD_BLACK);
  //
  //    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  //
  //    u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  //    u8g2_for_adafruit_gfx.setCursor(73, 17);
  //    u8g2_for_adafruit_gfx.print(1019);
  //    display.drawBitmap(52, 1, p_icon, 18, 18,GxEPD_WHITE);
  //
  //
  //    display.fillRect(51, 22, 49, 20, GxEPD_BLACK);
  //
  //    display.drawBitmap(52, 23, h_icon, 18, 18,GxEPD_WHITE);
  //    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  //    u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  //    u8g2_for_adafruit_gfx.setCursor(73, 38);
  //    u8g2_for_adafruit_gfx.print("98%");
  //
  //
  //
  //
  //    display.fillRect(51, 44, 49, 19, GxEPD_BLACK);
  //
  //    display.drawBitmap(52, 45, a_icon, 18, 18,GxEPD_WHITE);
  //
  //    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
  //    u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
  //    u8g2_for_adafruit_gfx.setCursor(73, 60);
  //    u8g2_for_adafruit_gfx.print("98%");
  //
  //    u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  //    u8g2_for_adafruit_gfx.setFontDirection(0);
  //    u8g2_for_adafruit_gfx.setFontMode(1);
  //    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

  switch_page(page_state);


  //display.updateWindow(0, 0, 296, 128, true);

}

uint16_t read16(File &f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void displayInit(void)
{
  static bool isInit = false;
  if (isInit) {
    return;
  }
  isInit = true;
  display.init();
  display.setRotation(3);
  display.eraseDisplay();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&DEFALUT_FONT);
  display.setTextSize(0);
#if SDCARD_ENABLE
  if (SDCARD_SS > 0) {
    display.fillScreen(GxEPD_WHITE);
#if !(TTGO_T5_2_2)
    SPIClass sdSPI(VSPI);
    sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);
    if (!SD.begin(SDCARD_SS, sdSPI))
#else
    if (!SD.begin(SDCARD_SS))
#endif
    {
      displayText("SDCard MOUNT FAIL", 50, CENTER_ALIGNMENT);
    } else {
      displayText("SDCard MOUNT PASS", 50, CENTER_ALIGNMENT);
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      displayText("SDCard Size: " + String(cardSize) + "MB", 70, CENTER_ALIGNMENT);
    }
    display.update();
    delay(2000);
  }
#endif
}

bool setPowerBoostKeepOn(int en)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  else
    Wire.write(0x35); // 0x37 is default reg value
  return Wire.endTransmission() == 0;
}

bool msi_info_mqtt_json(char s[]) {
#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(s);
  if (!root.success()) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
  root.printTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
  DeserializationError error = deserializeJson(root, s);
  if (error) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
#endif
  if ((const char *)root["cpu"] == NULL) {
    return false;
  }
  strlcpy(msi_info_str.cpu, root["cpu"], sizeof(msi_info_str.cpu));
  strlcpy(msi_info_str.gpu, root["gpu"], sizeof(msi_info_str.gpu));

  thirdPage();
  return true;
}

bool weather_mqtt_json(char s[]) {
  char code1[8], code2[8];
  static int i = 0;
#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(s);
  if (!root.success()) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
  root.printTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
  DeserializationError error = deserializeJson(root, s);
  if (error) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
#endif
  if ((const char *)root["code_1"] == NULL) {
    return false;
  }

  strlcpy(code1, wea_str.code_1, sizeof(code1));
  strlcpy(code2, wea_str.code_2, sizeof(code2));
  strlcpy(wea_str.code_1, root["code_1"], sizeof(wea_str.code_1));
  strlcpy(wea_str.code_2, root["code_2"], sizeof(wea_str.code_2));
  strlcpy(wea_str.wea_1, root["wea_1"], sizeof(wea_str.wea_1));
  strlcpy(wea_str.wea_2, root["wea_2"], sizeof(wea_str.wea_2));
  strlcpy(wea_str.tmp_1_min, root["tmp_1_min"], sizeof(wea_str.tmp_1_min));
  strlcpy(wea_str.tmp_1_max, root["tmp_1_max"], sizeof(wea_str.tmp_1_max));
  strlcpy(wea_str.tmp_2_min, root["tmp_2_min"], sizeof(wea_str.tmp_2_min));
  strlcpy(wea_str.tmp_2_max, root["tmp_2_max"], sizeof(wea_str.tmp_2_max));
  //Serial.println(wea_str.code_1);
  if (strcmp(code1, wea_str.code_1) || strcmp(code2, wea_str.code_2)) {
    i = 10;
  }
  if (i >= 10) {
    flashWeather();
    i = 0;
  }
  first_weather();
  second_weather();
  i++;
  return true;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char s[500] = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    sprintf(s, "%s%c", s, (char)payload[i]);
  }
  Serial.println();
  //  Serial.print(s);
  //  Serial.println();
  //String c=Encoding.ASCII.GetChars(payload);
  if (strcmp(topic, "notice") == 0) {
    strlcpy(notice_str.notice_6, notice_str.notice_5, sizeof(notice_str.notice_6));
    strlcpy(notice_str.notice_5, notice_str.notice_4, sizeof(notice_str.notice_5));
    strlcpy(notice_str.notice_4, notice_str.notice_3, sizeof(notice_str.notice_4));
    strlcpy(notice_str.notice_3, notice_str.notice_2, sizeof(notice_str.notice_3));
    strlcpy(notice_str.notice_2, notice_str.notice_1, sizeof(notice_str.notice_2));
    strlcpy(notice_str.notice_1, s, sizeof(notice_str.notice_1));
    saveBadgeInfo(&notice_str);
    Serial.print(notice_str.notice_1);
    if (page_state == 1) {
      firstPage();
    } else if (page_state == 2) {
      secondPage();
    }
    client.publish("notice_back", "already received notice");
  } else if (strcmp(topic, "weather") == 0) {
    weather_mqtt_json(s);
  } else if (strcmp(topic, "msi_info") == 0) {
    if (page_state == 3)
      msi_info_mqtt_json(s);
  } else if (strcmp(topic, "to_temperature") == 0) {
    char s[8];
    itoa(bme680_str.temperature, s, 10);
    client.publish("from_temperature", s);
  } else if (strcmp(topic, "to_humidity") == 0) {
    char s[8];
    itoa(bme680_str.humidity, s, 10);
    client.publish("from_humidity", s);
  } else if (strcmp(topic, "to_pressure") == 0) {
    char s[8];
    itoa(bme680_str.pressure, s, 10);
    client.publish("from_pressure", s);
  } else if (strcmp(topic, "to_iaq") == 0) {
    char s[8];
    itoa(bme680_str.iaq, s, 10);
    client.publish("from_iaq", s);
  }
}

void reconnect() {
  static int failed_time = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      failed_time = 0;
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("debug", "ESP32 connected");
      // ... and resubscribe
      client.subscribe("notice");
      client.subscribe("weather");
      client.subscribe("msi_info");
      client.subscribe("to_temperature");
      client.subscribe("to_humidity");
      client.subscribe("to_pressure");
      client.subscribe("to_iaq");
    } else {
      if (failed_time > 10) {
        esp_restart();
      }
      failed_time++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setTime() {
  struct tm timeinfo;
  static int time_failed = 0;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(200);
    time_failed++;
    if (time_failed > 10) {
      esp_restart();
    }
  }

  // Sync
  // while(timeinfo.tm_sec!=0){
  //   Serial.println(timeinfo.tm_sec);
  //   getLocalTime(&timeinfo);
  // }

  // Start an alarm
  //timerAlarmEnable(timer);
}

void setWeather() {
  //client.publish("weather_call", "call for weather info");
  // Start an alarm
  timerAlarmEnable(timer2);
}

void IRAM_ATTR onTimer() {
  Serial.println("timer");
  timer_1 = 1;
}

void IRAM_ATTR onTimer2() {
  static int weather = 0, battery = 0, bme680_t = 0;
  Serial.print("timer:");
  Serial.print(weather);
  Serial.print(" ");
  Serial.print(battery);
  Serial.print(" ");
  Serial.println(bme680_t);
  if (weather == 120) {
    timer_2_weather = 1;
    weather = 0;
  }
  if (battery == 5) {
    timer_2_battery = 1;
    battery = 0;
  }
  if (bme680_t == 1) {
    timer_2_bme680 = 1;
    bme680_t = 0;
  }
  timer_1 = 1;
  weather++;
  battery++;
  bme680_t++;
}

void timerRec() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  if ((int)timeinfo.tm_hour == SLEEPTIME_h && (int)timeinfo.tm_min == SLEEPTIME_m) {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Going to sleep now");
    Serial.flush();
    //Wire.end();
    display.powerDown();
    esp_deep_sleep_start();
  }
  if (page_state == 1) {
    LCD_print(timeinfo);
    display.updateWindow(114, 16, 141, 59, true);
  }
}

void GetGasReference() {
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  //Serial.println("Getting a new gas reference value");
  int readings = 10;
  for (int i = 1; i <= readings; i++) { // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
  //Serial.println("Gas Reference = "+String(gas_reference,3));
}

int CalculateIAQ(int score) {
  String IAQ_text = "air quality is ";
  score = (100 - score) * 5;
  if      (score >= 301)                  IAQ_text += "Hazardous";
  else if (score >= 201 && score <= 300 ) IAQ_text += "Very Unhealthy";
  else if (score >= 176 && score <= 200 ) IAQ_text += "Unhealthy";
  else if (score >= 151 && score <= 175 ) IAQ_text += "Unhealthy for Sensitive Groups";
  else if (score >=  51 && score <= 150 ) IAQ_text += "Moderate";
  else if (score >=  00 && score <=  50 ) IAQ_text += "Good";
  Serial.println(IAQ_text);
  return score;
}

int GetHumidityScore() {  //Calculate humidity contribution to IAQ index
  float current_humidity = bme.readHumidity() + 5.0;
  bme680_str.humidity = current_humidity;
  if (current_humidity >= 38 && current_humidity <= 42) // Humidity +/-5% around optimum
    humidity_score = 0.25 * 100;
  else
  { // Humidity is sub-optimal
    if (current_humidity < 38)
      humidity_score = 0.25 / hum_reference * current_humidity * 100;
    else
    {
      humidity_score = ((-0.25 / (100 - hum_reference) * current_humidity) + 0.416666) * 100;
    }
  }
  return humidity_score;
}

int GetGasScore() {
  //Calculate gas contribution to IAQ index
  gas_score = (0.75 / (gas_upper_limit - gas_lower_limit) * gas_reference - (gas_lower_limit * (0.75 / (gas_upper_limit - gas_lower_limit)))) * 100.00;
  if (gas_score > 75) gas_score = 75; // Sometimes gas readings can go outside of expected scale maximum
  if (gas_score <  0) gas_score = 0;  // Sometimes gas readings can go outside of expected scale minimum
  return gas_score;
}

bool bme680_read() {
  bme680_str.pressure = bme.readPressure() / 100.0F;
  bme680_str.temperature = (bme.readTemperature() * 10) - 15;
  Serial.print("temperature:");
  Serial.println(bme680_str.temperature);
  Serial.print("pressure:");
  Serial.println(bme680_str.pressure);
  humidity_score = GetHumidityScore();
  gas_score      = GetGasScore();

  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  float air_quality_score = humidity_score + gas_score;
  Serial.println("comprised of " + String(humidity_score) + "% Humidity and " + String(gas_score) + "% Gas");
  if ((getgasreference_count++) % 5 == 0) GetGasReference();
  bme680_str.iaq = CalculateIAQ(air_quality_score);
  return true;
}

void showVoltage()
{
  static uint64_t timeStamp = 0;
  if (millis() - timeStamp > 1000) {
    timeStamp = millis();
    uint16_t v = analogRead(ADC_PIN);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    String voltage = "Voltage :" + String(battery_voltage) + "V";
    Serial.println(voltage);
    if (battery_voltage < 3.2) {
      display.fillScreen(GxEPD_WHITE);
      display.update();
      display.drawBitmap(100, 18, battery2, 100, 100, GxEPD_BLACK);
      display.drawBitmap(20, 88, usb, 40, 40, GxEPD_BLACK);
      display.update();
      delay(1000);
      Serial.println("Going to sleep now");
      Serial.flush();
      //Wire.end();
      display.powerDown();
      esp_deep_sleep_start();
    }
    Battary_level = map(battery_voltage * 100, 330, 420, 0, 33);
    //showBattary();
    Serial.print("map:");
    Serial.println(Battary_level);
    //        tft.fillScreen(TFT_BLACK);
    //        tft.setTextDatum(MC_DATUM);
    //        tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
  }
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  if (bootCount == 2) {
    esp_restart();
  }

  Serial.print("CPU Frequency:");
  Serial.println(getCpuFrequencyMhz());


  WebServerStart();


  //    adcAttachPin(35);
  //    analogSetAttenuation(ADC_11db);
  //    analogSetSamples(10);

  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
  //Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    Serial.println();
    vref = adc_chars.vref;
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    Serial.println();
  } else {
    Serial.println("Default Vref: 1100mV");
  }

  if (!client.connected()) {
    reconnect();
  }

  u8g2_for_adafruit_gfx.begin(display);
  //u8g2_for_adafruit_gfx.setFont(u8g2_font_prospero_bold_nbp_tn);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //    // Create semaphore to inform us when the timer has fired
  //    timerSemaphore = xSemaphoreCreateBinary();
  //    // Use 1st timer of 4 (counted from zero).
  //    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  //    // info).
  //    timer = timerBegin(3, 80, true);
  //    // Attach onTimer function to our timer.
  //    timerAttachInterrupt(timer, &onTimer, true);
  //    // Set alarm to call onTimer function every second (value in microseconds).
  //    // Repeat the alarm (third parameter)
  //    timerAlarmWrite(timer, 60000000, true);

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer2 = timerBegin(2, 80, true);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer2, &onTimer2, true);
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer2, 60000000, true);

#ifdef ENABLE_IP5306
  Wire.begin(I2C_SDA, I2C_SCL);
  bool ret = setPowerBoostKeepOn(1);
  Serial.printf("Power KeepUp %s\n", ret ? "PASS" : "FAIL");
#endif

  // It is only necessary to turn on the power amplifier power supply on the T5_V24 board.
#ifdef AMP_POWER_CTRL
  pinMode(AMP_POWER_CTRL, OUTPUT);
  digitalWrite(AMP_POWER_CTRL, HIGH);
#endif

#ifdef DAC_MAX98357
  AudioGeneratorMP3 *mp3;
  AudioFileSourcePROGMEM *file;
  AudioOutputI2S *out;
  AudioFileSourceID3 *id3;

  file = new AudioFileSourcePROGMEM(image, sizeof(image));
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S();
  out->SetPinout(IIS_BCK, IIS_WS, IIS_DOUT);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (1) {
    if (mp3->isRunning()) {
      if (!mp3->loop())
        mp3->stop();
    } else {
      Serial.printf("MP3 done\n");
      break;
    }
  }
#endif

  if (SPEAKER_OUT > 0) {
    ledcSetup(CHANNEL_0, 1000, 8);
    ledcAttachPin(SPEAKER_OUT, CHANNEL_0);
    int i = 3;
    while (i--) {
      //ledcWriteTone(CHANNEL_0, 1000);
      delay(200);
      ledcWriteTone(CHANNEL_0, 0);
    }
  }
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, -1);
  if (!FILESYSTEM.begin()) {
    Serial.println("FILESYSTEM is not database");
    Serial.println("Please use Arduino ESP32 Sketch data Upload files");
    while (1) {
      delay(1000);
    }
  }
  if (!loadBadgeInfo(&notice_str)) {
    loadDefaultInfo();
  }

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
    showMianPage();
  }


  setTime();
  //delay(5000);
  setWeather();


  while (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    delay(1000);
    bme680_errTime++;
    if (bme680_errTime == 10) {
      //esp_restart();
      display.fillScreen(GxEPD_WHITE);
      display.update();
      u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
      u8g2_for_adafruit_gfx.setBackgroundColor(GxEPD_WHITE);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_calibration_gothic_nbp_tf);
      u8g2_for_adafruit_gfx.setCursor(75, 55);
      u8g2_for_adafruit_gfx.print("An error occurred.");
      u8g2_for_adafruit_gfx.setCursor(75, 85);
      u8g2_for_adafruit_gfx.print("Please restart the system.");
      display.drawBitmap(13, 38, oops, 50, 50, GxEPD_BLACK);

      //display.drawBitmap(100, 18, battery2, 100, 100, GxEPD_BLACK);
      //        display.drawBitmap(20, 88, usb, 40, 40, GxEPD_BLACK);


      display.update();
      delay(1000);
      Serial.println("Going to sleep now");
      Serial.flush();
      //Wire.end();
      display.powerDown();
      esp_deep_sleep_start();
    }
  }


  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  GetGasReference();
  bme680_errTime = 0;

  button_init();


  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
  setCpuFrequencyMhz(80);
  Serial.print("CPU Frequency:");
  Serial.println(getCpuFrequencyMhz());
}

void loop()
{
  static int battery_adc = 0;

  button_loop();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (timer_1 == 1) {
    timer_1 = 0;
    timerRec();
  }

  if (timer_2_weather == 1) {
    timer_2_weather = 0;
    client.publish("weather_call", "call for weather info");
  }

  if (timer_2_battery == 1) {
    showVoltage();
    timer_2_battery = 0;
  }

  if (msi_info_state == -1) {
    ;
  } else if (msi_info_state == 1) {
    client.publish("msi_info_call", "on");
    msi_info_state = -1;
  } else if (msi_info_state == 0) {
    client.publish("msi_info_call", "off");
    msi_info_state = -1;
  }

  if (timer_2_bme680 == 1) {
    bme680_read();
    showBme680();
    timer_2_bme680 = 0;
  }

}
