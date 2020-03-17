#ifndef HTTP
#define HTTP

#include "esp_http_client.h"
#include "esp_log.h"

#define HTTP_RESPONSE_BUFFER_SIZE (1000)

esp_err_t http_request(esp_http_client_method_t method, const char* url);
char* http_get_response_body();
int http_get_response_body_size();

#endif
