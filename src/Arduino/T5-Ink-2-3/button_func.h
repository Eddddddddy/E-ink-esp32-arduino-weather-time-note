
void button_handle(uint8_t gpio)
{
    switch (gpio) {

#if BUTTON_3
        case BUTTON_3: {
        //static int i = 0;
        page_state = ((page_state + 1) > 2) ? 1 : page_state + 1;
        Serial.printf("Show Num: %d page\n", page_state);

        if (page_state == 2) {
          //client.publish("msi_info_call", "on");
          //msi_info_state=1;
        } else if (page_state == 1) {
          //client.publish("msi_info_call", "off");
          //msi_info_state=0;

          //            display.fillRect(99, 0, 150, 122, GxEPD_BLACK);
          //            display.updateWindow(99, 0, 150, 122, true);
          //            display.fillRect(99, 0, 150, 122, GxEPD_WHITE);
          //            display.updateWindow(99, 0, 150, 122, true);

          //            display.fillRect(101, 0, 170, 127, GxEPD_BLACK);
          //            display.updateWindow(101, 0, 170, 127, true);
        }
        switch_page(page_state);
        //display.powerDown();
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
