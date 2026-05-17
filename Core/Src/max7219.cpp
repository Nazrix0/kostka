#include "max7219.h"
#include <cstring>


max7219::max7219(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin) 
    : SpiDevice(hspi, cs_port, cs_pin) 
{
    max7219_init();
}

void max7219::max7219_init(){
	     
        write_reg( SHUTDOWN_REG,     0x01);//Wyjście z trybu uśpienia
        write_reg( DISPLAY_TEST_REG, 0x00);// Wyłączenie trybu 
        write_reg( SCAN_LIMIT_REG,   0x07);// Ustawienie liczby skanowanych wierszy 
        write_reg( DECODE_MODE_REG,  0x00);// Brak dekodowania BCD 
        write_reg( INTENSITY_REG,    0x01); // Ustawienie maksymalnej jasności


	
}
max7219::~max7219() {

}

void max7219::write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx_data[MAX7219_DEVICES * 2];

    for (int dev = 0; dev < MAX7219_DEVICES; dev++) {
        tx_data[dev * 2] = reg;
        tx_data[dev * 2 + 1] = value;
    }

    // STM32 HAL: Opuszczenie CS, Wysyłka (wielkość w bajtach = MAX7219_DEVICES * 2), Podniesienie CS
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_hspi, tx_data, MAX7219_DEVICES * 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
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

    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_hspi, tx_data, MAX7219_DEVICES * 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
}

void max7219::update() {
    uint8_t tx_data[MAX7219_DEVICES * 2];

    for (int row = 0; row < 8; row++) {
        for (int dev = 0; dev < MAX7219_DEVICES; dev++) {
            int index = (MAX7219_DEVICES - 1 - dev) * 2;
            tx_data[index] = row + 1; 
            tx_data[index + 1] = _frame_buffer[dev][row]; 
        }

        HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(_hspi, tx_data, MAX7219_DEVICES * 2, HAL_MAX_DELAY);
        HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
    }
}