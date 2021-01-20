//sleep time. when time is SLEEPTIME_h:SLEEPTIME_m, device will sleep TIME_TO_SLEEP seconds.
#define TIME_TO_SLEEP  27000
#define SLEEPTIME_h 23
#define SLEEPTIME_m 30

//the part of bme680 on the eink. when sensor read BME680_FLASHTIME times, eink will flash once.
#define BME680_FLASHTIME 20

//wifi ssid and password for NO.1
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

//wifi ssid and password for NO.2
#define WIFI_SSID2 "ssid"
#define WIFI_PASSWORD2 "password"

//wifi ssid and password for NO.3
#define WIFI_SSID3 "ssid"
#define WIFI_PASSWORD3 "password"

//MQTT server IP and port
#define MQTT_SERVER "IP"
#define MQTT_PORT 1883

//set clock timezone. like 8 for +8. and -5 for -5 zone.
#define TIMEZONE 8

//some config modes for debug
//#define USE_AP_MODE
//#define WEBSERVER
//#define SDCARD_ENABLE
#define BME_ENABLE
