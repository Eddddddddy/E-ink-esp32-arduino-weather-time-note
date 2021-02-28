
void saveBadgeInfo(Notice_struct *notice)
{
  EEPROM.writeString(0, notice->notice_1);
  EEPROM.writeString(100, notice->notice_2);
  EEPROM.writeString(200, notice->notice_3);
  EEPROM.writeString(300, notice->notice_4);
  EEPROM.writeString(400, notice->notice_5);
  EEPROM.writeString(500, notice->notice_6);
  EEPROM.commit();
}

void loadDefaultInfo(void)
{
  strlcpy(notice_str.notice_1, "无消息", sizeof(notice_str.notice_1));
  strlcpy(notice_str.notice_2, "无消息", sizeof(notice_str.notice_2));
  strlcpy(notice_str.notice_3, "无消息", sizeof(notice_str.notice_3));
  strlcpy(notice_str.notice_4, "无消息", sizeof(notice_str.notice_4));
  strlcpy(notice_str.notice_5, "无消息", sizeof(notice_str.notice_5));
  strlcpy(notice_str.notice_6, "无消息", sizeof(notice_str.notice_6));
  saveBadgeInfo(&notice_str);
}

bool loadBadgeInfo(Notice_struct *notice)
{
  if (EEPROM.readString(0) == "") {
    Serial.println("EEPROM empty");
    return false;
  }
  strlcpy(notice->notice_1, EEPROM.readString(0).c_str(), sizeof(notice->notice_1));
  strlcpy(notice->notice_2, EEPROM.readString(100).c_str(), sizeof(notice->notice_2));
  strlcpy(notice->notice_3, EEPROM.readString(200).c_str(), sizeof(notice->notice_3));
  strlcpy(notice->notice_4, EEPROM.readString(300).c_str(), sizeof(notice->notice_4));
  strlcpy(notice->notice_5, EEPROM.readString(400).c_str(), sizeof(notice->notice_5));
  strlcpy(notice->notice_6, EEPROM.readString(500).c_str(), sizeof(notice->notice_6));
  return true;
}

void notice_init() {
  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!loadBadgeInfo(&notice_str)) {
    loadDefaultInfo();
  }
}
