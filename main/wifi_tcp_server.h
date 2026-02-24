#pragma once

void wifi_tcp_server_init(const char* ssid, const char* password, int port);
void wifi_tcp_server_task(void* pvParameters);
