#include "l3g4200d.h"

#include <iostream>

l3g4200d::l3g4200d(int cs_pin,spi_host_device_t host_id) {
    
    spi_device_interface_config_t devcfg = {};
        devcfg.clock_speed_hz = 1000000;
        devcfg.mode = 3;
        devcfg.spics_io_num = cs_pin; 
        devcfg.queue_size = 1,
        devcfg.flags = 0;
        devcfg.cs_ena_pretrans = 2;                    
    	devcfg.cs_ena_posttrans = 2;
		ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, &_spi_device));

}

uint8_t  l3g4200d::read_reg(uint8_t cmd){
	
	uint8_t tx_datai[2];
    tx_datai[0] = cmd | 0x80;
    tx_datai[1] = 0x00;
    uint8_t rx_datai[2] ;    

    spi_transaction_t t = {
        .length = 16,            // Długość transakcji w bitach (8 bitów = 1 bajt)
        .tx_buffer = &tx_datai,  // Wskaźnik na dane do wysłania
        .rx_buffer = &rx_datai
    };

    // Wykonaj transakcję z urządzeniem 2
    esp_err_t ret = spi_device_polling_transmit(_spi_device, &t);
    
    if (ret != ESP_OK) {
        std::cout <<("Blad transmisji SPI!\n");
        return 0;
    }

    return rx_datai[1];
}

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

GyroData l3g4200d::read_all_axes() {
    GyroData data = {0, 0, 0};
    
    uint8_t tx_data[7] = {0};
    uint8_t rx_data[7] = {0};


    tx_data[0] = 0x80 | 0x40 | REG_OUT_X_L;

    spi_transaction_t t = {};
    t.length = 7 * 8; 
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;

    esp_err_t ret = spi_device_polling_transmit(_spi_device, &t);

    if (ret == ESP_OK) {

        data.x = (int16_t)(rx_data[1] | (rx_data[2] << 8));
        data.y = (int16_t)(rx_data[3] | (rx_data[4] << 8));
        data.z = (int16_t)(rx_data[5] | (rx_data[6] << 8));
    }

    return data;
}

uint8_t l3g4200d::read_id(){
        return read_reg(REG_WHO_AM_Ii);
}


    