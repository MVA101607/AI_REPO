#pragma once

// === UART Configuration ===
#define UART_OPI_BAUD 115200   // Скорость UART к OrangePI
#define UART_RS485_BAUD 115200 // Скорость UART RS-485 (не используется)

// === Timing ===
#define OPI_BOOT_DELAY_MS 10000  // Задержка на загрузку OPI (мс)
#define OPI_BOOT_RESTART_MS 5000 // Задержка перезагрузки OPI (мс)
#define OPI_BOOT_FAIL_MS 50000   // Задержка при неудачной загрузке OPI (мс)
#define OPI_POWEROFF_MS 10000    // Задержка при выключении OPI (мс)
#define BQ_SHIP_DELAY_MS 30000   // Задержка перед выключением чарджера (мс)

// === I2C / Charger ===
#define BQ_POLL_INTERVAL_MS 1000 // Опрос чарджера каждую секунду

// === Power Monitor ===
#define VBUS_MIN_THRESHOLD_MV 9000 // Порог отключения внешнего питания (мВ)

// === Power Button (PIN_EN_FAN, active LOW = shutdown) ===
#define PWR_BTN_DEBOUNCE_MS 50 // Дебаунс кнопки питания (мс)
#define PWR_BTN_HOLD_MS 100    // Удержание для подтверждения выключения (мс)

// === AT Command Protocol ===
#define AT_LINE_BUF_SIZE 128 // Максимальная длина AT-команды
#define AT_MAX_HANDLERS 8    // Максимальное кол-во обработчиков команд