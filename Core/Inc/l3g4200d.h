#pragma once

//#include "driver/spi_master.h"
#include "sensor.h"
#ifndef MAIN_l3g4200d_H_
#define MAIN_l3g4200d_H_


#define REG_CTRL_REG1 	0x20
#define REG_CTRL_REG4 	0x23
#define REG_OUT_X_L   	0x28
class l3g4200d: public Sensor {

private:

	void write_reg(uint8_t cmd, uint8_t value) override;
	int16_t map(int16_t raw_val) override;
public:
	l3g4200d(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin);
	
	void init();
};

#endif 
