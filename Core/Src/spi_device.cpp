#include "spi_device.h"

SpiDevice::SpiDevice(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin) 
    : _hspi(hspi), _cs_port(cs_port), _cs_pin(cs_pin) {
    // Na starcie ustawiamy CS w stan wysoki (urządzenie nieaktywne)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET); 
}

SpiDevice::~SpiDevice() {
}

void SpiDevice::write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = { reg, value };
    
    // Opuszczenie CS (początek komunikacji)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    
    // Transmisja przez STM32 HAL
    HAL_SPI_Transmit(_hspi, tx_data, 2, HAL_MAX_DELAY);
    
    // Podniesienie CS (koniec komunikacji)
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
}