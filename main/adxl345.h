#pragma once

#include "driver/spi_master.h"
#include "sensor.h"
#ifndef MAIN_ADXL345_H_
#define MAIN_ADXL345_H_



class adxl345:public Sensor {
#define	BW_RATE 		0x2C 
#define REG_POWER_CTL 	0x2D
#define REG_DATAX0     	0x32
#define REG_DATA_FORMAT 0x31
private:
	int16_t map(int16_t v);
public:
	adxl345(spi_host_device_t host_id, int cs_pin);
	void init(); 
	uint8_t read_bw_tate();
	
};

#endif 
