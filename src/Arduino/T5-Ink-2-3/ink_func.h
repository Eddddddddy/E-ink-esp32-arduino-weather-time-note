
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


void first_weather() {
    int low_l, low_h, high_l, high_h;
    low_l = abs(atoi(wea_str.tmp_1_min) % 10);
    low_h = abs(atoi(wea_str.tmp_1_min) / 10);
    high_l = abs(atoi(wea_str.tmp_1_max) % 10);
    high_h = abs(atoi(wea_str.tmp_1_max) / 10);

    display.fillRect(0, 65 - 7, 100, 31, GxEPD_BLACK);

    u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
    u8g2_for_adafruit_gfx.setFontDirection(0);
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

    display.drawBitmap(3, 68 - 7, weather_icon[atoi(wea_str.code_1)], 25, 25, GxEPD_WHITE); //weather_icon
    //display.drawBitmap(33, 68, weather_text[num_code], 24, 12,GxEPD_WHITE);//天气
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setCursor(33, 80 - 7);
    u8g2_for_adafruit_gfx.print(wea_str.wea_1);
    if (atoi(wea_str.tmp_1_min) > 0) {
        display.drawBitmap(30, 80 - 7, num[low_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 8, 80 - 7, num[low_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_1_min) < -9) {
        display.drawBitmap(30, 80 - 7, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 8, 80 - 7, num[low_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 16, 80 - 7, num[low_l], 8, 16, GxEPD_WHITE); //个位
    } else if (atoi(wea_str.tmp_1_min) < 0) {
        display.drawBitmap(30, 80 - 7, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 8, 80 - 7, num[low_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_1_min) == 0) {
        display.drawBitmap(30 + 8, 80 - 7, num[0], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    }

    display.drawBitmap(30 + 24, 80 - 7, num_, 16, 16, GxEPD_WHITE); //--

    if (atoi(wea_str.tmp_1_max) > 0) {
        display.drawBitmap(30 + 40, 80 - 7, num[high_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 48, 80 - 7, num[high_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_1_max) < -9) {
        display.drawBitmap(30 + 40, 80 - 7, num[11], 8, 16, GxEPD_WHITE); //--
        display.drawBitmap(30 + 48, 80 - 7, num[high_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 56, 80 - 7, num[high_l], 8, 16, GxEPD_WHITE); //个位
    } else if (atoi(wea_str.tmp_1_max) < 0) {
        display.drawBitmap(30 + 40, 80 - 7, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 48, 80 - 7, num[high_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_1_max) == 0) {
        display.drawBitmap(30 + 48, 80 - 7, num[0], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 - 7, num[10], 8, 16, GxEPD_WHITE); //℃
    }


    display.updateWindow(0, 65 - 7, 100, 31, true);
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

    display.fillRect(0, 98 - 6, 100, 30, GxEPD_BLACK);
    display.drawBitmap(3, 68 + 32 - 6, weather_icon[atoi(wea_str.code_2)], 25, 25, GxEPD_WHITE); //weather_icon
    //display.drawBitmap(33, 68+32, weather_text[num_code], 24, 12,GxEPD_WHITE);//天气
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setCursor(33, 80 + 32 - 6);
    u8g2_for_adafruit_gfx.print(wea_str.wea_2);
    if (atoi(wea_str.tmp_2_min) > 0) {
        display.drawBitmap(30, 80 + 32 - 6, num[low_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 8, 80 + 32 - 6, num[low_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_2_min) < -9) {
        display.drawBitmap(30, 80 + 32 - 6, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 8, 80 + 32 - 6, num[low_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 16, 80 + 32 - 6, num[low_l], 8, 16, GxEPD_WHITE); //个位
    } else if (atoi(wea_str.tmp_2_min) < 0) {
        display.drawBitmap(30, 80 + 32 - 6, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 8, 80 + 32 - 6, num[low_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_2_min) == 0) {
        display.drawBitmap(30 + 8, 80 + 32 - 6, num[0], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 16, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    }

    display.drawBitmap(30 + 24, 80 + 32 - 6, num_, 16, 16, GxEPD_WHITE); //--

    if (atoi(wea_str.tmp_2_max) > 0) {
        display.drawBitmap(30 + 40, 80 + 32 - 6, num[high_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 48, 80 + 32 - 6, num[high_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_2_max) < -9) {
        display.drawBitmap(30 + 40, 80 + 32 - 6, num[11], 8, 16, GxEPD_WHITE); //--
        display.drawBitmap(30 + 48, 80 + 32 - 6, num[high_h], 8, 16, GxEPD_WHITE); //十位
        display.drawBitmap(30 + 56, 80 + 32 - 6, num[high_l], 8, 16, GxEPD_WHITE); //个位
    } else if (atoi(wea_str.tmp_2_max) < 0) {
        display.drawBitmap(30 + 40, 80 + 32 - 6, num[11], 8, 16, GxEPD_WHITE); //-
        display.drawBitmap(30 + 48, 80 + 32 - 6, num[high_l], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    } else if (atoi(wea_str.tmp_2_max) == 0) {
        display.drawBitmap(30 + 48, 80 + 32 - 6, num[0], 8, 16, GxEPD_WHITE); //个位
        display.drawBitmap(30 + 56, 80 + 32 - 6, num[10], 8, 16, GxEPD_WHITE); //℃
    }


    display.updateWindow(0, 98 - 6, 100, 29, true);
    display.updateWindow(0, 98 - 6, 100, 29, true);
    //delay(1000);
    //display.powerDown();
}

void LCD_one(int num, int loc) {
    uint16_t show_1, show_2, show_3, show_4, show_5, show_6, show_7;
    int x;
    switch (loc) {
        case 1: x = 0 - 9; break;
        case 2: x = 34 - 9; break;
        case 3: x = 77 - 9; break;
        case 4: x = 111 - 9; break;
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
  display.drawBitmap(183 - 9, 31, LCD_d, 5, 5, GxEPD_BLACK); //数码管
  display.drawBitmap(183 - 9, 55, LCD_d, 5, 5, GxEPD_BLACK); //数码管
}


void flashWeather() {
    display.fillRect(0, 65 - 7, 100, 31, GxEPD_BLACK);
    display.fillRect(0, 98 - 6, 100, 30, GxEPD_BLACK);
    display.updateWindow(0, 65 - 6, 100, 62, true);
    display.fillRect(0, 65 - 7, 100, 31, GxEPD_WHITE);
    display.fillRect(0, 98 - 6, 100, 30, GxEPD_WHITE);
    display.updateWindow(0, 65 - 6, 100, 62, true);
}

void showMianPage(void)
{
  static int i = 0;
  displayInit();
  display.fillScreen(GxEPD_WHITE);

  //display.update();

  display.updateWindow(0, 0, 249, 122, true);

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

//uint16_t read16(File &f)
//{
//  // BMP data is stored little-endian, same as Arduino.
//  uint16_t result;
//  ((uint8_t *)&result)[0] = f.read(); // LSB
//  ((uint8_t *)&result)[1] = f.read(); // MSB
//  return result;
//}
//
//uint32_t read32(File &f)
//{
//  // BMP data is stored little-endian, same as Arduino.
//  uint32_t result;
//  ((uint8_t *)&result)[0] = f.read(); // LSB
//  ((uint8_t *)&result)[1] = f.read();
//  ((uint8_t *)&result)[2] = f.read();
//  ((uint8_t *)&result)[3] = f.read(); // MSB
//  return result;
//}

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

void u8g2Font_init() {
  u8g2_for_adafruit_gfx.begin(display);
  //u8g2_for_adafruit_gfx.setFont(u8g2_font_prospero_bold_nbp_tn);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy14_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
}

void ink_init() {
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
    showMianPage();
  }
}
