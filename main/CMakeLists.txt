idf_component_register(SRCS "sensor.cpp" "spi_device.cpp"  "main.cpp" "max7219.cpp" "adxl345.cpp" "l3g4200d.cpp"
                       PRIV_REQUIRES  spi_flash esp_system freertos driver esp_hw_support nvs_flash esp_wifi esp_netif esp_event bt
                           REQUIRES 
        
        esp_wifi 
        esp_netif 
        esp_event 
        
        lwip
                       INCLUDE_DIRS "")
