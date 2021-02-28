


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
