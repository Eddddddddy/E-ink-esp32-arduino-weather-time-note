// include library, include base class, make path known
#include "SD.h"
#include "SPI.h"
#include <FS.h>
#include <ArduinoJson.h>
#include "Esp.h"

#include "config.h"
#include "func.h"

#include "button_func.h"

#include "notice_func.h"

#include "page_func.h"

#include "battery_func.h"

#include "bme_func.h"

#include "mqtt_func.h"

#include "timer_func.h"

#include "esp_power_func.h"

#include "ink_func.h"

#include "ntp_func.h"

#include "other_func.h"

#include "wifi_func.h"

void setup()
{
  Serial.begin(115200);
  delay(500);

  boot_init();

  Serial.print("CPU Frequency:");
  Serial.println(getCpuFrequencyMhz());

  WebServerStart();

  batteryValue_init();

  if (!client.connected()) {
    reconnect();
  }

  u8g2Font_init();

  ntp_init();

  timer_init();

  other_init();

  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, -1);

  notice_init();

  ink_init();

  timer_begin();

  bme_init();

  button_init();

  powerSave();
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
