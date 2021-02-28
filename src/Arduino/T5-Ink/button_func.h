
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
