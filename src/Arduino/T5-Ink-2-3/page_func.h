


void switch_page (int page) {
    switch (page) {
        case 1: {
            //      display.fillRect(271, 0, 26, 42, GxEPD_WHITE);
            //      display.drawBitmap(272, 10, home_icon, 23, 23,GxEPD_BLACK);
            //      display.fillRect(271, 44, 26, 42, GxEPD_BLACK);
            //      display.drawBitmap(273, 54, txt_icon, 23, 23,GxEPD_WHITE);
            //      display.fillRect(271, 88, 26, 44, GxEPD_BLACK);
            //      display.drawBitmap(272, 97, inf_icon, 23, 23,GxEPD_WHITE);
            firstPage();
        } break;
        case 2: {
            //      display.fillRect(271, 0, 26, 42, GxEPD_BLACK);
            //      display.drawBitmap(272, 10, home_icon, 23, 23,GxEPD_WHITE);
            //      display.fillRect(271, 44, 26, 42, GxEPD_WHITE);
            //      display.drawBitmap(273, 54, txt_icon, 23, 23,GxEPD_BLACK);
            //      display.fillRect(271, 88, 26, 44, GxEPD_BLACK);
            //      display.drawBitmap(272, 97, inf_icon, 23, 23,GxEPD_WHITE);
            secondPage();
        } break;
        case 3: {
            //      display.fillRect(271, 0, 26, 42, GxEPD_BLACK);
            //      display.drawBitmap(272, 10, home_icon, 23, 23,GxEPD_WHITE);
            //      display.fillRect(271, 44, 26, 42, GxEPD_BLACK);
            //      display.drawBitmap(273, 54, txt_icon, 23, 23,GxEPD_WHITE);
            //      display.fillRect(271, 88, 26, 44, GxEPD_WHITE);
            //      display.drawBitmap(272, 97, inf_icon, 23, 23,GxEPD_BLACK);
            //thirdPage();
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
  u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2_for_adafruit_gfx.setFontDirection(0);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);
  u8g2_for_adafruit_gfx.setCursor(111, 105);
  u8g2_for_adafruit_gfx.print(notice_str.notice_1);
  u8g2_for_adafruit_gfx.setCursor(112, 105);
  u8g2_for_adafruit_gfx.print(notice_str.notice_1);

  display.updateWindow(99, 0, 150, 122, true);//196 127
  display.updateWindow(99, 0, 150, 122, true);//196 127
  display.updateWindow(99, 0, 150, 122, true);//196 127
  //display.updateWindow(101, 0, 170, 127, true);
}

void secondPage() {



    display.fillRect(99, 0, 172, 128, GxEPD_WHITE);
    display.fillRect(111, 22, 130, 2, GxEPD_BLACK);
    display.fillRect(111, 47, 130, 2, GxEPD_BLACK);
    display.fillRect(111, 72, 130, 2, GxEPD_BLACK);
    display.fillRect(111, 98, 130, 2, GxEPD_BLACK);
    //display.fillRect(111, 107, 150, 1, GxEPD_BLACK);

    u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy16_t_gb2312);
    u8g2_for_adafruit_gfx.setFontDirection(0);
    u8g2_for_adafruit_gfx.setFontMode(1);

    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

    u8g2_for_adafruit_gfx.setCursor(111, 18);
    u8g2_for_adafruit_gfx.print(notice_str.notice_1);
    u8g2_for_adafruit_gfx.setCursor(112, 18);
    u8g2_for_adafruit_gfx.print(notice_str.notice_1);

    u8g2_for_adafruit_gfx.setCursor(111, 42);
    u8g2_for_adafruit_gfx.print(notice_str.notice_2);
    u8g2_for_adafruit_gfx.setCursor(112, 42);
    u8g2_for_adafruit_gfx.print(notice_str.notice_2);

    u8g2_for_adafruit_gfx.setCursor(111, 67);
    u8g2_for_adafruit_gfx.print(notice_str.notice_3);
    u8g2_for_adafruit_gfx.setCursor(112, 67);
    u8g2_for_adafruit_gfx.print(notice_str.notice_3);

    u8g2_for_adafruit_gfx.setCursor(111, 92);
    u8g2_for_adafruit_gfx.print(notice_str.notice_4);
    u8g2_for_adafruit_gfx.setCursor(112, 92);
    u8g2_for_adafruit_gfx.print(notice_str.notice_4);

    u8g2_for_adafruit_gfx.setCursor(111, 118);
    u8g2_for_adafruit_gfx.print(notice_str.notice_5);
    u8g2_for_adafruit_gfx.setCursor(112, 118);
    u8g2_for_adafruit_gfx.print(notice_str.notice_5);

    //  u8g2_for_adafruit_gfx.setCursor(111, 124);
    //  u8g2_for_adafruit_gfx.print(notice_str.notice_6);

    display.updateWindow(99, 0, 150, 122, true);
    display.updateWindow(99, 0, 150, 122, true);
    //display.updateWindow(101, 0, 170, 127, true);
}
