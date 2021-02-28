
void IRAM_ATTR onTimer() {
  Serial.println("timer");
  timer_1 = 1;
}

void IRAM_ATTR onTimer2() {
  static int weather = 0, battery = 0, bme680_t = 0;
  int weather_flush_time;
if (weather_first == 0) {
    weather_flush_time = 1;
} else {
    weather_flush_time = 120;
}
  Serial.print("timer:");
  Serial.print(weather);
  Serial.print(" ");
  Serial.print(battery);
  Serial.print(" ");
  Serial.println(bme680_t);
  if (weather == weather_flush_time) {
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
    display.updateWindow(114 - 9, 16, 141, 59, true);
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

void timer_init() {
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
}

void timer_begin() {
  setTime();
  //delay(5000);
  setWeather();
}
