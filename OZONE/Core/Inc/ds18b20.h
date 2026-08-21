#pragma once
#include <stdbool.h>
#include <stdint.h>

#define DS_READ_INTERVAL_MS 1000
#define DS_INVALID_TEMP     -127.0f

void  ds_init(void);
void  ds_update(void);
float ds_get_temp(void);
bool  ds_is_valid(void);

// AT handler: AT+TEMP?
bool  ds_at_handler(const char *base_cmd, const char *params);