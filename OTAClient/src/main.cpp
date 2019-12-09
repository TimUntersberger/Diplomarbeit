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

void setupOtaConfig()
{
  printf("before ota config get thing name\n");
  thingName = EspConfig.getThingName();
  printf("before ota config get app name\n");
  EspConfig.getNvsStringValue("app_name", appName);
  printf("before ota config temp\n");
  char temp[256];
  strcpy(temp, firmwareUrl);
  strcat(temp, appName);
  printf("before ota config /download\n");
  config.url = (char *)malloc(256 * sizeof(char));
  strcpy((char *)config.url, "https://");
  strcat((char *)config.url, temp);
  strcat((char *)config.url, "/download");
  printf("before ota config /updatedAt\n");
  strcpy(updatedAtUrl, temp);
  strcat(updatedAtUrl, "/updatedAt");
  // config.cert_pem = (char *)cert_pem_start;
  config.cert_pem = (char *)CERT_PEM;
}

void checkOtaVersion()
{
  char response[11];
  //response is a unix timestamp (seconds) which is 10 digits long
  printf("before httpclient get\n");
  HttpClient.get(updatedAtUrl, response, 11, true); //need to fix response-length
  int newestUpdatedAt = atoi(response);
  if (newestUpdatedAt > updatedAt)
  {
    updatedAt = newestUpdatedAt;
    EspConfig.setNvsIntValue("updated_at", updatedAt);
    printf("%d\n", updatedAt);
    esp_https_ota(&config);
    esp_restart();
  }
  else
  {
    printf("is already newest version\n");
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
  printf("before everything\n");
  EspConfig.init();
  Logger.init("OTAClient");
  SerialLoggerTarget *serialLoggerTarget = new SerialLoggerTarget("ota", LOG_LEVEL_INFO);
  Logger.addLoggerTarget(serialLoggerTarget);
  //TODO: Optimize startup by avoiding unnecessary ota updates
  //TODO: Why do we need to specify content-length
  EspConfig.setNvsStringValue("app_name", "test");
  updatedAt = EspConfig.getNvsIntValue("updated_at");
  printf("before setup ota config\n");
  setupOtaConfig();
  printf("before start wifi\n");
  EspWifiManager.startWifi();
  printf("before is connecting loop\n");
  int timeout = 100;
  while (EspWifiManager.getIsConnecting()&& timeout != 0)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    timeout--;
  }

  if (EspWifiManager.getIsConnected())
  {
    printf("before is check ota version\n");
    checkOtaVersion();
    xTaskCreate(&checkOtaVersionTask, "check_ota_version", 4096, NULL, 5, NULL);
  }
  else
  {
    printf("not connected\n");
    EspAp.init();
    while (!EspAp.isApStarted())
    {
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    HttpServer.init();
  }

}
