#pragma once
#include <string>


std::string processCommand(const std::string& command);

void setAccelPointer(void* accel);
void setgyroscopePointer(void* accel);

int getCurrentMode();  