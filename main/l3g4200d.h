#pragma once

#include "driver/spi_master.h"

#ifndef MAIN_l3g4200d_H_
#define MAIN_l3g4200d_H_


struct GyroData {
    int16_t x;
    int16_t y;
    int16_t z;
};
class l3g4200d {
#define REG_WHO_AM_Ii	0x0F
#define REG_CTRL_REG1 	0x20
#define REG_CTRL_REG4 	0x23
#define REG_OUT_X_L   	0x28
private:
	spi_device_handle_t _spi_device;
	uint8_t read_reg(uint8_t reg);
	void write_reg(uint8_t cmd, uint8_t value);
	
public:
	l3g4200d(int cs_pin,spi_host_device_t host_id);
	void init();

    
	GyroData read_all_axes();
	uint8_t read_id();
};

#endif 
