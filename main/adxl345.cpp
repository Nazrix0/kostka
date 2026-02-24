#include "adxl345.h"

#include <iostream>

adxl345::adxl345(int cs_pin,spi_host_device_t host_id) {
    
    spi_device_interface_config_t devcfg = {};
        devcfg.clock_speed_hz = 1000000;
        devcfg.mode = 3;
        devcfg.spics_io_num = cs_pin; 
        devcfg.queue_size = 1;
        devcfg.flags = 0;
        devcfg.cs_ena_pretrans = 2;                    
    	devcfg.cs_ena_posttrans = 2;
		ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, &_spi_device));

}

uint8_t  adxl345::read_reg(uint8_t cmd){
	
	uint8_t tx_datai[2];
    tx_datai[0] = cmd | 0x80;
    tx_datai[1] = 0x00;
    uint8_t rx_datai[2] ;    

    spi_transaction_t t = {
        .length = 16,            
        .tx_buffer = &tx_datai,  
        .rx_buffer = &rx_datai
    };

    esp_err_t ret = spi_device_polling_transmit(_spi_device, &t);
    
    if (ret != ESP_OK) {
        std::cout <<("Blad transmisji SPI!\n");
        return 0;
    }

    return rx_datai[1];
}

void adxl345::write_reg(uint8_t cmd, uint8_t value) {
    uint8_t tx_data[2];
    tx_data[0] = cmd;
    tx_data[1] = value;

    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = tx_data;
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(_spi_device, &t));
}

void adxl345::init() {
    
    write_reg(REG_POWER_CTL, 0x00); 

    write_reg(REG_DATA_FORMAT,0x0B); 

    write_reg(REG_POWER_CTL, 0x08); 
    
    std::cout << "ADXL345 zainicjalizowany i wlaczony." << std::endl;
}

AccelData adxl345::read_all_axes() {
    AccelData data = {0, 0, 0};
    
    uint8_t tx_data[7] = {0};
    uint8_t rx_data[7] = {0};

    tx_data[0] = 0x80 | 0x40 | REG_DATAX0; 

    spi_transaction_t t = {};
    t.length = 7 * 8; 
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;


    if (spi_device_polling_transmit(_spi_device, &t) == ESP_OK) {
        int16_t x = (int16_t)(rx_data[1] | (rx_data[2] << 8));
        int16_t y = (int16_t)(rx_data[3] | (rx_data[4] << 8));
        int16_t z = (int16_t)(rx_data[5] | (rx_data[6] << 8));

        data.x = axis_to_0_15(x);
        data.y = axis_to_0_15(y);
        data.z = axis_to_0_15(z);
    } else {
        std::cout << "Blad odczytu osi!" << std::endl;
    }

    return data;
}

uint8_t adxl345::read_id(){
    return read_reg(BW_RATE);
}

uint8_t adxl345::read_bw_tate(){
    return read_reg(REG_WHO_AM_I);
}

int16_t adxl345::axis_to_0_15(int16_t v){
    int32_t scaled = ((int32_t)v + 512) * 15 / 1024;
    
    if (scaled < 0) scaled = 0;
    if (scaled > 15) scaled = 15;
    
    return (int16_t)scaled;
}
