

void showBattery() {
  if (Battery_level > 33)Battery_level = 33;
  else if (Battery_level < 0)Battery_level = 0;
  display.fillRect(4, 46 - 8, 39, 13, GxEPD_WHITE);
  display.fillRect(43, 49 - 8, 3, 8, GxEPD_WHITE);
  display.fillRect(6, 48 - 8, 35, 9, GxEPD_BLACK);
  //display.fillRect(7,49,33,7,GxEPD_WHITE);
  display.fillRect(7, 49 - 8, Battery_level, 7, GxEPD_WHITE);
  //display.updateWindow(7,49,33,7);
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
      display.fillScreen(GxEPD_BLACK);
      display.update();
      delay(1000);
      display.update();
      delay(1000);
      display.update();
      delay(1000);
      display.fillScreen(GxEPD_WHITE);
      display.update();
      delay(1000);
      display.update();
      delay(1000);
      display.update();
      delay(1000);
      display.drawBitmap(85, 15, battery2, 100, 100, GxEPD_BLACK);
      display.drawBitmap(30, 82, usb, 40, 40, GxEPD_BLACK);
      display.update();
      delay(1000);
      display.update();
      delay(1000);
      Serial.println("Going to sleep now");
      Serial.flush();
      //Wire.end();
      display.powerDown();
      esp_deep_sleep_start();
    }
    Battery_level = map(battery_voltage * 100, 330, 420, 0, 33);
    //showBattery();
    Serial.print("map:");
    Serial.println(Battery_level);
    //        tft.fillScreen(TFT_BLACK);
    //        tft.setTextDatum(MC_DATUM);
    //        tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
  }
}

void batteryValue_init() {

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
}
