/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <iostream>
//#include <thread>
//#include <string>
//#include <chrono>
//#include <inttypes.h>

//#include "sdkconfig.h"
//#include "esp_chip_info.h"
//#include "esp_flash.h"
//#include "esp_system.h"
//#include "esp_log.h"
//#include "nvs_flash.h"
//#include "esp_netif.h"
//#include "esp_event.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "sensor.h"


#include "max7219.h"
#include "adxl345.h"
#include "l3g4200d.h"
//#include "wifi_tcp_server.h"
//#include "bt_spp_server.h"
//#include "command_processor.h"

#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_0

#define CLK_PIN     18
#define MOSI_PIN    23
#define MISO_PIN    19
#define CS1_PIN      5
#define CS2_PIN     17
#define CS3_PIN     16

//#define WIFI_SSID     "ESP32_AP"
//#define WIFI_PASS     ""
//#define TCP_PORT      3333

max7219* display1 = nullptr;
adxl345* accel = nullptr;
l3g4200d* gyroscope = nullptr;



void d1(int x, int y, int z) {
    const int MID = 7;

    int dx = x - MID;
    int dy = y - MID;
    int dz = z - MID;
    
    int ax = abs(dx);
    int ay = abs(dy);
    int az = abs(dz);

    if (display1 != nullptr) {
        display1->clear_bufor();
        display1->update();
    }


    if (az >= ax && az >= ay) {
        if (z >= MID) {
            display1->set_display(5); std::cout << "5";
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);     
            display1->update();

        } else {
            display1->set_display(0); std::cout << "0";
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }

    else if (ax >= ay) {
        if (x >= MID) {
            display1->set_display(2); std::cout << "2";
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);     
            display1->update();
            
        } else {
            display1->set_display(4); std::cout << "4";
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }

    else {
        if (y >= MID) {
            display1->set_display(1); std::cout << "1";
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update(); 
        } else {
            display1->set_display(3); std::cout << "3";
            display1->side_displays_col(-dx, dz, 2, 0, true, true);
            display1->side_displays_row(-dx, dz, 1, 4, true, false);
            display1->side_displays_row(-dx, dz, 3, 2, false, true);
            display1->side_displays_row(-dx, dz, 0, 5, false, true);
            display1->update();
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
            display1->set_display(5); std::cout << "5";
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);
            display1->update();
        } else {
            display1->set_display(0); std::cout << "0";
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }
    else if (ax >= ay) {
        if (inv_x >= MID) {
            display1->set_display(2); std::cout << "2";
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);    
            display1->update();
        } else {
            display1->set_display(4); std::cout << "4";
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }
    else {
        if (inv_y >= MID) {
            display1->set_display(1); std::cout << "1";
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update(); 
        } else {
            display1->set_display(3); std::cout << "3";
            display1->side_displays_col(-dx, dz, 2, 0, true, true);
            display1->side_displays_row(-dx, dz, 1, 4, true, false);
            display1->side_displays_row(-dx, dz, 3, 2, false, true);
            display1->side_displays_row(-dx, dz, 0, 5, false, true);
            display1->update();
        }
    }
}


void d3(int x, int y, int z, int gx, int gy, int gz) {

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

    if (display1 != nullptr) {
        display1->clear_bufor();
        display1->update();
    }
    if (az >= ax && az >= ay) {
        if (z >= MID) {
            display1->set_display(5); std::cout << "5";
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);
            display1->update();
        } else {
            display1->set_display(0); std::cout << "0";
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }
    else if (ax >= ay) {
        if (x >= MID) {
            display1->set_display(2); std::cout << "2";
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);    
            display1->update();
        } else {
            display1->set_display(4); std::cout << "4";
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }
    else {
        if (y >= MID) {
            display1->set_display(1); std::cout << "1";
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update();
        } else {
            display1->set_display(3); std::cout << "3";
            display1->side_displays_col(-dx, dz, 2, 0, true, true);
            display1->side_displays_row(-dx, dz, 1, 4, true, false);
            display1->side_displays_row(-dx, dz, 3, 2, false, true);
            display1->side_displays_row(-dx, dz, 0, 5, false, true);
            display1->update();
        }
    }
}

extern "C" void  app_main(void){
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());


    spi_bus_config_t buscfg={};
        buscfg.miso_io_num = MISO_PIN,
        buscfg.mosi_io_num = MOSI_PIN,
        buscfg.sclk_io_num = CLK_PIN,
        buscfg.quadwp_io_num = -1,
        buscfg.quadhd_io_num = -1,
        buscfg.max_transfer_sz = 4092,
    

    spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    display1 = new max7219(VSPI_HOST, CS1_PIN);
    accel = new adxl345(VSPI_HOST, CS3_PIN);
    gyroscope = new l3g4200d(VSPI_HOST, CS2_PIN);
    accel->init();
    gyroscope->init();
    display1->clear_bufor();


    //setAccelPointer(accel);
    //setgyroscopePointer(gyroscope);

    //wifi_tcp_server_init(WIFI_SSID, WIFI_PASS, TCP_PORT);
   // xTaskCreate(wifi_tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    
    //bt_spp_server_init("ESP32_BT");
    int mode = 3;
    while (1) {
        	printf("Wyslano: 0x00 | Odebrano z Device 3: 0x%02X\n", accel->read_id());
        	printf("Wyslano: 0x2C | Odebrano z Device 3: 0x%02X\n", accel->read_bw_tate());
            
            printf("Wyslano: 0x0F | Odebrano z Device 2: 0x%02X\n",gyroscope->read_id());
            accel->update_raw_axes();
            gyroscope->update_raw_axes();


            std::cout << "X: " << accel -> getX()
                  << " Y: " << accel -> getY() 
                  << " Z: " << accel -> getZ() << std::endl;


            std::cout << "GX: " << gyroscope->getX()
                  << " GY: " << gyroscope->getY()
                  << " GZ: " << gyroscope->getZ() << std::endl;
            //int mode = getCurrentMode();
            

            if (gpio_get_level(BUTTON_PIN) == 0) {
                mode++;                 // Zwiększ tryb o 1
                if (mode > 3) {         // Jeśli przekroczy 3, wróć do 1
                    mode = 1;
                }
                printf("Zmieniono tryb na: %d\n", mode);

                // ZABEZPIECZENIE: Czekaj, aż użytkownik PUŚCI przycisk
                while (gpio_get_level(BUTTON_PIN) == 0) {

                    vTaskDelay(10 / portTICK_PERIOD_MS); 
                }
                
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            printf("Current mode: %d\n", mode);
            
            switch(mode) {
                case 1:
                    d1(accel -> getX(), accel -> getY(), accel -> getZ()); 
                    break;
                case 2:
                    d2(accel->getX(), accel->getY(), accel->getZ());
                    break;
                case 3:
                    d3(accel -> getX(), accel -> getY(), accel -> getZ(), gyroscope->getX(), gyroscope->getY(), gyroscope->getZ());
                    break;
                default:
                    d1(accel->getX(), accel->getY(), accel->getZ());
                    break;
            }
            display1->clear_bufor();
            
            vTaskDelay(400/portTICK_PERIOD_MS);

    }        
}


