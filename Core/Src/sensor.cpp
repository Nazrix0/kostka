#include "sensor.h"
//#include <iostream>

 
void Sensor::update_raw_axes() {
    uint8_t tx_data[7] = {0};
    uint8_t rx_data[7] = {0};
    
    // Ustawienie bitu odczytu i bitu autoinkrementacji (MSB w zależności od czujnika)
    tx_data[0] = 0x80 | 0x40 | _data_reg; 

    // 1. Aktywacja czujnika (CS w stan niski)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);

    // 2. Transmisja i jednoczesny odbiór przez bibliotekę HAL
    if (HAL_SPI_TransmitReceive(_hspi, tx_data, rx_data, 7, HAL_MAX_DELAY) == HAL_OK) {
        // Złożenie danych 16-bitowych z odebranych bajtów
        data.x = (int16_t)(rx_data[1] | (rx_data[2] << 8));
        data.y = (int16_t)(rx_data[3] | (rx_data[4] << 8));
        data.z = (int16_t)(rx_data[5] | (rx_data[6] << 8));
        
        // Mapowanie wartości
        data.x = map(data.x);
        data.y = map(data.y);   
        data.z = map(data.z);
    } else {
        //#include <iostream>
        //std::cout << "Blad odczytu osi SPI!\n";
    }

    // 3. Dezaktywacja czujnika (CS w stan wysoki)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
}

uint8_t Sensor::read_id() {
    return read_reg(_id_reg);
}

uint8_t Sensor::read_reg(uint8_t cmd) {
    uint8_t tx_data[2];
    tx_data[0] = cmd | 0x80; // Ustawienie najstarszego bitu oznaczającego odczyt (w wielu czujnikach SPI)
    tx_data[1] = 0x00;       // Pusty bajt, by wygenerować taktowanie zegara
    uint8_t rx_data[2] = {0};    

    // 1. Aktywacja czujnika (CS na LOW)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);

    // 2. Wymiana danych
    if (HAL_SPI_TransmitReceive(_hspi, tx_data, rx_data, 2, HAL_MAX_DELAY) != HAL_OK) {
        //std::cout << "Blad transmisji SPI w read_reg!\n";
        
        // W przypadku błędu podnosimy CS, żeby nie zawiesić magistrali
        HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
        return 0;
    }

    // 3. Dezaktywacja czujnika (CS na HIGH)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);

    // Zwracamy drugi bajt, w którym znajduje się odpowiedź czujnika na nasze żądanie
    return rx_data[1];
}