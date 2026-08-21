// serial_at_ll.h — Hardware Abstraction Layer для USART2 (AT-канал к OrangePi).
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void serial_at_init(void);
int serial_at_available(void);
char serial_at_read(void);
void serial_at_write(char c);
void serial_at_print(const char *s);

// Внутренний API: используется в USART2_IRQHandler.
void serial_at_push_byte(char c);

// Power-management API.
//
// serial_at_disable() — полностью снимает USART2: выключает периферию,
// запрещает RXNE IRQ, отключает NVIC и переводит PA2/PA3 в FLOATING/HI-Z,
// чтобы они не подтекали током на одноплатник (OrangePi питается через
// data-lines, поэтому любой активный UART-драйвер мешает его выключению).
void serial_at_disable(void);

// serial_at_reinit() — повторная инициализация USART2 «с нуля» и запуск RXNE
// IRQ с NVIC. Использовать после disable() для возврата AT-канала.
void serial_at_reinit(void);