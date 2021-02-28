
void boot_init() {
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  if (bootCount == 2) {
    esp_restart();
  }
}

void powerSave() {
  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
  setCpuFrequencyMhz(80);
  Serial.print("CPU Frequency:");
  Serial.println(getCpuFrequencyMhz());
}
