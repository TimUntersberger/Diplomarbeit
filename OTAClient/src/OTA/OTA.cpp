#include <OTA/OTA.h>

void OTAClass::init(char *_firmwareUrl, char *_cert_pem)
{
  setupUpdatedAtUrl();
  setupOtaConfig();

  cert_pem = _cert_pem;
  firmwareUrl = _firmwareUrl;
}

void OTAClass::start()
{
  checkOtaVersion();
  xTaskCreate(&checkOtaVersionTask, "check_ota_version", 8000, NULL, 5, NULL);
}

void OTAClass::checkOtaVersion()
{
  char response[11];
  //response is a unix timestamp (seconds) which is 10 digits long
  char updateAppName[20];
  setupUpdatedAtUrl();
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

void OTAClass::checkOtaVersionTask(void *pvParam)
{
  while (true)
  {
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
    checkOtaVersion();
  }
}

/**
 * Build updatedAtUrl with values from EspConfig
 */
void OTAClass::setupUpdatedAtUrl()
{
  char message[20];
  char temp[256];

  thingName = EspConfig.getThingName();
  sprintf(message, "Loaded thingname: %s", thingName);
  Logger.info(LOG_TAG, message);
  EspConfig.getNvsStringValue("appname", appName);
  sprintf(message, "Loaded appname: %s", appName);
  Logger.info(LOG_TAG, message);

  strcpy(temp, firmwareUrl);
  strcat(temp, appName);
  Logger.debug(LOG_TAG, "Initialized firmware url with appname");

  config.url = (char *)malloc(256 * sizeof(char));
  strcpy((char *)config.url, "https://");
  strcat((char *)config.url, temp);
  strcat((char *)config.url, "/download");
  Logger.debug(LOG_TAG, "Initialized ota config url");

  strcpy(updatedAtUrl, temp);
  strcat(updatedAtUrl, "/updatedAt");
  Logger.debug(LOG_TAG, "Initialized updatedAt url");
}

void OTAClass::setupOtaConfig()
{
  config.cert_pem = (char *)cert_pem;
  Logger.debug(LOG_TAG, "Initialized ota config certificate");
}