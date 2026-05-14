#include "sensor.h"
#include <iostream>


void Sensor::update_raw_axes() {
    uint8_t tx_data[7] = {0};
    uint8_t rx_data[7] = {0};
    
    tx_data[0] = 0x80 | 0x40 | _data_reg; 

    spi_transaction_t t = {};
    t.length = 7 * 8; 
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;


    if (spi_device_polling_transmit(_spi_device, &t) == ESP_OK) {
        data.x = (int16_t)(rx_data[1] | (rx_data[2] << 8));
        data.y = (int16_t)(rx_data[3] | (rx_data[4] << 8));
        data.z = (int16_t)(rx_data[5] | (rx_data[6] << 8));
    }
}
uint8_t Sensor::read_id(){
    return read_reg(_id_reg);
}


uint8_t  Sensor::read_reg(uint8_t cmd){
	
	uint8_t tx_data[2];
    tx_data[0] = cmd | 0x80;
    tx_data[1] = 0x00;
    uint8_t rx_data[2] ;    

    spi_transaction_t t = {
        .length = 16,            
        .tx_buffer = &tx_data,  
        .rx_buffer = &rx_data
    };

    if (spi_device_polling_transmit(_spi_device, &t) != ESP_OK) {
        std::cout << "Blad transmisji SPI!\n";
        return 0;
    }

    return rx_data[1];
}