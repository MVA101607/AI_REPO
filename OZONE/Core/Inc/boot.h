#pragma once

typedef enum
{
    INIT = 0,
    OPI_STARTING,
    SDR_STARTING,
    THERM_INIT,
    WAIT_OPI_BOOT,
    UART_OPI_START,
    WAIT_APP_READY,
    RUNNING,
    POWEROFF_PENDING,
    SHUTDOWN,
    SLEEP_PENDING,
    SLEEP,
} BootState;

void boot_init(void);
void boot_update(void);
BootState boot_get_state(void);