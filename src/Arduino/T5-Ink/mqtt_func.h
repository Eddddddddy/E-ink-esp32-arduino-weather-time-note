
bool msi_info_mqtt_json(char s[]) {
#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(s);
  if (!root.success()) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
  root.printTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
  DeserializationError error = deserializeJson(root, s);
  if (error) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
#endif
  if ((const char *)root["cpu"] == NULL) {
    return false;
  }
  strlcpy(msi_info_str.cpu, root["cpu"], sizeof(msi_info_str.cpu));
  strlcpy(msi_info_str.gpu, root["gpu"], sizeof(msi_info_str.gpu));

  thirdPage();
  return true;
}

bool weather_mqtt_json(char s[]) {
  char code1[8], code2[8];
  static int i = 0;
#if ARDUINOJSON_VERSION_MAJOR == 5
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(s);
  if (!root.success()) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
  root.printTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
  StaticJsonDocument<256> root;
  DeserializationError error = deserializeJson(root, s);
  if (error) {
    Serial.println(F("Failed to json weather"));
    return false;
  }
#endif
  if ((const char *)root["code_1"] == NULL) {
    return false;
  }

  strlcpy(code1, wea_str.code_1, sizeof(code1));
  strlcpy(code2, wea_str.code_2, sizeof(code2));
  strlcpy(wea_str.code_1, root["code_1"], sizeof(wea_str.code_1));
  strlcpy(wea_str.code_2, root["code_2"], sizeof(wea_str.code_2));
  strlcpy(wea_str.wea_1, root["wea_1"], sizeof(wea_str.wea_1));
  strlcpy(wea_str.wea_2, root["wea_2"], sizeof(wea_str.wea_2));
  strlcpy(wea_str.tmp_1_min, root["tmp_1_min"], sizeof(wea_str.tmp_1_min));
  strlcpy(wea_str.tmp_1_max, root["tmp_1_max"], sizeof(wea_str.tmp_1_max));
  strlcpy(wea_str.tmp_2_min, root["tmp_2_min"], sizeof(wea_str.tmp_2_min));
  strlcpy(wea_str.tmp_2_max, root["tmp_2_max"], sizeof(wea_str.tmp_2_max));
  //Serial.println(wea_str.code_1);
  if (strcmp(code1, wea_str.code_1) || strcmp(code2, wea_str.code_2)) {
    i = 10;
  }
  if (i >= 10) {
    flashWeather();
    i = 0;
  }
  first_weather();
  second_weather();
  i++;
  return true;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char s[500] = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    sprintf(s, "%s%c", s, (char)payload[i]);
  }
  Serial.println();
  //  Serial.print(s);
  //  Serial.println();
  //String c=Encoding.ASCII.GetChars(payload);
  if (strcmp(topic, "notice") == 0) {
    strlcpy(notice_str.notice_6, notice_str.notice_5, sizeof(notice_str.notice_6));
    strlcpy(notice_str.notice_5, notice_str.notice_4, sizeof(notice_str.notice_5));
    strlcpy(notice_str.notice_4, notice_str.notice_3, sizeof(notice_str.notice_4));
    strlcpy(notice_str.notice_3, notice_str.notice_2, sizeof(notice_str.notice_3));
    strlcpy(notice_str.notice_2, notice_str.notice_1, sizeof(notice_str.notice_2));
    strlcpy(notice_str.notice_1, s, sizeof(notice_str.notice_1));
    saveBadgeInfo(&notice_str);
    Serial.print(notice_str.notice_1);
    if (page_state == 1) {
      firstPage();
    } else if (page_state == 2) {
      secondPage();
    }
    client.publish("notice_back", "already received notice");
  } else if (strcmp(topic, "weather") == 0) {
    weather_mqtt_json(s);
  } else if (strcmp(topic, "msi_info") == 0) {
    if (page_state == 3)
      msi_info_mqtt_json(s);
  } else if (strcmp(topic, "to_temperature") == 0) {
    char s[8];
    itoa(bme680_str.temperature, s, 10);
    client.publish("from_temperature", s);
  } else if (strcmp(topic, "to_humidity") == 0) {
    char s[8];
    itoa(bme680_str.humidity, s, 10);
    client.publish("from_humidity", s);
  } else if (strcmp(topic, "to_pressure") == 0) {
    char s[8];
    itoa(bme680_str.pressure, s, 10);
    client.publish("from_pressure", s);
  } else if (strcmp(topic, "to_iaq") == 0) {
    char s[8];
    itoa(bme680_str.iaq, s, 10);
    client.publish("from_iaq", s);
  }
}

void reconnect() {
  static int failed_time = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      failed_time = 0;
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("debug", "ESP32 connected");
      // ... and resubscribe
      client.subscribe("notice");
      client.subscribe("weather");
      client.subscribe("msi_info");
      client.subscribe("to_temperature");
      client.subscribe("to_humidity");
      client.subscribe("to_pressure");
      client.subscribe("to_iaq");
    } else {
      if (failed_time > 10) {
        esp_restart();
      }
      failed_time++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
