#include "l3g4200d.h"

#include <iostream>


l3g4200d::l3g4200d(spi_host_device_t host_id, int cs_pin) 
    : Sensor(host_id, cs_pin, 1000000, 3, 0, 0x0F){}

void l3g4200d::write_reg(uint8_t cmd, uint8_t value) {
    uint8_t tx_data[2];
    tx_data[0] = cmd & 0x7F; 
    tx_data[1] = value;

    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = tx_data;
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(_spi_device, &t));
}

void l3g4200d::init() {

    write_reg(REG_CTRL_REG1, 0x0F);

    write_reg(REG_CTRL_REG4, 0x80); 

    std::cout << "L3G4200D zainicjalizowany." << std::endl;
}




    