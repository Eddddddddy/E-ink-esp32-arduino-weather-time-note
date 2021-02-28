
typedef struct {
  int humidity;
  int pressure;
  int temperature;
  int iaq;
} bme680_struct;

bme680_struct bme680_str;

Adafruit_BME680 bme; // I2C

//bme680
#define SEALEVELPRESSURE_HPA (1002.2)
float hum_weighting = 0.25; // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75; // so gas effect is 75% of the total air quality score
int   humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int   getgasreference_count = 0;
int   gas_lower_limit = 10000;  // Bad air quality limit
int   gas_upper_limit = 300000; // Good air quality limit
int   bme680_errTime = 0;

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

  showBattery();

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
  //showBattery();
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

void bme_init() {
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
}
