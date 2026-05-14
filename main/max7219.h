#pragma once

#include "driver/spi_master.h"
#include "spi_device.h"

#define DECODE_MODE_REG     0x09
#define INTENSITY_REG       0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F
#define MAX7219_DEVICES 6
#define NOOP_REG       0x00

#ifndef MAIN_MAX7219_H_
#define MAIN_MAX7219_H_

class max7219: public SpiDevice {
private:
	uint8_t _frame_buffer[MAX7219_DEVICES][8];
	void max7219_init();
	void write_reg_device(uint8_t device,uint8_t reg,uint8_t value);
public:
	max7219(spi_host_device_t host_id, int cs_pin);
	virtual ~max7219();
	void set_intensity(uint8_t col_index);

	void set_row(uint8_t device, uint8_t row_index, uint8_t value);
	void set_col(uint8_t device, uint8_t col_index, uint8_t value);
	void side_displays_col(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw = false, bool reverse_dir = false);
    void side_displays_row(int dx, int dy, int wall_pos, uint8_t display_num, bool invert_hw = false, bool reverse_dir = false);
	void set_display(uint8_t device);
	void clear_bufor();

	void update();
};

#endif