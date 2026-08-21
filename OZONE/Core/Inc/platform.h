// platform.h — общие extern-объявления символов, реализованных в aicode.c.
// Подключается везде, где требуется обращение к i2c_ll_* / g_charger / g_bat
// / bat_info_update / calc_soc / delay_ms.
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "bq25792.h"

typedef struct
{
    uint16_t vbat_mv;
    uint16_t vbus_mv;
    uint16_t vsys_mv;
    uint16_t ibus_ma;
    int16_t  ibat_ma;
    int16_t  tdie_raw;
    int16_t  chg_stat;
    int16_t  vbus_stat;
    uint8_t  fault[2];
    uint8_t  soc_pct;
} bat_info_t;

extern volatile uint32_t g_tick_ms;
extern bq25792_t  g_charger;
extern bat_info_t g_bat;
extern BQ25792_RegisterMap_t regmap;

int   i2c_ll_write(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len);
int   i2c_ll_read (uint8_t addr, uint8_t reg, uint8_t *data,        uint8_t len);
void  delay_ms(uint32_t ms);
int   charger_init_2s(bq25792_t *dev);
int   bat_info_update(const bq25792_t *dev, bat_info_t *info);
uint8_t calc_soc(uint16_t vbat_mv);
