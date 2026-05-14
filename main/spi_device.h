#pragma once

#include "driver/spi_master.h"

class SpiDevice {
protected:
    spi_device_handle_t _spi_device;

public:
    SpiDevice(spi_host_device_t host_id, int cs_pin, int clock_speed_hz, uint8_t mode, uint32_t flags = 0);
    virtual ~SpiDevice();

    virtual void write_reg(uint8_t reg, uint8_t value);
};