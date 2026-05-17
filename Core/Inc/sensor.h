#pragma once

#pragma once
#include "spi_device.h"

struct SensorData3D {
    int16_t x;
    int16_t y;
    int16_t z;
};

class Sensor : public SpiDevice {
protected:
    SensorData3D data;
    uint8_t _id_reg;
    uint8_t _data_reg = 0x00; 
    virtual uint8_t read_reg(uint8_t reg);
    virtual int16_t map(int16_t raw_val) = 0;
public:
        
    // Tak to powinno wyglądać wewnątrz class Sensor w sensor.h:
    Sensor(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, uint8_t id_reg)
        : SpiDevice(hspi, cs_port, cs_pin), _id_reg(id_reg)
    {
        data = {0, 0, 0};
    }

    void update_raw_axes();
    virtual uint8_t read_id();
    int16_t getX() const { return data.x; }
    int16_t getY() const { return data.y; }
    int16_t getZ() const { return data.z; }
    virtual SensorData3D getData() const {return data; }
};