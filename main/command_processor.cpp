#include "command_processor.h"
#include "driver/gpio.h"
#include "adxl345.h"
#include "l3g4200d.h"
#include <cstdlib>
#include <sstream>

static int led_pin = GPIO_NUM_2;
static adxl345* accel_ptr = nullptr;
static l3g4200d* gyroscope_ptr = nullptr;

int current_mode = 1;  


void setAccelPointer(void* accel) {
    accel_ptr = (adxl345*)accel;
}

void setgyroscopePointer(void* accel) {
    gyroscope_ptr = (l3g4200d*)accel;
}

int getCurrentMode() {
    return current_mode;
}

std::string processCommand(const std::string& command) {


     if (command == "SPI_ADXL345:GET") {
        if (accel_ptr != nullptr) {
            AccelData reading = accel_ptr->read_all_axes();
            GyroData reading1 = gyroscope_ptr->read_all_axes();
            std::ostringstream oss;
            oss << "x: " << reading.x << " y: " << reading.y << " z: " << reading.z
            << " xg: " << reading1.x << " yg: " << reading1.y << " zg: " << reading1.z; 
            return oss.str();
        } else {
            std::ostringstream oss;
            oss << "SPI_ADXL345:ERR";
            return oss.str();
        }
    }
    else if (command == "MOD:1 SET") {
        current_mode = 1;
        return "MODE:1 SET";
    }
    else if (command == "MOD:2 SET") {
        current_mode = 2;
        return "MODE:2 SET";
    }
    else if (command == "MOD:3 SET") {
        current_mode = 3;
        return "MODE:3 SET";
    }
    else if (command == "MOD:?") {
        return "MODE:" + std::to_string(current_mode);
    }
    
    return "ACK:" + command;
}
