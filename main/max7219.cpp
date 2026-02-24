#include "max7219.h"
#include <cstring>


max7219::max7219(int cs_pin,spi_host_device_t host_id) {
    
        spi_device_interface_config_t devcfg={};
        devcfg.clock_speed_hz = 10000000;  
        devcfg.mode = 0;                 
        devcfg.spics_io_num = cs_pin;
        devcfg.queue_size = 1;
        devcfg.flags = SPI_DEVICE_HALFDUPLEX;
        devcfg.pre_cb = NULL;
        devcfg.post_cb = NULL;
		ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, &_spi_device));
		
		max7219_init();
    
}


max7219::~max7219() {
	
}

void max7219::write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = { reg, value };
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    
    t.tx_buffer = tx_data;
    t.length = 16;

    ESP_ERROR_CHECK(spi_device_polling_transmit(_spi_device, &t));
}

void max7219::max7219_init(){
	    for (uint8_t d = 0; d < MAX7219_DEVICES; d++) {
        write_reg_device(d, SHUTDOWN_REG,     0x01);//Wyjście z trybu uśpienia
        write_reg_device(d, DISPLAY_TEST_REG, 0x00);// Wyłączenie trybu 
        write_reg_device(d, SCAN_LIMIT_REG,   0x07);// Ustawienie liczby skanowanych wierszy 
        write_reg_device(d, DECODE_MODE_REG,  0x00);// Brak dekodowania BCD 
        write_reg_device(d, INTENSITY_REG,    0x15); // Ustawienie maksymalnej jasności
    }	
    
	    clear();                           
		clear();
	
}

void max7219::clear(){
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t device = 0; device < MAX7219_DEVICES; device++) {
            write_reg_device(device, row + 1, 0x00);
        }
    }
}

void max7219::set_intensity(uint8_t intensity){
	if (intensity > 15) intensity = 15;
    write_reg(INTENSITY_REG, intensity);
}
	
void max7219::set_row(uint8_t device, uint8_t row_index, uint8_t value){
    if (row_index > 7) return;
    if (value >= 16) return;
    uint8_t asd[16] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
	write_reg_device(device,row_index + 1, asd[value]);
	
}

uint8_t row_buffer[8] = {0};
void max7219::set_col(uint8_t device, uint8_t col_index, uint8_t value) {
    if (col_index > 7 || value > 16) return;

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t bit = 0;

        if (value <= 8) {
            if (row < value) bit = 1;
        } else {
            if (row >= value - 8) bit = 1;
        }

        if (bit)
            row_buffer[row] |=  (1 << col_index);
        else
            row_buffer[row] &= ~(1 << col_index);

        write_reg_device(device,row + 1,row_buffer[row]);
    }
}

void max7219::write_reg_device(uint8_t device,uint8_t reg,uint8_t value){
    if (device >= MAX7219_DEVICES) return;

    uint8_t tx_data[MAX7219_DEVICES * 2];

    for (int i = 0; i < MAX7219_DEVICES; i++) {
        tx_data[i * 2]     = NOOP_REG;
        tx_data[i * 2 + 1] = 0x00;
    }

    int index = (MAX7219_DEVICES - 1 - device) * 2;
    tx_data[index]     = reg;
    tx_data[index + 1] = value;

    spi_transaction_t t{};
    t.tx_buffer = tx_data;
    t.length    = MAX7219_DEVICES * 16;

    ESP_ERROR_CHECK(spi_device_polling_transmit(_spi_device, &t));
}

void max7219::clear_single_display(uint8_t display_num) {
    for (uint8_t row = 0; row < 8; row++) {
        set_row(display_num, row, 0); 
    }
}
