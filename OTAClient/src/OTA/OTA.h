#include <esp_http_client.h>
#include <EspConfig.h>
#include <Logger.h>
#include <esp_log.h>
#include <stdio.h>
#include <string.h>
#include <HttpClient.h>
#include <esp_https_ota.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_TAG "OTA"

#pragma once

class OTAClass
{
public:
  OTAClass();
  void init(char *_firmwareUrl, char *_cert_pem);
  void start();

private:
  void setupOtaConfig();
  void setupUpdatedAtUrl();
  void checkOtaVersion();
  void checkOtaVersionTask(void *pvParam);

  esp_http_client_config_t config = {};
  //TODO: Set max length in ui to 20
  char appName[20];
  char *thingName;
  char *cert_pem;
  int updatedAt;

  char *firmwareUrl;
  char updatedAtUrl[256];
};
extern OTAClass OTA;
