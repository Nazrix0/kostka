#include "l3g4200d.h"

//#include <iostream>


l3g4200d::l3g4200d(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin) 
    : Sensor(hspi, cs_port, cs_pin, 0x0F)
{
    this->_data_reg = REG_OUT_X_L;
}

void l3g4200d::write_reg(uint8_t cmd, uint8_t value) {
    uint8_t tx_data[2];
    tx_data[0] = cmd & 0x7F; // Zachowujemy Twoją maskę bitową dla tego czujnika
    tx_data[1] = value;

    // Ręczne sterowanie CS i wysyłka via HAL (korzysta z protected z SpiDevice)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_hspi, tx_data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
}

void l3g4200d::init() {

    write_reg(REG_CTRL_REG1, 0x0F);

    write_reg(REG_CTRL_REG4, 0x80); 

    //#include <iostream>
    //std::cout << "L3G4200D zainicjalizowany." << std::endl;
}


int16_t l3g4200d::map(int16_t raw_val) {
    int16_t step = raw_val / 3000; 
    
    int16_t mapped = 3 + step;
    

    if (mapped < 0) mapped = 0;
    if (mapped > 5) mapped = 5;
    
    mapped += (mapped - 3);
    
    return mapped;
}



    