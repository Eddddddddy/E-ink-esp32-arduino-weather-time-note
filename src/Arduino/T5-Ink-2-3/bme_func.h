
void showBme680() {
    static int i = 0;
    if (i >= BME680_FLASHTIME) {
        i = 0;
        //flashBme();
    }
#ifndef BME_ENABLE
    bme680_str.temperature = 246;
    bme680_str.pressure = 1013;
    bme680_str.humidity = 50;
    bme680_str.iaq = 100;
#endif

    //    display.fillRect(0, 0, 49, 63-8, GxEPD_BLACK);
    //    display.setCursor(2, 32);
    //    display.setTextColor(GxEPD_WHITE);
    //    display.setFont(fonts[0]);
    //    display.setTextSize(1);
    //    display.print(bme680_str.temperature);

    display.fillRect(0, 0, 49, 63 - 8, GxEPD_BLACK);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB24_tn);
    u8g2_for_adafruit_gfx.setCursor(4, 32);
    u8g2_for_adafruit_gfx.print(bme680_str.temperature / 10);

    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_7x14B_tf);
    u8g2_for_adafruit_gfx.setCursor(41, 31);
    u8g2_for_adafruit_gfx.print(bme680_str.temperature % 10);

    display.fillRect(52, 0, 48, 26, GxEPD_BLACK);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
    u8g2_for_adafruit_gfx.setCursor(74, 20);
    display.drawBitmap(53, 4, p_icon, 18, 18, GxEPD_WHITE);
    u8g2_for_adafruit_gfx.print(bme680_str.pressure);

    display.fillRect(52, 29, 48, 26, GxEPD_BLACK);
    display.drawBitmap(53, 33, h_icon, 18, 18, GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
    u8g2_for_adafruit_gfx.setCursor(74, 48);
    u8g2_for_adafruit_gfx.print(String(bme680_str.humidity) + "%");

    //    display.fillRect(51, 44, 49, 19, GxEPD_BLACK);
    //    display.drawBitmap(52, 45, a_icon, 18, 18,GxEPD_WHITE);
    //    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    //    //u8g2_for_adafruit_gfx.setFont(u8g2_font_lastapprenticebold_tr);
    //    u8g2_for_adafruit_gfx.setCursor(73, 60);
    //    u8g2_for_adafruit_gfx.print(bme680_str.iaq);

    u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
    u8g2_for_adafruit_gfx.setFontDirection(0);
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

    showBattery();

    display.updateWindow(0, 0, 100, 63 - 7);
    i++;
}

void flashBme() {
    display.fillRect(0, 0, 49, 63 - 8, GxEPD_BLACK);
    display.fillRect(52, 0, 48, 26, GxEPD_BLACK);
    display.fillRect(52, 29, 48, 26, GxEPD_BLACK);
    display.updateWindow(0, 0, 100, 64);
    display.fillRect(0, 0, 49, 63 - 8, GxEPD_WHITE);
    display.fillRect(52, 0, 48, 26, GxEPD_WHITE);
    display.fillRect(52, 29, 48, 26, GxEPD_WHITE);
    display.updateWindow(0, 0, 100, 64);
    //showBattery();
}

bool bme680_read() {

    bme680_str.temperature = bme.readTemperature() * 10;
    bme680_str.humidity = bme.readHumidity();
    bme680_str.pressure = bme.readPressure() / 100.0F;

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    return true;
}

void bme_init() {
  #ifdef BME_ENABLE
      unsigned status;
    // default settings
    //status = bme.begin();
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    while (!bme.begin()) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      delay(1000);
      bme680_errTime++;
      if (bme680_errTime == 10) {
        //esp_restart();
        display.fillScreen(GxEPD_BLACK);
        display.updateWindow(0, 0, 296, 128, true);
        delay(1000);
        display.update();
        delay(1000);
        display.fillScreen(GxEPD_WHITE);
        display.update();
        delay(1000);
        display.update();
        delay(1000);
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
    bme680_errTime = 0;
  #endif
}
