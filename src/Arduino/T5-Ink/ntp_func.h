
void ntp_init() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
