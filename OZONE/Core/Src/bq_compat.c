// bq_compat.c — Compatibility layer (Arduino-style API → LL API).
// Цель: сохранить Arduino-сигнатуры (bq_init/update/fsm_run/at_handler)
// для boot.c, при этом использовать LL bq25792-драйвер из _original/.
//
// КЛЮЧЕВОЕ: bq_fsm_run НЕ трогает GPIO/периферию — только переходы
// между состояниями на основе информации в BatInfo. Управление питанием —
// зона ответственности tx_control / boot-FSM.

#include "bq_compat.h"
#include "at_commands.h"
#include "main.h"
#include "platform.h" /* extern-объявления реалий aicode.c */
/* bq25792_regs.h подтянется транзитивно через bq25792.h — повторное включение запрещено */
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_ll_i2c.h"

static BqAppState _state = BQ_STATE_INIT;

// Локальный mirror — нужен для AT-обработчика, который запрашивает
// последние значения вне контекста boot_update().
static struct
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
    uint8_t soc_pct;
} _last_info;

int bq_init(void)
{
    if (g_charger.i2c_write == NULL || g_charger.i2c_read == NULL)
    {
        g_charger.i2c_write = i2c_ll_write;
        g_charger.i2c_read = i2c_ll_read;
    }

    int ret = charger_init_2s(&g_charger);
    if (ret == 0)
    {
        _state = BQ_STATE_BATTERY_ONLY;
    }
    return ret;
}

// ===== Чтение ADC + наполнение BatInfo =====
int bq_update(BatInfo *info)
{
    if (info == NULL)
        return -1;
    (void)memset(info, 0, sizeof(*info));

    int err = 0;
    uint16_t vbat_mv = 0, vbus_mv = 0, vsys_mv = 0;
    uint16_t ibus_ma = 0;
    int16_t ibat_ma = 0;
    int16_t tdie_raw = 0;

    err |= bq25792_read_vbat(&g_charger, &vbat_mv);
    err |= bq25792_read_vbus(&g_charger, &vbus_mv);
    err |= bq25792_read_vsys(&g_charger, &vsys_mv);
    err |= bq25792_read_ibus(&g_charger, &ibus_ma);
    err |= bq25792_read_ibat(&g_charger, &ibat_ma);
    err |= bq25792_read_tdie(&g_charger, &tdie_raw);

    uint8_t stat[5] = {0};
    err |= bq25792_read_chg_status(&g_charger, stat);
    uint8_t fault[2] = {0};
    err |= bq25792_read_fault_status(&g_charger, fault);

    if (err != 0)
    {
        // Все нули при ошибке.
        info->vbat_mv = info->vbus_mv = info->vsys_mv = 0;
        info->ibus_ma = 0;
        info->ibat_ma = 0;
        info->tdie_raw = 0;
        return err;
    }

    info->vbat_mv = vbat_mv;
    info->vbus_mv = vbus_mv;
    info->vsys_mv = vsys_mv;
    info->ibus_ma = ibus_ma;
    info->ibat_ma = ibat_ma;
    info->tdie_raw = tdie_raw;
    info->chg_stat = (uint8_t)bq25792_decode_chg_stat(stat[1]);
    info->stat0 = stat[0];
    info->fault0 = fault[0];
    info->fault1 = fault[1];
    info->soc_pct = calc_soc(vbat_mv);
    info->vbus_present = (vbus_mv > BQ_VBUS_PRESENT_MV);
    info->bat_present = (vbat_mv > BQ_BAT_PRESENT_MV);

    // Mirror в локальную структуру — нужна для AT-обработчика.
    _last_info.vbat_mv = vbat_mv;
    _last_info.vbus_mv = vbus_mv;
    _last_info.vsys_mv = vsys_mv;
    _last_info.ibus_ma = ibus_ma;
    _last_info.ibat_ma = ibat_ma;
    _last_info.tdie_raw = tdie_raw;
    _last_info.chg_stat = info->chg_stat;
    _last_info.fault0 = fault[0];
    _last_info.fault1 = fault[1];
    _last_info.soc_pct = info->soc_pct;

    return 0;
}

// Локальный MX_I2C1_Init удалён — используется публичный из main.c
// (CubeMX-исходник правды в одном месте).

// ===== Power-management для I2C1 =====
//
// bq_i2c_disable() — полностью снимает I2C1 и переводит PB6/PB7 в INPUT
// (pull-up), чтобы шина не висела активным драйвером в shutdown.
// bq_i2c_reinit()  — повторная инициализация в AF+open-drain,
// фактически вызывает MX_I2C1_Init() из main.c.

void bq_i2c_disable(void)
{
    LL_I2C_Disable(I2C1);
    LL_I2C_DeInit(I2C1);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    gpio.Mode = LL_GPIO_MODE_INPUT;
    gpio.Pull = LL_GPIO_PULL_UP;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOB, &gpio);
}

void bq_i2c_reinit(void)
{
    // Делегируем в публичный MX_I2C1_Init (main.c) — единый источник.
    MX_I2C1_Init();
}

// ===== Software I2C bus recovery =====
// 9 импульсов SCL (как требует I2C-spec) для освобождения подвисшего slave.
// После — переинициализация I2C1 (PE disable/enable).
void bq_recover_i2c(void)
{
    LL_GPIO_InitTypeDef gpio = {0};
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    // PB6 (SCL) → open-drain OUTPUT
    gpio.Pin = LL_GPIO_PIN_6;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    LL_GPIO_Init(GPIOB, &gpio);

    // PB7 (SDA) → INPUT + PULLUP
    gpio.Pin = LL_GPIO_PIN_7;
    gpio.Mode = LL_GPIO_MODE_INPUT;
    gpio.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &gpio);

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
    for (int i = 0; i < 32; i++)
    {
        // Минимум 5 µs LOW/HIGH; на 64 МГц ~5 µs = 320 циклов NOP.
        for (volatile uint32_t d = 0; d < 320; d++)
        {
            __NOP();
        }
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
        for (volatile uint32_t d = 0; d < 320; d++)
        {
            __NOP();
        }
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
    }
    for (volatile uint32_t d = 0; d < 1500; d++)
    {
        __NOP();
    }

    bq_i2c_reinit();

    // Возвращаем PB6/PB7 в AF4 (I2C1).
    // gpio.Pin = LL_GPIO_PIN_6;
    // gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    // gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    // gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    // LL_GPIO_Init(GPIOB, &gpio);

    // gpio.Pin = LL_GPIO_PIN_7;
    // gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    // gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    // gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    // LL_GPIO_Init(GPIOB, &gpio);

    // LL_I2C_Disable(I2C1);
    // for (volatile uint32_t d = 0; d < 1500; d++)
    // {
    //     __NOP();
    // }
    // LL_I2C_Enable(I2C1);
}

void bq_fsm_run(BatInfo *info)
{
    if (info == NULL)
        return;

    // Если есть аппаратные fault — переходим в FAULT.
    const bool has_fault = (info->fault0 & 0xFCu) || (info->fault1 & 0xC0u);
    if (has_fault && _state != BQ_STATE_INIT)
    {
        _state = BQ_STATE_FAULT;
    }

    switch (_state)
    {
    case BQ_STATE_INIT:
        _state = info->vbus_present ? BQ_STATE_ADAPTER_ONLY
                                    : BQ_STATE_BATTERY_ONLY;
        break;

    case BQ_STATE_BATTERY_ONLY:
        if (info->vbus_present)
        {
            _state = BQ_STATE_CHARGING;
        }
        break;

    case BQ_STATE_CHARGING:
        if (info->soc_pct >= BQ_SOC_FULL_PCT)
        {
            bq25792_set_charge_enable(&g_charger, false);
            _state = BQ_STATE_CHARGE_DONE;
        }
        if (!info->vbus_present)
        {
            _state = BQ_STATE_BATTERY_ONLY;
        }
        if (!info->bat_present)
        {
            bq25792_set_charge_enable(&g_charger, false);
            _state = BQ_STATE_ADAPTER_ONLY;
        }
        break;

    case BQ_STATE_CHARGE_DONE:
        if (info->soc_pct <= BQ_SOC_RECHG_PCT)
        {
            bq25792_set_charge_enable(&g_charger, true);
            _state = BQ_STATE_CHARGING;
        }
        if (!info->vbus_present)
        {
            _state = BQ_STATE_BATTERY_ONLY;
        }
        if (!info->bat_present)
        {
            bq25792_set_charge_enable(&g_charger, false);
            _state = BQ_STATE_ADAPTER_ONLY;
        }
        break;

    case BQ_STATE_ADAPTER_ONLY:
        if (info->bat_present)
        {
            bq25792_set_charge_enable(&g_charger, true);
            _state = BQ_STATE_CHARGING;
        }
        break;

    case BQ_STATE_FAULT:
        if (info->fault0 == 0 && info->fault1 == 0)
        {
            _state = BQ_STATE_INIT;
        }
        break;

    default:
        _state = BQ_STATE_INIT;
        break;
    }
}

BqAppState bq_get_state(void) { return _state; }

int bq_reset_watchdog(void)
{
    return bq25792_reset_watchdog(&g_charger);
}

int bq_set_charge_enable(bool en)
{
    return bq25792_set_charge_enable(&g_charger, en);
}

int bq_sleep(void)
{
    int ret = 0;
    ret |= bq25792_set_adc_enable(&g_charger, false, false);
    ret |= bq25792_set_charge_enable(&g_charger, false);
    return ret;
}

int bq_ship_mode(void)
{
    int ret = 0;
    ret |= bq25792_set_charge_enable(&g_charger, false);
    ret |= bq25792_set_mode(&g_charger, BQ25792_MODE_SHIP);
    return ret;
}

int bq_shutdown_mode(void)
{
    int ret = 0;
    ret |= bq25792_set_charge_enable(&g_charger, false);
    ret |= bq25792_set_mode(&g_charger, BQ25792_MODE_SHUTDOWN);
    return ret;
}

int bq_wake(void)
{
    int ret = 0;
    ret |= bq25792_set_adc_enable(&g_charger, true, true);
    ret |= bq25792_set_charge_enable(&g_charger, false);
    return ret;
}

// ===== AT-handler: AT+CHRG?... =====
bool bq_at_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd;
    if (params == NULL)
    {
        at_send(AT_ERROR);
        return true;
    }

    char buf[64];

    if (strcmp(params, "?") == 0)
    {
        const char *st_name = "INIT";
        switch (_state)
        {
        case BQ_STATE_INIT:
            st_name = "INIT";
            break;
        case BQ_STATE_BATTERY_ONLY:
            st_name = "BATTERY_ONLY";
            break;
        case BQ_STATE_CHARGING:
            st_name = "CHARGING";
            break;
        case BQ_STATE_CHARGE_DONE:
            st_name = "CHARGE_DONE";
            break;
        case BQ_STATE_ADAPTER_ONLY:
            st_name = "ADAPTER_ONLY";
            break;
        case BQ_STATE_FAULT:
            st_name = "FAULT";
            break;
        }
        snprintf(buf, sizeof(buf), "AT+CHRG=%u,%u,%u,%u,%d,%s",
                 _last_info.vbat_mv, _last_info.vbus_mv, _last_info.vsys_mv,
                 _last_info.soc_pct, _last_info.ibat_ma, st_name);
        at_send(buf);
        return true;
    }

    if (strncmp(params, "VBAT", 4) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=VBAT,%u", _last_info.vbat_mv);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "VBUS", 4) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=VBUS,%u", _last_info.vbus_mv);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "VSYS", 4) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=VSYS,%u", _last_info.vsys_mv);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "SOC", 3) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=SOC,%u", _last_info.soc_pct);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "IBAT", 4) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=IBAT,%d", _last_info.ibat_ma);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "TDIE", 4) == 0)
    {
        snprintf(buf, sizeof(buf), "AT+CHRG=TDIE,%d.%u",
                 _last_info.tdie_raw / 2,
                 (_last_info.tdie_raw & 1u) ? 5u : 0u);
        at_send(buf);
        return true;
    }
    if (strncmp(params, "CHGSTAT", 7) == 0)
    {
        const char *chg_name = "UNKNOWN";
        switch ((bq25792_chg_stat_t)_last_info.chg_stat)
        {
        case BQ25792_CHG_NOT_CHARGING:
            chg_name = "NOT_CHARGING";
            break;
        case BQ25792_CHG_TRICKLE:
            chg_name = "TRICKLE";
            break;
        case BQ25792_CHG_PRECHARGE:
            chg_name = "PRECHG";
            break;
        case BQ25792_CHG_FAST_CC:
            chg_name = "FAST_CC";
            break;
        case BQ25792_CHG_TAPER_CV:
            chg_name = "TAPER_CV";
            break;
        case BQ25792_CHG_TOPOFF:
            chg_name = "TOPOFF";
            break;
        case BQ25792_CHG_DONE:
            chg_name = "DONE";
            break;
        default:
            break;
        }
        snprintf(buf, sizeof(buf), "AT+CHRG=CHGSTAT,%s", chg_name);
        at_send(buf);
        return true;
    }

    at_send(AT_ERROR);
    return true;
}