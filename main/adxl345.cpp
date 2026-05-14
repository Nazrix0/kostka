#include "adxl345.h"

#include <iostream>


adxl345::adxl345(spi_host_device_t host_id, int cs_pin) 
    : Sensor(host_id, cs_pin, 1000000, 3, 0, 0x00) 
{
}


void adxl345::init() {
    
    write_reg(REG_POWER_CTL, 0x00); 

    write_reg(REG_DATA_FORMAT,0x0B); 

    write_reg(REG_POWER_CTL, 0x08); 
    
}

int16_t adxl345::axis_to_0_15(int16_t v){
    int32_t scaled = ((int32_t)v + 512) * 15 / 1024;
    
    if (scaled < 0) scaled = 0;
    if (scaled > 15) scaled = 15;
    
    return (int16_t)scaled;
}
