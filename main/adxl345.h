#pragma once

#include "driver/spi_master.h"

#ifndef MAIN_ADXL345_H_
#define MAIN_ADXL345_H_


struct AccelData {
    int16_t x;
    int16_t y;
    int16_t z;
};

class adxl345 {
#define REG_WHO_AM_I	0x00
#define	BW_RATE 		0x2C 
#define REG_POWER_CTL 	0x2D
#define REG_DATAX0     	0x32
#define REG_DATA_FORMAT 0x31
private:
	spi_device_handle_t _spi_device;
	uint8_t read_reg(uint8_t reg);
	void write_reg(uint8_t reg, uint8_t value);
public:
	adxl345(int cs_pin,spi_host_device_t host_id);
	void init(); 
	AccelData read_all_axes();
	uint8_t read_id();
	uint8_t read_bw_tate();
	int16_t axis_to_0_15(int16_t v);
};

#endif 
