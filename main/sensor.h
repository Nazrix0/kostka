#pragma once

#include "driver/spi_master.h"
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
public:
    
    Sensor(spi_host_device_t host_id, int cs_pin, int clock_speed_hz, uint8_t mode, uint32_t flags, uint8_t id_reg)
        : SpiDevice(host_id, cs_pin, clock_speed_hz, mode, flags), _id_reg(id_reg)
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