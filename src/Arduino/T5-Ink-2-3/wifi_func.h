

void WebServerStart(void)
{

#ifdef USE_AP_MODE
  uint8_t mac[6];
  char apName[18] = {0};
  IPAddress apIP = IPAddress(192, 168, 1, 1);

  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  esp_wifi_get_mac(WIFI_IF_STA, mac);

  sprintf(apName, "TTGO-Badge-%02X:%02X", mac[4], mac[5]);

  if (!WiFi.softAP(apName)) {
    Serial.println("AP Config failed.");
    return;
  } else {
    Serial.println("AP Config Success.");
    Serial.print("AP MAC: ");
    Serial.println(WiFi.softAPmacAddress());
  }
#else
  WiFi.mode(WIFI_STA);
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);
  int tryTime = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    //while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tryTime++;
    if (tryTime > 10) {
      esp_restart();
    }
  }
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(WiFi.localIP());
#endif

#ifdef WEBSERVER
  {
    if (MDNS.begin("ttgo")) {
      Serial.println("MDNS responder started");
    }

    server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.html");

    server.on("css/main.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "css/main.css", "text/css");
    });
    server.on("js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "js/jquery.min.js", "application/javascript");
    });
    server.on("js/tbdValidate.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILESYSTEM, "js/tbdValidate.js", "application/javascript");
    });
    server.on("/data", HTTP_POST, [](AsyncWebServerRequest * request) {
      request->send(200, "text/plain", "");

      for (int i = 0; i < request->params(); i++) {
        String name = request->getParam(i)->name();
        String params = request->getParam(i)->value();
        Serial.println(name + " : " + params);
        if (name == "company") {
          strlcpy(info.company, params.c_str(), sizeof(info.company));
        } else if (name == "name") {
          strlcpy(info.name, params.c_str(), sizeof(info.name));
        } else if (name == "address") {
          strlcpy(info.address, params.c_str(), sizeof(info.address));
        } else if (name == "email") {
          strlcpy(info.email, params.c_str(), sizeof(info.email));
        } else if (name == "link") {
          strlcpy(info.link, params.c_str(), sizeof(info.link));
        } else if (name == "tel") {
          strlcpy(info.tel, params.c_str(), sizeof(info.tel));
        }
      }
      saveBadgeInfo(&info);
    });

    server.onFileUpload([](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
      static File file;
      static int pathIndex = 0;
      if (!index) {
        Serial.printf("UploadStart: %s\n", filename.c_str());
        file = FILESYSTEM.open(path[pathIndex], FILE_WRITE);
        if (!file) {
          Serial.println("Open FAIL");
          request->send(500, "text/plain", "hander error");
          return;
        }
      }
      if (file.write(data, len) != len) {
        Serial.println("Write fail");
        request->send(500, "text/plain", "hander error");
        file.close();
        return;
      }

      if (final) {
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
        file.close();
        request->send(200, "text/plain", "");
        if (++pathIndex >= 2) {
          pathIndex = 0;
          showMianPage();
        }
      }
    });

    server.onNotFound([](AsyncWebServerRequest * request) {
      request->send(404, "text/plain", "Not found");
    });

    MDNS.addService("http", "tcp", 80);

    server.begin();
  }
#endif
}
