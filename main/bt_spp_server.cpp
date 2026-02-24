#include "bt_spp_server.h"
#include "command_processor.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "esp_log.h"
#include <string>

static const char *TAG = "BT_SPP";
static uint32_t spp_handle = 0;

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(TAG, "ESP_SPP_INIT_EVT");
        esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, "ESP32_SPP");
        break;
        
    case ESP_SPP_START_EVT:
        ESP_LOGI(TAG, "ESP_SPP_START_EVT");
        break;
        
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_SPP_SRV_OPEN_EVT");
        spp_handle = param->srv_open.handle;
        break;
        
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(TAG, "ESP_SPP_DATA_IND_EVT len=%d", param->data_ind.len);
        
        if (param->data_ind.len > 0) {
            std::string data((char*)param->data_ind.data, param->data_ind.len);
            
            size_t pos = data.find('\n');
            if (pos != std::string::npos) data = data.substr(0, pos);
            pos = data.find('\r');
            if (pos != std::string::npos) data = data.substr(0, pos);
            
            ESP_LOGI(TAG, "BT RX: %s", data.c_str());
            
            std::string response = processCommand(data);
            response += "\n";
            
            esp_spp_write(spp_handle, response.length(), (uint8_t*)response.c_str());
            ESP_LOGI(TAG, "BT TX: %s", response.c_str());
        }
        break;
        
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_SPP_CLOSE_EVT");
        spp_handle = 0;
        break;
        
    default:
        break;
    }
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "authentication success: %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    default:
        break;
    }
}

void bt_spp_server_init(const char* device_name)
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    ESP_ERROR_CHECK(esp_bt_gap_register_callback(esp_bt_gap_cb));
    ESP_ERROR_CHECK(esp_spp_register_callback(esp_spp_cb));
    
    esp_spp_cfg_t bt_spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = true,
        .tx_buffer_size = 0, 
    };
    ESP_ERROR_CHECK(esp_spp_enhanced_init(&bt_spp_cfg));
    
    ESP_ERROR_CHECK(esp_bt_gap_set_device_name(device_name));
    ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE));
    
    ESP_LOGI(TAG, "Bluetooth started: %s", device_name);
}
