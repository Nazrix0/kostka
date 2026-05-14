#include "spi_device.h"

SpiDevice::SpiDevice(spi_host_device_t host_id, int cs_pin, int clock_speed_hz, uint8_t mode, uint32_t flags) {
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = clock_speed_hz;
    devcfg.mode = mode;
    devcfg.spics_io_num = cs_pin; 
    devcfg.queue_size = 1;
    devcfg.flags = flags;
    devcfg.cs_ena_pretrans = 2;                    
    devcfg.cs_ena_posttrans = 2;
    
    ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, &_spi_device));
}

SpiDevice::~SpiDevice() {
    spi_bus_remove_device(_spi_device);
}


void SpiDevice::write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = { reg, value };
    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = tx_data;
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(_spi_device, &t));
}