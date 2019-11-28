#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <EspAp.h>
#include <EspConfig.h>
#include <HttpServer.h>
#include <EspWifiManager.h>
#include <esp_https_ota.h>
#include <esp_log.h>

// extern const uint8_t cert_pem_start[] asm("_binary_src_cert_pem_start");
// extern const uint8_t cert_pem_end[] asm("_binary_src_cert_pem_end");
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
  config.url = "https://ota.baaka.io/download";
  // config.cert_pem = (char *)cert_pem_start;
  config.cert_pem = (char *)CERT_PEM;
}

void app_main()
{

  EspConfig.init();
  setupOtaConfig();
  EspWifiManager.startWifi();

  while (EspWifiManager.getIsConnecting())
  {
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }

  if (EspWifiManager.getIsConnected())
  {
    printf("\nconnected\n\n");
    esp_https_ota(&config);
    esp_restart();
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
