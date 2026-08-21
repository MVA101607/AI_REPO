// onewire_ll.h — минимальный bit-bang 1-Wire на основе LL GPIO.
#pragma once
#include <stdbool.h>
#include <stdint.h>

void    onewire_init(void);
bool    onewire_reset(void);
void    onewire_write_bit(bool bit);
bool    onewire_read_bit(void);
void    onewire_write_byte(uint8_t b);
uint8_t onewire_read_byte(void);