/**
 * @file aicode.c
 * @brief Платформа + утилиты (i2c_ll_write/read, g_tick_ms, delay_ms).
 *        Главный FSM перенесён в boot.c.
 *
 * Изменено относительно оригинала:
 *  - platform_log подавлен через макрос (#define).
 *  - aicode_main() — тонкая обёртка над boot_init/boot_update.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "bq25792.h"
#include "platform.h"
#include "main.h"

#include "stm32f1xx_ll_i2c.h"

// Подавление логов — все вызовы platform_log превращаются в no-op.
// Логи в USART2 сбивали бы AT-парсер с OrangePi.
#define platform_log(...)     do { } while (0)
#define platform_log_hex(...) do { } while (0)

#define BQ_I2C              I2C1
#define I2C_TIMEOUT_MS      10u

volatile uint32_t g_tick_ms = 0u;
BQ25792_RegisterMap_t regmap;

static inline uint32_t tick_ms(void) { return g_tick_ms; }

void delay_ms(uint32_t ms)
{
    uint32_t start = tick_ms();
    while ((tick_ms() - start) < ms) { __NOP(); }
}

#define I2C_WAIT(expr, timeout_ms, ret_label)                  \
    do {                                                       \
        uint32_t _t = tick_ms();                               \
        while (!(expr)) {                                      \
            if ((tick_ms() - _t) >= (timeout_ms)) {           \
                goto ret_label;                                \
            }                                                  \
        }                                                      \
    } while (0)

int i2c_ll_write(uint8_t addr, uint8_t reg,
                 const uint8_t *data, uint8_t len)
{
    I2C_WAIT(!LL_I2C_IsActiveFlag_BUSY(BQ_I2C), I2C_TIMEOUT_MS, err_busy);
    LL_I2C_GenerateStartCondition(BQ_I2C);
    I2C_WAIT(LL_I2C_IsActiveFlag_SB(BQ_I2C), I2C_TIMEOUT_MS, err_start);
    LL_I2C_TransmitData8(BQ_I2C, (uint8_t)(addr << 1u) | 0x00u);
    I2C_WAIT(LL_I2C_IsActiveFlag_ADDR(BQ_I2C), I2C_TIMEOUT_MS, err_addr);
    LL_I2C_ClearFlag_ADDR(BQ_I2C);
    I2C_WAIT(LL_I2C_IsActiveFlag_TXE(BQ_I2C), I2C_TIMEOUT_MS, err_tx);
    LL_I2C_TransmitData8(BQ_I2C, reg);
    for (uint8_t i = 0u; i < len; i++) {
        I2C_WAIT(LL_I2C_IsActiveFlag_TXE(BQ_I2C), I2C_TIMEOUT_MS, err_tx);
        LL_I2C_TransmitData8(BQ_I2C, data[i]);
    }
    I2C_WAIT(LL_I2C_IsActiveFlag_BTF(BQ_I2C), I2C_TIMEOUT_MS, err_tx);
    LL_I2C_GenerateStopCondition(BQ_I2C);
    return 0;

err_busy:  return 1;
err_start: LL_I2C_GenerateStopCondition(BQ_I2C); return 2;
err_addr:  LL_I2C_GenerateStopCondition(BQ_I2C); return 4;
err_tx:    LL_I2C_GenerateStopCondition(BQ_I2C); return 8;
}

int i2c_ll_read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    int i = 0u;

    do {
        if (len == 0u) return 0;

        I2C_WAIT(!LL_I2C_IsActiveFlag_BUSY(BQ_I2C), I2C_TIMEOUT_MS, err_busy);

        LL_I2C_GenerateStartCondition(BQ_I2C);
        I2C_WAIT(LL_I2C_IsActiveFlag_SB(BQ_I2C), I2C_TIMEOUT_MS, err_start);
        LL_I2C_TransmitData8(BQ_I2C, (uint8_t)(addr << 1u) | 0x00u);
        I2C_WAIT(LL_I2C_IsActiveFlag_ADDR(BQ_I2C), I2C_TIMEOUT_MS, err_addr);
        LL_I2C_ClearFlag_ADDR(BQ_I2C);
        I2C_WAIT(LL_I2C_IsActiveFlag_TXE(BQ_I2C), I2C_TIMEOUT_MS, err_tx);
        LL_I2C_TransmitData8(BQ_I2C, reg);
        I2C_WAIT(LL_I2C_IsActiveFlag_TXE(BQ_I2C), I2C_TIMEOUT_MS, err_tx);

        LL_I2C_GenerateStartCondition(BQ_I2C);
        I2C_WAIT(LL_I2C_IsActiveFlag_SB(BQ_I2C), I2C_TIMEOUT_MS, err_start);
        LL_I2C_TransmitData8(BQ_I2C, (uint8_t)(addr << 1u) | 0x01u);

        LL_I2C_AcknowledgeNextData(BQ_I2C, LL_I2C_NACK);
        I2C_WAIT(LL_I2C_IsActiveFlag_ADDR(BQ_I2C), I2C_TIMEOUT_MS, err_addr);
        __disable_irq();
        LL_I2C_ClearFlag_ADDR(BQ_I2C);
        LL_I2C_GenerateStopCondition(BQ_I2C);
        __enable_irq();
        I2C_WAIT(LL_I2C_IsActiveFlag_RXNE(BQ_I2C), I2C_TIMEOUT_MS, err_rx);
        data[i++] = LL_I2C_ReceiveData8(BQ_I2C);
        reg++; len--;
    } while (len);

    return 0;

err_busy:  return 1;
err_start: LL_I2C_GenerateStopCondition(BQ_I2C); return 2;
err_addr:  LL_I2C_GenerateStopCondition(BQ_I2C); return 4;
err_tx:    LL_I2C_GenerateStopCondition(BQ_I2C); return 8;
err_rx:    LL_I2C_GenerateStopCondition(BQ_I2C); return 16;
}

// ============================================================
// Утилиты (используются bq_compat.c, bat_info_update и boot).
// ============================================================
bq25792_t   g_charger;
bat_info_t  g_bat;

uint8_t calc_soc(uint16_t vbat_mv)
{
    const uint16_t V_MIN = 6000u;
    const uint16_t V_MAX = 8400u;
    if (vbat_mv <= V_MIN) return 0u;
    if (vbat_mv >= V_MAX) return 100u;
    return (uint8_t)(((uint32_t)(vbat_mv - V_MIN) * 100u) / (V_MAX - V_MIN));
}

static void _init_regmap(void)
{
    /* regmap.BYTE[] объявлен volatile в regs.h, но это software-cache,
     * не MMIO. Все равно нужно исключить warning о discards-qualifiers. */
    (void)memset((void *)&regmap.BYTE[0], 0, sizeof(regmap.BYTE));
}

int charger_init_2s(bq25792_t *dev)
{
    uint8_t part_id = 0u;
    int ret = 0;

    delay_ms(30u);
    _init_regmap();
    for (uint8_t i = 0; i <= 0x49; i++) {
        ret |= bq25792_buf_read(dev, i, (uint8_t*)&regmap.BYTE[i]);
    }
    if (ret != 0) return -1;

    part_id = regmap.reg.REG48_Part_Information.BITS.PN;
    if (part_id != 0x01u) return -2;

    regmap.reg.REG10_ChargerControl1.BITS.WATCHDOG_2_0 = BQ25792_REG10_WATCHDOG_DISABLE;
    regmap.reg.REG10_ChargerControl1.BITS.VAC_OVP_1_0 = BQ25792_REG10_VAC_OVP_26V;
    ret |= bq25792_write_reg(dev, 0x10, regmap.BYTE[0x10]);

    regmap.reg.REG11_ChargerControl2.BITS.AUTO_INDET_EN = 0;
    regmap.reg.REG11_ChargerControl2.BITS.EN_9V = 0;
    regmap.reg.REG11_ChargerControl2.BITS.EN_12V = 0;
    regmap.reg.REG11_ChargerControl2.BITS.HVDCP_EN = 1;
    ret |= bq25792_write_reg(dev, 0x11, regmap.BYTE[0x11]);

    regmap.reg.REG2E_ADCControl.BITS.ADC_EN = 1;
    regmap.reg.REG2E_ADCControl.BITS.ADC_RATE = 0;
    regmap.reg.REG2E_ADCControl.BITS.ADC_SAMPLE_1_0 = BQ25792_REG2E_ADC_SAMPLE_12BIT;
    regmap.reg.REG2E_ADCControl.BITS.ADC_AVG = 1;
    ret |= bq25792_write_reg(dev, 0x2E, regmap.BYTE[0x2E]);

    regmap.reg.REG05_InputVoltageLimit.BITS.VINDPM = BQ25792_REG05_VINDPM_mV_TO_REG(18000u);
    ret |= bq25792_write_reg(dev, 0x05, regmap.BYTE[0x05]);

    regmap.reg.REG03_ChargeCurrentLimit.WORD = BQ25792_REG03_ICHG_mA_TO_REG(1000u);
    ret |= bq25792_write_reg(dev, 0x03, regmap.BYTE[0x03]);

    regmap.reg.REG14_ChargerControl5.BITS.SFET_PRESENT = 1;
    regmap.reg.REG14_ChargerControl5.BITS.EN_IBAT = 1;
    regmap.reg.REG14_ChargerControl5.BITS.EN_IINDPM = 1;
    regmap.reg.REG14_ChargerControl5.BITS.IBAT_REG_1_0 = BQ25792_REG14_IBAT_REG_5A;
    ret |= bq25792_write_reg(dev, 0x14, regmap.BYTE[0x14]);

    regmap.reg.REG0F_ChargerControl0.BITS.EN_CHG = 1;
    regmap.reg.REG0F_ChargerControl0.BITS.EN_TERM = 1;
    ret |= bq25792_write_reg(dev, 0x0F, regmap.BYTE[0x0F]);

    if (ret != 0) return -3;
    return 0;
}

int bat_info_update(const bq25792_t *dev, bat_info_t *info)
{
    uint8_t stat[5];
    int reti = 0;

    for (uint8_t i = 0; i <= 0x49; i++) {
        reti |= bq25792_buf_read(dev, i, (uint8_t*)&regmap.BYTE[i]);
    }

    info->vbat_mv  = regmap.reg.REG3B_VBAT_ADC.WORD;
    info->vbus_mv  = regmap.reg.REG35_VBUS_ADC.WORD;
    info->vsys_mv  = regmap.reg.REG3D_VSYS_ADC.WORD;
    info->ibus_ma  = regmap.reg.REG31_IBUS_ADC.WORD;
    info->ibat_ma  = regmap.reg.REG33_IBAT_ADC.WORD;
    info->tdie_raw = regmap.reg.REG41_TDIE_ADC.WORD;
    info->fault[0] = regmap.reg.REG20_FaultStatus0.BYTE;
    info->fault[1] = regmap.reg.REG21_FaultStatus1.BYTE;
    stat[1]        = regmap.reg.REG1C_ChargerStatus1.BYTE;

    if (reti == 0) {
        info->chg_stat  = bq25792_decode_chg_stat(stat[1]);
        info->vbus_stat = bq25792_decode_vbus_stat(stat[1]);
        info->soc_pct   = calc_soc(info->vbat_mv);
    }

    regmap.reg.REG05_InputVoltageLimit.BITS.VINDPM = BQ25792_REG05_VINDPM_mV_TO_REG(18000u);
    reti |= bq25792_smart_write(dev, 0x05, &regmap); (void)reti;

    regmap.reg.REG14_ChargerControl5.BITS.SFET_PRESENT = 1;
    regmap.reg.REG14_ChargerControl5.BITS.EN_IBAT = 1;
    regmap.reg.REG14_ChargerControl5.BITS.EN_IINDPM = 1;
    regmap.reg.REG14_ChargerControl5.BITS.IBAT_REG_1_0 = BQ25792_REG14_IBAT_REG_5A;
    reti |= bq25792_smart_write(dev, 0x14, &regmap);

    regmap.reg.REG10_ChargerControl1.BITS.WATCHDOG_2_0 = BQ25792_REG10_WATCHDOG_DISABLE;
    regmap.reg.REG10_ChargerControl1.BITS.VAC_OVP_1_0 = BQ25792_REG10_VAC_OVP_26V;
    reti |= bq25792_smart_write(dev, 0x10, &regmap); (void)reti;

    regmap.reg.REG11_ChargerControl2.BITS.AUTO_INDET_EN = 0;
    regmap.reg.REG11_ChargerControl2.BITS.EN_9V = 0;
    regmap.reg.REG11_ChargerControl2.BITS.EN_12V = 0;
    regmap.reg.REG11_ChargerControl2.BITS.HVDCP_EN = 1;
    reti |= bq25792_smart_write(dev, 0x11, &regmap);

    regmap.reg.REG03_ChargeCurrentLimit.BITS.ICHG = BQ25792_REG03_ICHG_mA_TO_REG(1000u);
    reti |= bq25792_smart_write(dev, 0x03, &regmap);

    regmap.reg.REG2E_ADCControl.BYTE = 0;
    regmap.reg.REG2E_ADCControl.BITS.ADC_EN = 1;
    regmap.reg.REG2E_ADCControl.BITS.ADC_SAMPLE_1_0 = BQ25792_REG2E_ADC_SAMPLE_12BIT;
    reti |= bq25792_smart_write(dev, 0x2E, &regmap);

    return reti;
}

// ============================================================
// Application entry (бывший aicode_main) → теперь boot-FSM.
// ============================================================

void boot_init(void);
void boot_update(void);

void aicode_main(void)
{
    g_charger.i2c_write = i2c_ll_write;
    g_charger.i2c_read  = i2c_ll_read;

    boot_init();

    while (1) {
        boot_update();
    }
}