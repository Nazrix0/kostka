/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <inttypes.h>

#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "max7219.h"
#include "adxl345.h"
#include "l3g4200d.h"
#include "wifi_tcp_server.h"
//#include "bt_spp_server.h"
#include "command_processor.h"

#define CLK_PIN     18
#define MOSI_PIN    23
#define MISO_PIN    19
#define CS1_PIN      5
#define CS2_PIN     17
#define CS3_PIN     16

#define WIFI_SSID     "ESP32_AP"
#define WIFI_PASS     ""
#define TCP_PORT      3333

max7219* display1 = nullptr;
adxl345* accel = nullptr;
l3g4200d* gyroscope = nullptr;
void set_main(int display){
    if (display1 != nullptr) {
        for(int row = 0; row < 8; row++) {
            display1->set_row(display, row, 8);  
        }
    }
}


void side_displays_col(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw = false, bool reverse_dir = false) {
    if (display1 == nullptr) return;
    for (int col = 0; col < 8; col++) {
        int cx = 0, cy = 0;
        int mapped_idx = col * 2 - 7; 
        if (reverse_dir) mapped_idx = -mapped_idx;
        
        if (wall_pos == 0)      { cx = mapped_idx; cy = 7; }         
        else if (wall_pos == 1) { cx = 7; cy = -mapped_idx; }        
        else if (wall_pos == 2) { cx = -mapped_idx; cy = -7; }       
        else if (wall_pos == 3) { cx = -7; cy = mapped_idx; }        
        
        int height = 4 + (dx * cx + dy * cy) / 6;
        
        if (height < 0) height = 0;
        if (height > 8) height = 8;
        
        int hw_value = height;
        if (invert_hw && height > 0 && height < 8) {
            hw_value = 16 - height;
        }
        display1->set_col(display_num, col, hw_value);
    }
}


void side_displays_row(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw = false, bool reverse_dir = false) {
    if (display1 == nullptr) return;
    for (int row = 0; row < 8; row++) {
        int cx = 0, cy = 0;
        int mapped_idx = row * 2 - 7;
        if (reverse_dir) mapped_idx = -mapped_idx;
        
        if (wall_pos == 0)      { cx = mapped_idx; cy = 7; }         
        else if (wall_pos == 1) { cx = 7; cy = -mapped_idx; }        
        else if (wall_pos == 2) { cx = -mapped_idx; cy = -7; }       
        else if (wall_pos == 3) { cx = -7; cy = mapped_idx; }        
        
        int width = 4 + (dx * cx + dy * cy) / 6;
        if (width < 0) width = 0;
        if (width > 8) width = 8;
        
        int hw_value = width;
        if (invert_hw && width > 0 && width < 8) {
            hw_value = 16 - width; 
        }
        display1->set_row(display_num, row, hw_value);
    }
}


void d1(int x, int y, int z) {
    const int MID = 7;

    int dx = x - MID;
    int dy = y - MID;
    int dz = z - MID;
    
    int ax = abs(dx);
    int ay = abs(dy);
    int az = abs(dz);

    if (display1 != nullptr) display1->clear();


    if (az >= ax && az >= ay) {
        if (z >= MID) {
            set_main(5); std::cout << "5";
            side_displays_col(dx, dy, 0, 1, false, true); 
            side_displays_col(dx, dy, 3, 4, false, true); 
            side_displays_col(dx, dy, 2, 3, false, true);   
            side_displays_col(dx, dy, 1, 2, false, true);     
            display1->clear_single_display(0);
        } else {
            set_main(0); std::cout << "0";
            side_displays_col(dx, -dy, 2, 1, true, false);
            side_displays_col(dx, -dy, 3, 4, true, false);
            side_displays_col(dx, -dy, 0, 3, true, false);
            side_displays_col(dx, -dy, 1, 2, true, false);
            display1->clear_single_display(5);
        }
    }

    else if (ax >= ay) {
        if (x >= MID) {
            set_main(2); std::cout << "2";
            side_displays_row(dy, dz, 2, 0, false, false); 
            side_displays_row(dy, dz, 3, 3, true, true); 
            side_displays_row(dy, dz, 1, 1, false, false);   
            side_displays_col(dy, dz, 0, 5, true, false);     
            display1->clear_single_display(4);
            
        } else {
            set_main(4); std::cout << "4";
            side_displays_row(-dy, dz, 2, 0, true, true);
            side_displays_row(-dy, dz, 1, 3, false, false);
            side_displays_row(-dy, dz, 3, 1, true, true);
            side_displays_col(-dy, dz, 0, 5, false, true);
            display1->clear_single_display(2);
        }
    }

    else {
        if (y >= MID) {
            set_main(1); std::cout << "1";
            side_displays_col(dx, dz, 2, 0, false, false); 
            side_displays_row(dx, dz, 3, 4, false, true); 
            side_displays_row(dx, dz, 1, 2, true, false);   
            side_displays_row(dx, dz, 0, 5, true, false);   
            display1->clear_single_display(3); 
        } else {
            set_main(3); std::cout << "3";
            side_displays_col(-dx, dz, 2, 0, true, true);
            side_displays_row(-dx, dz, 1, 4, true, false);
            side_displays_row(-dx, dz, 3, 2, false, true);
            side_displays_row(-dx, dz, 0, 5, false, true);
            display1->clear_single_display(1);
        }
    }
}

void d2(int x, int y, int z) {
    const int MID = 7;
    
    int dx = -(x - MID);
    int dy = -(y - MID);
    int dz = -(z - MID);

    int inv_x = MID + dx;
    int inv_y = MID + dy;
    int inv_z = MID + dz;
    
    int ax = abs(dx);
    int ay = abs(dy);
    int az = abs(dz);

    if (az >= ax && az >= ay) {
        if (inv_z >= MID) {
            set_main(5); std::cout << "5";
            side_displays_col(dx, dy, 0, 1, false, true); 
            side_displays_col(dx, dy, 3, 4, false, true); 
            side_displays_col(dx, dy, 2, 3, false, true);   
            side_displays_col(dx, dy, 1, 2, false, true);
            display1->clear_single_display(0);
        } else {
            set_main(0); std::cout << "0";
            side_displays_col(dx, -dy, 2, 1, true, false);
            side_displays_col(dx, -dy, 3, 4, true, false);
            side_displays_col(dx, -dy, 0, 3, true, false);
            side_displays_col(dx, -dy, 1, 2, true, false);
            display1->clear_single_display(5);
        }
    }
    else if (ax >= ay) {
        if (inv_x >= MID) {
            set_main(2); std::cout << "2";
            side_displays_row(dy, dz, 2, 0, false, false); 
            side_displays_row(dy, dz, 3, 3, true, true); 
            side_displays_row(dy, dz, 1, 1, false, false);   
            side_displays_col(dy, dz, 0, 5, true, false);    
            display1->clear_single_display(4);
        } else {
            set_main(4); std::cout << "4";
            side_displays_row(-dy, dz, 2, 0, true, true);
            side_displays_row(-dy, dz, 1, 3, false, false);
            side_displays_row(-dy, dz, 3, 1, true, true);
            side_displays_col(-dy, dz, 0, 5, false, true);
            display1->clear_single_display(2);
        }
    }
    else {
        if (inv_y >= MID) {
            set_main(1); std::cout << "1";
            side_displays_col(dx, dz, 2, 0, false, false); 
            side_displays_row(dx, dz, 3, 4, false, true); 
            side_displays_row(dx, dz, 1, 2, true, false);   
            side_displays_row(dx, dz, 0, 5, true, false);   
            display1->clear_single_display(3); 
        } else {
            set_main(3); std::cout << "3";
            side_displays_col(-dx, dz, 2, 0, true, true);
            side_displays_row(-dx, dz, 1, 4, true, false);
            side_displays_row(-dx, dz, 3, 2, false, true);
            side_displays_row(-dx, dz, 0, 5, false, true);
            display1->clear_single_display(1);
        }
    }
}


int map_gyro(int16_t raw_val) {
    int step = raw_val / 3000; 
    
    int mapped = 3 + step;
    

    if (mapped < 0) mapped = 0;
    if (mapped > 5) mapped = 5;
    
    return mapped;
}

void d3(int x, int y, int z, int gx, int gy, int gz) {

    int gx_mapped = map_gyro(gx);
    int gy_mapped = map_gyro(gy);
    int gz_mapped = map_gyro(gz);

    x += (gx_mapped - 3);
    y += (gy_mapped - 3);
    z += (gz_mapped - 3);

    if (x < 0) x = 0;
    if (x > 15) x = 15;
    if (y < 0) y = 0; 
    if (y > 15) y = 15;
    if (z < 0) z = 0; 
    if (z > 15) z = 15;

    const int MID = 7;
    int dx = x - MID;
    int dy = y - MID;
    int dz = z - MID;
    
    int ax = std::abs(dx);
    int ay = std::abs(dy);
    int az = std::abs(dz);

    if (display1 != nullptr) display1->clear();

    if (az >= ax && az >= ay) {
        if (z >= MID) {
            set_main(5); std::cout << "5";
            side_displays_col(dx, dy, 0, 1, false, true); 
            side_displays_col(dx, dy, 3, 4, false, true); 
            side_displays_col(dx, dy, 2, 3, false, true);   
            side_displays_col(dx, dy, 1, 2, false, true);
            display1->clear_single_display(0);
        } else {
            set_main(0); std::cout << "0";
            side_displays_col(dx, -dy, 2, 1, true, false);
            side_displays_col(dx, -dy, 3, 4, true, false);
            side_displays_col(dx, -dy, 0, 3, true, false);
            side_displays_col(dx, -dy, 1, 2, true, false);
            display1->clear_single_display(5);
        }
    }
    else if (ax >= ay) {
        if (x >= MID) {
            set_main(2); std::cout << "2";
            side_displays_row(dy, dz, 2, 0, false, false); 
            side_displays_row(dy, dz, 3, 3, true, true); 
            side_displays_row(dy, dz, 1, 1, false, false);   
            side_displays_col(dy, dz, 0, 5, true, false);    
            display1->clear_single_display(4);
        } else {
            set_main(4); std::cout << "4";
            side_displays_row(-dy, dz, 2, 0, true, true);
            side_displays_row(-dy, dz, 1, 3, false, false);
            side_displays_row(-dy, dz, 3, 1, true, true);
            side_displays_col(-dy, dz, 0, 5, false, true);
            display1->clear_single_display(2);
        }
    }
    else {
        if (y >= MID) {
            set_main(1); std::cout << "1";
            side_displays_col(dx, dz, 2, 0, false, false); 
            side_displays_row(dx, dz, 3, 4, false, true); 
            side_displays_row(dx, dz, 1, 2, true, false);   
            side_displays_row(dx, dz, 0, 5, true, false);   
            display1->clear_single_display(3);
        } else {
            set_main(3); std::cout << "3";
            side_displays_col(-dx, dz, 2, 0, true, true);
            side_displays_row(-dx, dz, 1, 4, true, false);
            side_displays_row(-dx, dz, 3, 2, false, true);
            side_displays_row(-dx, dz, 0, 5, false, true);
            display1->clear_single_display(1);
        }
    }
}

extern "C" void  app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    spi_bus_config_t buscfg={};
        buscfg.miso_io_num = MISO_PIN,
        buscfg.mosi_io_num = MOSI_PIN,
        buscfg.sclk_io_num = CLK_PIN,
        buscfg.quadwp_io_num = -1,
        buscfg.quadhd_io_num = -1,
        buscfg.max_transfer_sz = 4092,
    

    spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    display1 = new max7219(CS1_PIN,VSPI_HOST);
    accel = new adxl345(CS3_PIN,VSPI_HOST);
    gyroscope = new l3g4200d(CS2_PIN,VSPI_HOST);
    accel->init();
    gyroscope->init();
    display1->clear();


    setAccelPointer(accel);
    setgyroscopePointer(gyroscope);

    wifi_tcp_server_init(WIFI_SSID, WIFI_PASS, TCP_PORT);
    xTaskCreate(wifi_tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    
    //bt_spp_server_init("ESP32_BT");

    while (1) {
        	printf("Wyslano: 0x00 | Odebrano z Device 3: 0x%02X\n", accel->read_id());
        	printf("Wyslano: 0x2C | Odebrano z Device 3: 0x%02X\n", accel->read_bw_tate());
            
            printf("Wyslano: 0x0F | Odebrano z Device 2: 0x%02X\n",gyroscope->read_id());

            AccelData reading = accel->read_all_axes();
        
            int x = reading.x; 
            int y = reading.y; 
            int z = reading.z; 

            std::cout << "X: " << x
                  << " Y: " << y 
                  << " Z: " << z << std::endl;

            GyroData reading1 = gyroscope->read_all_axes();
            std::cout << "GX: " << reading1.x 
                  << " GY: " << reading1.y 
                  << " GZ: " << reading1.z << std::endl;
            int mode = getCurrentMode();
            printf("Current mode: %d\n", mode);
            
            switch(mode) {
                case 1:
                    d1(x, y, z); 
                    break;
                case 2:
                    d2(x, y, z);
                    break;
                case 3:
                    d3(x, y, z, reading1.x, reading1.y, reading1.z);
                    break;
                default:
                    d1(x, y, z);
                    break;
            }

            
            vTaskDelay(20/portTICK_PERIOD_MS);

    }        
}


