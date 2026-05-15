#include "max7219.h"
#include <cstring>


max7219::max7219(spi_host_device_t host_id, int cs_pin) : SpiDevice(host_id, cs_pin, 10000000, 0, SPI_DEVICE_HALFDUPLEX) 
{
    max7219_init();
}

void max7219::max7219_init(){
	     
        write_reg( SHUTDOWN_REG,     0x01);//Wyjście z trybu uśpienia
        write_reg( DISPLAY_TEST_REG, 0x00);// Wyłączenie trybu 
        write_reg( SCAN_LIMIT_REG,   0x07);// Ustawienie liczby skanowanych wierszy 
        write_reg( DECODE_MODE_REG,  0x00);// Brak dekodowania BCD 
        write_reg( INTENSITY_REG,    0x0F); // Ustawienie maksymalnej jasności


	
}
max7219::~max7219() {

}

void max7219::set_intensity(uint8_t intensity){
	if (intensity > 15) intensity = 15;
    write_reg(INTENSITY_REG, intensity);
}

void max7219::clear_bufor() {
    memset(_frame_buffer, 0, sizeof(_frame_buffer));
}

void max7219::set_row(uint8_t device, uint8_t row_index, uint8_t value) {
    if (device >= MAX7219_DEVICES || row_index > 7) return;
    static const uint8_t masks[16] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
    _frame_buffer[device][row_index] = (value < 16) ? masks[value] : 0x00;
}
void max7219::set_col(uint8_t device, uint8_t col_index, uint8_t value) {
    if (device >= MAX7219_DEVICES || col_index > 7 || value > 16) return;

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t bit = 0;

        if (value <= 8) {
            if (row < value) bit = 1;
        } else {
            if (row >= value - 8) bit = 1;
        }
        if (bit) {
            _frame_buffer[device][row] |=  (1 << col_index);  // Ustaw bit na 1
        } else {
            _frame_buffer[device][row] &= ~(1 << col_index);  // Ustaw bit na 0
        }
    }
}

void max7219::side_displays_col(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw, bool reverse_dir) {
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
        set_col(display_num, col, hw_value);
    }
}

void max7219::side_displays_row(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw, bool reverse_dir) {
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
        
        if (invert_hw && width > 0 && width < 8) {
            width = 16 - width; 
        }
        set_row(display_num, row, width);
    }
}

void max7219::set_display(uint8_t device) {
    if (device >= MAX7219_DEVICES) return;

    for (int row = 0; row < 8; row++) {

        set_row(device, row, 8);
    }
}

void max7219::write_reg_device(uint8_t device, uint8_t reg, uint8_t value){
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

void max7219::update() {
    uint8_t tx_data[MAX7219_DEVICES * 2];

    // MAX7219 ma 8 rejestrów danych (1-8). Musimy wysłać każdy rząd dla każdego urządzenia.
    for (int row = 0; row < 8; row++) {
        for (int dev = 0; dev < MAX7219_DEVICES; dev++) {
            // W kaskadzie ostatnie urządzenie w kodzie jest pierwszym w łańcuchu fizycznym
            int index = (MAX7219_DEVICES - 1 - dev) * 2;
            tx_data[index] = row + 1; // Adres rejestru (1-8)
            tx_data[index + 1] = _frame_buffer[dev][row]; // Dane z bufora
        }

        spi_transaction_t t = {};
        t.tx_buffer = tx_data;
        t.length = MAX7219_DEVICES * 16;
        spi_device_polling_transmit(_spi_device, &t);
    }
}