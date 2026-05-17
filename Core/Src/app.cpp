//#include "app.h"
#include "main.h" // Pobiera z CubeMX definicje np. CS1_Pin, BUTTON_Pin
#include "max7219.h"
#include "adxl345.h"
#include "l3g4200d.h"
//#include <iostream>

// Zmienna hspi1 jest generowana w main.c, tu mówimy C++, że ona istnieje
extern SPI_HandleTypeDef hspi1; 

max7219* display1 = nullptr;
adxl345* accel = nullptr;
l3g4200d* gyroscope = nullptr;

// ... Tutaj przeklejasz swoje funkcje d1(), d2(), d3() bez żadnych zmian ...

int abs(int v){
    return (v < 0) ? -v : v;
}
void d1(int x, int y, int z) {
    const int MID = 7;

    int dx = x - MID;
    int dy = y - MID;
    int dz = z - MID;
    
    int ax = abs(dx);
    int ay = abs(dy);
    int az = abs(dz);

    if (display1 != nullptr) display1->clear_bufor();


    if (az >= ax && az >= ay) {
        if (z >= MID) {
            display1->set_display(5); 
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);     
            display1->update();

        } else {
            display1->set_display(0); 
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }

    else if (ax >= ay) {
        if (x >= MID) {
            display1->set_display(2); 
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);     
            display1->update();
            
        } else {
            display1->set_display(4); 
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }

    else {
        if (y >= MID) {
            display1->set_display(1);
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update(); 
        } else {
            display1->set_display(3); 
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

    if (display1 != nullptr) display1->clear_bufor();

    if (az >= ax && az >= ay) {
        if (inv_z >= MID) {
            display1->set_display(5); 
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);
            display1->update();
        } else {
            display1->set_display(0); 
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }
    else if (ax >= ay) {
        if (inv_x >= MID) {
            display1->set_display(2);
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);    
            display1->update();
        } else {
            display1->set_display(4); 
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }
    else {
        if (inv_y >= MID) {
            display1->set_display(1); 
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update(); 
        } else {
            display1->set_display(3); 
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
    
    int ax = abs(dx);
    int ay = abs(dy);
    int az = abs(dz);

    if (display1 != nullptr) display1->clear_bufor();


    if (az >= ax && az >= ay) {
        if (z >= MID) {
            display1->set_display(5); 
            display1->side_displays_col(dx, dy, 0, 1, false, true); 
            display1->side_displays_col(dx, dy, 3, 4, false, true); 
            display1->side_displays_col(dx, dy, 2, 3, false, true);   
            display1->side_displays_col(dx, dy, 1, 2, false, true);
            display1->update();
        } else {
            display1->set_display(0); 
            display1->side_displays_col(dx, -dy, 2, 1, true, false);
            display1->side_displays_col(dx, -dy, 3, 4, true, false);
            display1->side_displays_col(dx, -dy, 0, 3, true, false);
            display1->side_displays_col(dx, -dy, 1, 2, true, false);
            display1->update();
        }
    }
    else if (ax >= ay) {
        if (x >= MID) {
            display1->set_display(2); 
            display1->side_displays_row(dy, dz, 2, 0, false, false); 
            display1->side_displays_row(dy, dz, 3, 3, true, true); 
            display1->side_displays_row(dy, dz, 1, 1, false, false);   
            display1->side_displays_col(dy, dz, 0, 5, true, false);    
            display1->update();
        } else {
            display1->set_display(4);
            display1->side_displays_row(-dy, dz, 2, 0, true, true);
            display1->side_displays_row(-dy, dz, 1, 3, false, false);
            display1->side_displays_row(-dy, dz, 3, 1, true, true);
            display1->side_displays_col(-dy, dz, 0, 5, false, true);
            display1->update();
        }
    }
    else {
        if (y >= MID) {
            display1->set_display(1);
            display1->side_displays_col(dx, dz, 2, 0, false, false); 
            display1->side_displays_row(dx, dz, 3, 4, false, true); 
            display1->side_displays_row(dx, dz, 1, 2, true, false);   
            display1->side_displays_row(dx, dz, 0, 5, true, false);   
            display1->update();
        } else {
            display1->set_display(3); 
            display1->side_displays_col(-dx, dz, 2, 0, true, true);
            display1->side_displays_row(-dx, dz, 1, 4, true, false);
            display1->side_displays_row(-dx, dz, 3, 2, false, true);
            display1->side_displays_row(-dx, dz, 0, 5, false, true);
            display1->update();
        }
    }
}

extern "C" void cpp_main(void) {
    // Inicjalizacja obiektów. 
    // Zakładam, że w CubeMX nazwałeś piny CS jako CS1, CS2 i CS3
    display1 = new max7219(&hspi1, CS1_GPIO_Port, CS1_Pin);
    accel = new adxl345(&hspi1, CS3_GPIO_Port, CS3_Pin);
    gyroscope = new l3g4200d(&hspi1, CS2_GPIO_Port, CS2_Pin);
    
    accel->init();
    gyroscope->init();
    display1->clear_bufor();

    int mode = 3;
    while (1) {
        accel->update_raw_axes();
        gyroscope->update_raw_axes();

        // Obsługa przycisku (Zmiana z gpio_get_level na HAL_GPIO_ReadPin)
        if (HAL_GPIO_ReadPin(TCK_GPIO_Port, TCK_Pin) == GPIO_PIN_RESET) {
            mode++;
            if (mode > 3) mode = 1;
            
            // Debouncing
            while (HAL_GPIO_ReadPin(TCK_GPIO_Port, TCK_Pin) == GPIO_PIN_RESET) {
                HAL_Delay(10); // Zmiana vTaskDelay na HAL_Delay
            }
            HAL_Delay(10);
        }

        switch(mode) {
            case 1: d1(accel->getX(), accel->getY(), accel->getZ()); break;
            case 2: d2(accel->getX(), accel->getY(), accel->getZ()); break;
            case 3: d3(accel->getX(), accel->getY(), accel->getZ(), gyroscope->getX(), gyroscope->getY(), gyroscope->getZ()); break;
            default: d1(accel->getX(), accel->getY(), accel->getZ()); break;
        }
        
        display1->clear_bufor();
        HAL_Delay(20); // Zmiana z vTaskDelay(20/portTICK_PERIOD_MS)
    }        
}