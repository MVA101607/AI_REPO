#pragma once
#include <stdbool.h>

// Initialize TX (amplifier + boost) control.
void tx_control_init(void);

// Enable amplifier + boost converter (EN_VPA=HIGH, BSTDIS=LOW).
bool tx_enable(void);

// Disable amplifier + boost converter (EN_VPA=LOW, BSTDIS=HIGH).
bool tx_disable(void);

// Returns true if TX is currently enabled.
bool tx_is_enabled(void);

// AT command handler for TX commands: AT+TX=1, AT+TX=0, AT+TX?
bool tx_at_handler(const char *base_cmd, const char *params);

// Power-loss lockout (выключаем TX при отсутствии VBUS).
void tx_set_power_lockout(bool locked);
bool tx_is_locked(void);