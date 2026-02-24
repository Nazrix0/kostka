idf_component_register(SRCS "bt_spp_server.cpp" "wifi_tcp_server.cpp"  "command_processor.cpp" "main.cpp" "max7219.cpp" "adxl345.cpp" "l3g4200d.cpp"
                       PRIV_REQUIRES  spi_flash esp_system freertos driver esp_hw_support nvs_flash esp_wifi esp_netif esp_event bt
                           REQUIRES 
        nvs_flash 
        esp_wifi 
        esp_netif 
        esp_event 
        bt
        lwip
                       INCLUDE_DIRS "")
