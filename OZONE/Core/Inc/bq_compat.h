// bq_compat.h — слой совместимости Arduino-bq25792 → LL-bq25792.
// Минимальный набор API под Arduino-стиль, чтобы boot.c мог
// использовать те же сигнатуры (init/update/fsm_run/at_handler),
// что и в Arduino-исходниках.
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "bq25792.h"

#define BQ25792_I2C_ADDR 0x6B

// Регистры (адреса) — для команд уровня ниже.
#define BQ_REG_PART_INFO 0x48
#define BQ_REG_VBUS_ADC 0x35
#define BQ_REG_VBAT_ADC 0x3B
#define BQ_REG_VSYS_ADC 0x3D
#define BQ_REG_IBUS_ADC 0x31
#define BQ_REG_IBAT_ADC 0x33
#define BQ_REG_TDIE_ADC 0x41
#define BQ_REG_CHG_STAT0 0x1B
#define BQ_REG_CHG_STAT1 0x1C
#define BQ_REG_FAULT_STAT0 0x20
#define BQ_REG_FAULT_STAT1 0x21

// VBUS presence threshold (matches Arduino bq25792.cpp defines).
#define BQ_VBUS_PRESENT_MV 18000u
#define BQ_BAT_PRESENT_MV 6500u
#define BQ_SOC_FULL_PCT 100u
#define BQ_SOC_RECHG_PCT 90u

typedef enum
{
    BQ_STATE_INIT = 0,
    BQ_STATE_BATTERY_ONLY,
    BQ_STATE_CHARGING,
    BQ_STATE_CHARGE_DONE,
    BQ_STATE_ADAPTER_ONLY,
    BQ_STATE_FAULT,
} BqAppState;

typedef struct
{
    uint16_t vbat_mv;
    uint16_t vbus_mv;
    uint16_t vsys_mv;
    uint16_t ibus_ma;
    int16_t ibat_ma;
    int16_t tdie_raw;
    uint8_t chg_stat;
    uint8_t fault0;
    uint8_t fault1;
    uint8_t stat0;
    uint8_t soc_pct;
    bool vbus_present;
    bool bat_present;
} BatInfo;

int bq_init(void);
int bq_update(BatInfo *info);
void bq_fsm_run(BatInfo *info);
BqAppState bq_get_state(void);
int bq_reset_watchdog(void);
int bq_set_charge_enable(bool en);

// Software recovery I2C-bus: 9 manual SCL pulses + I2C1 re-init.
void bq_recover_i2c(void);

// Power-management API для I2C1.
//
// bq_i2c_disable() — снимает I2C1 (Disable + DeInit + clock disable) и
// переводит PB6/PB7 в INPUT PULLUP, чтобы не «висеть» на шине в shutdown.
// bq_i2c_reinit()  — повторная инициализация в AF+open-drain режим.
// Использовать, чтобы в boot-FSM можно было красиво усыплять просыпать
// периферию без копирования MX_I2C1_Init.
void bq_i2c_disable(void);
void bq_i2c_reinit(void);

// Снижение потребления без ship/shutdown. Выключает ADC/CHG, держит VSYS.
int bq_sleep(void);
int bq_wake(void);
int bq_ship_mode(void);
int bq_shutdown_mode(void);

// AT-хэндлер для AT+CHRG?...
bool bq_at_handler(const char *base_cmd, const char *params);