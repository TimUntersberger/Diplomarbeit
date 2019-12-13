#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <EspAp.h>
#include <EspConfig.h>
#include <HttpServer.h>
#include <HttpClient.h>
#include <EspWifiManager.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <Logger.h>
#include <LoggerTarget.h>
#include <SerialLoggerTarget.h>

#define LOG_TAG "OTA"

// extern const uint8_t cert_pem_start[] asm("_binary_src_cert_pem_start");
// extern const uint8_t cert_pem_end[] asm("_binary_src_cert_pem_end");

//TODO: Set max length in ui to 20
char appName[20];
char *thingName;
int updatedAt;

char *firmwareUrl = "ota.baaka.io/api/firmware/";
char updatedAtUrl[256];

const char *CERT_PEM = "-----BEGIN CERTIFICATE-----\n"
                       "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/"
                       "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT"
                       "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow"
                       "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD"
                       "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB"
                       "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O"
                       "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq"
                       "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b"
                       "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw"
                       "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD"
                       "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV"
                       "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG"
                       "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69"
                       "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr"
                       "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz"
                       "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5"
                       "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo"
                       "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ"
                       "\n-----END CERTIFICATE-----\n";

extern "C"
{
  void app_main();
}

esp_http_client_config_t config = {};
SerialLoggerTarget *serialLoggerTarget = new SerialLoggerTarget("ota", LOG_LEVEL_ERROR);

void setupOtaConfig()
{
  char message[20];
  thingName = EspConfig.getThingName();
  sprintf(message, "Loaded thingname: %s", thingName);
  Logger.info(LOG_TAG, message);
  EspConfig.getNvsStringValue("appName", appName);
  sprintf(message, "Loaded appname: %s", appName);
  Logger.info(LOG_TAG, message);
  char temp[256];
  strcpy(temp, firmwareUrl);
  strcat(temp, appName);
  Logger.debug(LOG_TAG, "Initialized firmware url with appName");
  config.url = (char *)malloc(256 * sizeof(char));
  strcpy((char *)config.url, "https://");
  strcat((char *)config.url, temp);
  strcat((char *)config.url, "/download");
  Logger.debug(LOG_TAG, "Initialized ota config url");
  strcpy(updatedAtUrl, temp);
  strcat(updatedAtUrl, "/updatedAt");
  // config.cert_pem = (char *)cert_pem_start;
  Logger.debug(LOG_TAG, "Initialized updatedAt url");
  config.cert_pem = (char *)CERT_PEM;
  Logger.debug(LOG_TAG, "Initialized ota config certificate");
}

void checkOtaVersion()
{
  char response[11];
  //response is a unix timestamp (seconds) which is 10 digits long
  char updateAppName[20];
  Logger.debug(LOG_TAG, "starting GET request to updatedAtUrl endpoint");
  HttpClient.get(updatedAtUrl, response, 11, true); //need to fix response-length
  int newestUpdatedAt = atoi(response);
  EspConfig.getNvsStringValue("updateAppName", updateAppName);
  if (strcmp(appName, updateAppName) != 0)
  {
    updatedAt = 0;
  }
  if (newestUpdatedAt > updatedAt)
  {
    Logger.info(LOG_TAG, "newer version is available");
    updatedAt = newestUpdatedAt;
    EspConfig.setNvsStringValue("updateAppName", updateAppName);
    EspConfig.setNvsIntValue("updatedAt", updatedAt);
    esp_https_ota(&config);
    esp_restart();
  }
  else
  {
    Logger.info(LOG_TAG, "is already newest version");
  }
}

void checkOtaVersionTask(void *pvParam)
{
  while (true)
  {
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
    checkOtaVersion();
  }
}

void app_main()
{
  Logger.debug(LOG_TAG, "entered app_main");
  EspConfig.init();
  Logger.init("OTAClient");

  Logger.addLoggerTarget(serialLoggerTarget);
  //TODO: Optimize startup by avoiding unnecessary ota updates
  //TODO: Why do we need to specify content-length
  EspConfig.setNvsStringValue("appName", "test");
  updatedAt = EspConfig.getNvsIntValue("updatedAt");
  setupOtaConfig();
  Logger.debug(LOG_TAG, "ota config setup complete");
  EspWifiManager.startWifi();
  Logger.debug(LOG_TAG, "wifi started");
  int timeout = 100;
  while (EspWifiManager.getIsConnecting() && timeout != 0)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    timeout--;
  }
  if (EspWifiManager.getIsConnected())
  {
    Logger.info(LOG_TAG, "connected to wifi");
    checkOtaVersion();
    xTaskCreate(&checkOtaVersionTask, "check_ota_version", 4096, NULL, 5, NULL);
  }
  else
  {
    Logger.info(LOG_TAG, "couldn't connect to wifi");
    EspAp.init();
    while (!EspAp.isApStarted())
    {
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    HttpServer.init();
  }
}
