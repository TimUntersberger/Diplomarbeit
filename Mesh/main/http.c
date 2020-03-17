#include "http.h"

static const char* TAG = "http_client";
int http_response_buffer_size = 0;
char http_response_buffer[HTTP_RESPONSE_BUFFER_SIZE] = {0};

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            for(int i = 0; i < evt->data_len; i++){
                http_response_buffer[http_response_buffer_size] = ((char*)evt->data)[i];
                http_response_buffer_size++;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            http_response_buffer[http_response_buffer_size] = '\0';
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_err_t http_request(esp_http_client_method_t method, const char* url){
    esp_http_client_config_t config = {
       .url = url,
       .method = method,
       .event_handler = _http_event_handle,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
       ESP_LOGI(TAG, "Status = %d, content_length = %d",
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));
    }

    esp_http_client_cleanup(client);    

    return err;
}

char* http_get_response_body(){
    return http_response_buffer;
}

int http_get_response_body_size(){
    return http_response_buffer_size;
}
