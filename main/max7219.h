#pragma once

#include "driver/spi_master.h"

#define DECODE_MODE_REG     0x09
#define INTENSITY_REG       0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F
#define MAX7219_DEVICES 6
#define NOOP_REG       0x00

#ifndef MAIN_MAX7219_H_
#define MAIN_MAX7219_H_

class max7219 {
private:
	spi_device_handle_t _spi_device;
    void write_reg(uint8_t reg, uint8_t value);
public:

	max7219(int cs_pin,spi_host_device_t host_id);
	virtual ~max7219();
	void set_row(uint8_t device, uint8_t row_index, uint8_t value);
	void set_col(uint8_t device, uint8_t col_index, uint8_t value);
	void set_intensity(uint8_t col_index);
	void max7219_init();
	void clear_single_display(uint8_t display_num);
	void clear();
	void write_reg_device(uint8_t device,uint8_t reg,uint8_t value);
	 
};

#endif