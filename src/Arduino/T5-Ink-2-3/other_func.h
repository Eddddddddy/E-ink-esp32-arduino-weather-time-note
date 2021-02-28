
bool setPowerBoostKeepOn(int en)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  else
    Wire.write(0x35); // 0x37 is default reg value
  return Wire.endTransmission() == 0;
}

void other_init() {
#ifdef ENABLE_IP5306
  Wire.begin(I2C_SDA, I2C_SCL);
  bool ret = setPowerBoostKeepOn(1);
  Serial.printf("Power KeepUp %s\n", ret ? "PASS" : "FAIL");
#endif

  // It is only necessary to turn on the power amplifier power supply on the T5_V24 board.
#ifdef AMP_POWER_CTRL
  pinMode(AMP_POWER_CTRL, OUTPUT);
  digitalWrite(AMP_POWER_CTRL, HIGH);
#endif

#ifdef DAC_MAX98357
  AudioGeneratorMP3 *mp3;
  AudioFileSourcePROGMEM *file;
  AudioOutputI2S *out;
  AudioFileSourceID3 *id3;

  file = new AudioFileSourcePROGMEM(image, sizeof(image));
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S();
  out->SetPinout(IIS_BCK, IIS_WS, IIS_DOUT);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (1) {
    if (mp3->isRunning()) {
      if (!mp3->loop())
        mp3->stop();
    } else {
      Serial.printf("MP3 done\n");
      break;
    }
  }
#endif

  if (SPEAKER_OUT > 0) {
    ledcSetup(CHANNEL_0, 1000, 8);
    ledcAttachPin(SPEAKER_OUT, CHANNEL_0);
    int i = 3;
    while (i--) {
      //ledcWriteTone(CHANNEL_0, 1000);
      delay(200);
      ledcWriteTone(CHANNEL_0, 0);
    }
  }
}
