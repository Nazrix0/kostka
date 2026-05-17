#pragma once
#include "main.h" // Zamiast driver/spi_master.h, ładujemy biblioteki STM32

class SpiDevice {
protected:
    SPI_HandleTypeDef* _hspi;
    GPIO_TypeDef* _cs_port;
    uint16_t _cs_pin;
    
    virtual void write_reg(uint8_t reg, uint8_t value);
public:
    // Konstruktor przyjmuje uchwyt SPI oraz port i pin dla CS
    SpiDevice(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin);
    virtual ~SpiDevice();
};