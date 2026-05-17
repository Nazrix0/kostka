#include "adxl345.h"

//#include <iostream>


adxl345::adxl345(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin) 
    : Sensor(hspi, cs_port, cs_pin, 0x00) 
{
    this->_data_reg = REG_DATAX0;
}

uint8_t adxl345::read_bw_tate() {
    return read_reg(BW_RATE);
}

void adxl345::init() {
    
    write_reg(REG_POWER_CTL, 0x00); 

    write_reg(REG_DATA_FORMAT,0x0B); 

    write_reg(REG_POWER_CTL, 0x08); 
    
}

int16_t adxl345::map(int16_t v){
    int32_t scaled = ((int32_t)v + 512) * 15 / 1024;
    
    if (scaled < 0) scaled = 0;
    if (scaled > 15) scaled = 15;
    
    return (int16_t)scaled;
}
