#pragma once
#include <stdbool.h>
#include "config.h"

// === AT Response Codes ===
#define AT_OK "AT+OK"
#define AT_ERROR "AT+ERR"
#define AT_PONG "AT+PONG"
#define AT_STM_READY "AT+STMREADY"

// === AT Command Strings ===
#define AT_CMD_SYNC "AT+SYNC"
#define AT_CMD_READY "AT+READY"
#define AT_CMD_DONE "AT+DONE"

#define AT_CMD_REBOOT "AT+REBOOT"
#define AT_CMD_POWEROFF "AT+POWEROFF"

#define AT_CMD "AT"
#define AT_PING "AT+PING"

#define AT_CMD_TX "AT+TX"
#define AT_CMD_CHRG "AT+CHRG"
#define AT_CMD_TEMP "AT+TEMP"

// Handler receives: base command (e.g. "AT+TX"), params string (e.g. "1", "0", "?" or NULL).
// Returns: true if command was handled.
typedef bool (*at_handler_t)(const char *base_cmd, const char *params);

void at_init(void);
void at_process(void);
void at_send(const char *response);
void at_register_handler(const char *base_cmd, at_handler_t handler);
bool at_app_ready_received(void);
bool at_app_done_received(void);
bool at_cmd_handler(const char *base_cmd, const char *params);
bool at_error_handler(const char *base_cmd, const char *params);
bool at_ping_handler(const char *base_cmd, const char *params);