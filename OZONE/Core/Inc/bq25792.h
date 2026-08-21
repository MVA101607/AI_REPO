/**
 * @file bq25792.h
 * @brief BQ25792 I2C Buck-Boost Battery Charger Driver
 *
 * Texas Instruments BQ25792 — 1-4 cell, 5A, I2C-controlled
 * synchronous buck-boost charger with dual-input selector and USB PD 3.0 OTG.
 *
 * I2C Address: 0x6B
 * Register range: 0x00 – 0x48
 */

#ifndef BQ25792_H
#define BQ25792_H

#include <stdint.h>
#include <stdbool.h>
#include "bq25792_regs.h"


#ifdef __cplusplus
extern "C" {
#endif

int bq25792_smart_write(const bq25792_t *dev, uint8_t reg, BQ25792_RegisterMap_t * map);

int bq25792_smart_read(const bq25792_t *dev, uint8_t reg, BQ25792_RegisterMap_t * map); 

int bq25792_buf_write(const bq25792_t *dev, uint8_t reg, uint8_t val); 

int bq25792_buf_read(const bq25792_t *dev, uint8_t reg, uint8_t *val);

/**
 * @brief Read-modify-write a register (byte).
 * @param dev   Driver handle
 * @param reg   Register address
 * @param mask  Bitmask of bits to modify
 * @param val   New value (applied within mask)
 * @return 0 on success
 */
int bq25792_modify_reg(const bq25792_t *dev, uint8_t reg, uint8_t mask, uint8_t val);

/* =========================================================
 * Charging configuration
 * ========================================================= */

/**
 * @brief Set charge voltage limit (VREG).
 * @param dev      Driver handle
 * @param mv       Voltage in mV (3000–18800 mV, 10 mV step)
 * @return 0 on success
 */
int bq25792_set_charge_voltage(const bq25792_t *dev, uint16_t mv);

/**
 * @brief Get charge voltage limit.
 * @param dev      Driver handle
 * @param mv       Pointer to result in mV
 * @return 0 on success
 */
int bq25792_get_charge_voltage(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Set fast charge current limit (ICHG).
 * @param dev      Driver handle
 * @param ma       Current in mA (50–5000 mA, 10 mA step)
 * @return 0 on success
 */
int bq25792_set_charge_current(const bq25792_t *dev, uint16_t ma);

/**
 * @brief Get fast charge current limit.
 * @param dev      Driver handle
 * @param ma       Pointer to result in mA
 * @return 0 on success
 */
int bq25792_get_charge_current(const bq25792_t *dev, uint16_t *ma);

/**
 * @brief Set minimal system voltage (VSYSMIN).
 * @param dev      Driver handle
 * @param mv       Voltage in mV (2500–16000 mV, 250 mV step)
 * @return 0 on success
 */
int bq25792_set_min_sys_voltage(const bq25792_t *dev, uint16_t mv);

/**
 * @brief Get minimal system voltage.
 * @param dev      Driver handle
 * @param mv       Pointer to result in mV
 * @return 0 on success
 */
int bq25792_get_min_sys_voltage(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Set input voltage limit (VINDPM).
 * @param dev      Driver handle
 * @param mv       Voltage in mV (3600–22000 mV, 100 mV step)
 * @return 0 on success
 */
int bq25792_set_input_voltage_limit(const bq25792_t *dev, uint16_t mv);

/**
 * @brief Get input voltage limit.
 * @param dev      Driver handle
 * @param mv       Pointer to result in mV
 * @return 0 on success
 */
int bq25792_get_input_voltage_limit(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Set input current limit (IINDPM).
 * @param dev      Driver handle
 * @param ma       Current in mA (100–3300 mA, 10 mA step)
 * @return 0 on success
 */
int bq25792_set_input_current_limit(const bq25792_t *dev, uint16_t ma);

/**
 * @brief Get input current limit.
 * @param dev      Driver handle
 * @param ma       Pointer to result in mA
 * @return 0 on success
 */
int bq25792_get_input_current_limit(const bq25792_t *dev, uint16_t *ma);

/**
 * @brief Set pre-charge current (IPRECHG).
 * @param dev      Driver handle
 * @param ma       Current in mA (40–2000 mA, 40 mA step)
 * @return 0 on success
 */
int bq25792_set_prechg_current(const bq25792_t *dev, uint16_t ma);

/**
 * @brief Set termination current (ITERM).
 * @param dev      Driver handle
 * @param ma       Current in mA (40–1000 mA, 40 mA step)
 * @return 0 on success
 */
int bq25792_set_term_current(const bq25792_t *dev, uint16_t ma);

/**
 * @brief Set recharge voltage offset (VRECHG).
 *        Recharge threshold = VREG - offset.
 * @param dev      Driver handle
 * @param reg_val  Raw register nibble [3:0] (0x0–0xF)
 * @return 0 on success
 */
int bq25792_set_rechg_threshold(const bq25792_t *dev, uint8_t reg_val);

/* =========================================================
 * Charger enable / disable
 * ========================================================= */

/**
 * @brief Enable or disable battery charging (ENCHG bit).
 * @param dev     Driver handle
 * @param enable  true = charge enabled, false = disabled
 * @return 0 on success
 */
int bq25792_set_charge_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Enable or disable HIZ mode (high-impedance input).
 *        In HIZ mode the converter stops switching; the battery
 *        powers the system.  HIZ is auto-cleared on adapter plug-in.
 * @param dev     Driver handle
 * @param enable  true = HIZ on
 * @return 0 on success
 */
int bq25792_set_hiz(const bq25792_t *dev, bool enable);

/**
 * @brief Enable or disable charge termination (ENTERM bit).
 * @param dev     Driver handle
 * @param enable  true = termination enabled (default)
 * @return 0 on success
 */
int bq25792_set_term_enable(const bq25792_t *dev, bool enable);

/* =========================================================
 * OTG (USB On-The-Go)
 * ========================================================= */

/**
 * @brief Enable or disable OTG mode (ENOTG bit).
 *        OTG discharges the battery to generate a VBUS output.
 *        Requires VBUS below UVLO and battery > VBATOTG threshold.
 * @param dev     Driver handle
 * @param enable  true = OTG on
 * @return 0 on success
 */
int bq25792_set_otg_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Set OTG output voltage (VOTG).
 * @param dev  Driver handle
 * @param mv   Voltage in mV (2800–22000 mV, 10 mV step)
 * @return 0 on success
 */
int bq25792_set_otg_voltage(const bq25792_t *dev, uint16_t mv);

/**
 * @brief Set OTG output current limit (IOTG).
 * @param dev  Driver handle
 * @param ma   Current in mA (120–3320 mA, 40 mA step)
 * @return 0 on success
 */
int bq25792_set_otg_current(const bq25792_t *dev, uint16_t ma);

/* =========================================================
 * Input MUX / ACFET control
 * ========================================================= */

/**
 * @brief Enable ACDRV1 gate driver (ACFET1-RBFET1 pair).
 *        Must be detected at POR; locked at 0 if not populated.
 * @param dev     Driver handle
 * @param enable  true = turn on ACFET1-RBFET1
 * @return 0 on success
 */
int bq25792_set_acdrv1_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Enable ACDRV2 gate driver (ACFET2-RBFET2 pair).
 * @param dev     Driver handle
 * @param enable  true = turn on ACFET2-RBFET2
 * @return 0 on success
 */
int bq25792_set_acdrv2_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Disable both ACDRV drivers simultaneously (DISACDRV bit).
 *        Forces both ACFET-RBFET pairs off regardless of ENACDRV bits.
 * @param dev     Driver handle
 * @param disable true = force both ACDRVs off
 * @return 0 on success
 */
int bq25792_set_disacdrv(const bq25792_t *dev, bool disable);

/**
 * @brief Set VAC OVP threshold.
 * @param dev   Driver handle
 * @param ovp   One of bq25792_vacovp_t (7V, 12V, 22V, 26V)
 * @return 0 on success
 */
int bq25792_set_vacovp(const bq25792_t *dev, bq25792_vacovp_t ovp);

/* =========================================================
 * Ship / shutdown mode
 * ========================================================= */

/**
 * @brief Set device operational mode (ship, shutdown, idle, system reset).
 *        Ship and Shutdown modes are only accepted when no adapter is present.
 * @param dev   Driver handle
 * @param mode  bq25792_mode_t
 * @return 0 on success
 */
int bq25792_set_mode(const bq25792_t *dev, bq25792_mode_t mode);

/* =========================================================
 * Watchdog timer
 * ========================================================= */

/**
 * @brief Set watchdog timer period.
 * @param dev      Driver handle
 * @param timeout  0=disable, 1=40s, 2=80s, 3=160s
 * @return 0 on success
 */
int bq25792_set_watchdog(const bq25792_t *dev, uint8_t timeout);

/**
 * @brief Reset (kick) the watchdog timer by writing WDRST=1.
 * @param dev  Driver handle
 * @return 0 on success
 */
int bq25792_reset_watchdog(const bq25792_t *dev);

/* =========================================================
 * Timer control
 * ========================================================= */

/**
 * @brief Configure fast charge safety timer.
 * @param dev      Driver handle
 * @param enable   true = timer enabled
 * @param setting  0=5h, 1=8h, 2=12h (default), 3=24h
 * @return 0 on success
 */
int bq25792_set_chg_timer(const bq25792_t *dev, bool enable, uint8_t setting);

/**
 * @brief Enable or disable pre-charge timer (2 h default).
 * @param dev     Driver handle
 * @param enable  true = timer enabled (default)
 * @return 0 on success
 */
int bq25792_set_prechg_timer_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Configure top-off timer.
 * @param dev      Driver handle
 * @param setting  0=disable (default), 1=15min, 2=30min, 3=45min
 * @return 0 on success
 */
int bq25792_set_topoff_timer(const bq25792_t *dev, uint8_t setting);

/* =========================================================
 * ADC
 * ========================================================= */

/**
 * @brief Enable or disable the internal ADC.
 * @param dev         Driver handle
 * @param enable      true = ADC on
 * @param continuous  true = continuous conversion, false = one-shot
 * @return 0 on success
 */
int bq25792_set_adc_enable(const bq25792_t *dev, bool enable, bool continuous);

/**
 * @brief Read VBUS voltage via ADC.
 * @param dev   Driver handle
 * @param mv    Pointer to result in mV (0–30000 mV, 1 mV LSB)
 * @return 0 on success
 */
int bq25792_read_vbus(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Read VAC1 voltage via ADC.
 * @param dev   Driver handle
 * @param mv    Pointer to result in mV
 * @return 0 on success
 */
int bq25792_read_vac1(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Read VAC2 voltage via ADC.
 * @param dev   Driver handle
 * @param mv    Pointer to result in mV
 * @return 0 on success
 */
int bq25792_read_vac2(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Read battery voltage (VBAT) via ADC.
 * @param dev   Driver handle
 * @param mv    Pointer to result in mV (0–20000 mV, 1 mV LSB)
 * @return 0 on success
 */
int bq25792_read_vbat(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Read system voltage (VSYS) via ADC.
 * @param dev   Driver handle
 * @param mv    Pointer to result in mV (0–24000 mV, 1 mV LSB)
 * @return 0 on success
 */
int bq25792_read_vsys(const bq25792_t *dev, uint16_t *mv);

/**
 * @brief Read input current (IBUS) via ADC.
 * @param dev   Driver handle
 * @param ma    Pointer to result in mA (0–5000 mA, 1 mA LSB)
 * @return 0 on success
 */
int bq25792_read_ibus(const bq25792_t *dev, uint16_t *ma);

/**
 * @brief Read battery current (IBAT) via ADC.
 *        Positive = charging.
 * @param dev   Driver handle
 * @param ma    Pointer to result in mA (0–8000 mA, 1 mA LSB)
 * @return 0 on success
 */
int bq25792_read_ibat(const bq25792_t *dev, int16_t *ma);

/**
 * @brief Read die temperature via ADC.
 * @param dev      Driver handle
 * @param temp_c   Pointer to result in 0.5 °C units (raw value × 0.5 = °C)
 * @return 0 on success
 */
int bq25792_read_tdie(const bq25792_t *dev, int16_t *temp_c);

/**
 * @brief Read TS pin voltage as percentage of REGN (0–99.9%, 0.098% LSB).
 * @param dev       Driver handle
 * @param ts_pct16  Raw 16-bit ADC value (divide by 10 for % × 10)
 * @return 0 on success
 */
int bq25792_read_ts(const bq25792_t *dev, uint16_t *ts_pct16);

/* =========================================================
 * Status readback
 * ========================================================= */

/**
 * @brief Read charger status registers 0–4.
 * @param dev    Driver handle
 * @param stat   Array of 5 bytes to receive status [0]=REG1B … [4]=REG1F
 * @return 0 on success
 */
int bq25792_read_chg_status(const bq25792_t *dev, uint8_t stat[5]);

/**
 * @brief Read fault status registers 0–1.
 * @param dev    Driver handle
 * @param fault  Array of 2 bytes [0]=REG20, [1]=REG21
 * @return 0 on success
 */
int bq25792_read_fault_status(const bq25792_t *dev, uint8_t fault[2]);

/**
 * @brief Read charger flag registers 0–3 (auto-clear on read).
 * @param dev   Driver handle
 * @param flag  Array of 4 bytes [0]=REG22 … [3]=REG25
 * @return 0 on success
 */
int bq25792_read_chg_flags(const bq25792_t *dev, uint8_t flag[4]);

/**
 * @brief Read fault flag registers 0–1 (auto-clear on read).
 * @param dev   Driver handle
 * @param flag  Array of 2 bytes [0]=REG26, [1]=REG27
 * @return 0 on success
 */
int bq25792_read_fault_flags(const bq25792_t *dev, uint8_t flag[2]);

/**
 * @brief Decode VBUS/charger source type from Charger Status 1.
 * @param stat1  Value of REG0x1C
 * @return bq25792_vbus_stat_t
 */
bq25792_vbus_stat_t bq25792_decode_vbus_stat(uint8_t stat1);

/**
 * @brief Decode charge phase from Charger Status 1.
 * @param stat1  Value of REG0x1C
 * @return bq25792_chg_stat_t
 */
bq25792_chg_stat_t bq25792_decode_chg_stat(uint8_t stat1);

/* =========================================================
 * Interrupt masks
 * ========================================================= */

/**
 * @brief Write Charger Mask 0 register (REG0x28).
 * @param dev   Driver handle
 * @param mask  Bitmask — set bit = suppress that INT source
 * @return 0 on success
 */
int bq25792_set_chg_mask0(const bq25792_t *dev, uint8_t mask);

/**
 * @brief Write Charger Mask 1 register (REG0x29).
 * @param dev   Driver handle
 * @param mask  Bitmask
 * @return 0 on success
 */
int bq25792_set_chg_mask1(const bq25792_t *dev, uint8_t mask);

/**
 * @brief Write Charger Mask 2 register (REG0x2A).
 * @param dev   Driver handle
 * @param mask  Bitmask
 * @return 0 on success
 */
int bq25792_set_chg_mask2(const bq25792_t *dev, uint8_t mask);

/**
 * @brief Write Fault Mask 0 register (REG0x2B).
 * @param dev   Driver handle
 * @param mask  Bitmask
 * @return 0 on success
 */
int bq25792_set_fault_mask0(const bq25792_t *dev, uint8_t mask);

/**
 * @brief Write Fault Mask 1 register (REG0x2C).
 * @param dev   Driver handle
 * @param mask  Bitmask
 * @return 0 on success
 */
int bq25792_set_fault_mask1(const bq25792_t *dev, uint8_t mask);

/* =========================================================
 * NTC / temperature
 * ========================================================= */

/**
 * @brief Enable or disable TS pin monitoring (TSIGNORE bit).
 *        When ignored, charger considers TS always in-range.
 * @param dev     Driver handle
 * @param ignore  true = ignore TS (no thermistor required)
 * @return 0 on success
 */
int bq25792_set_ts_ignore(const bq25792_t *dev, bool ignore);

/* =========================================================
 * ICO (Input Current Optimizer)
 * ========================================================= */

/**
 * @brief Enable ICO algorithm (ENICO bit).
 * @param dev     Driver handle
 * @param enable  true = ICO enabled
 * @return 0 on success
 */
int bq25792_set_ico_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Force an immediate ICO run (FORCEICO bit).
 *        ENICO must be 1 first.
 * @param dev  Driver handle
 * @return 0 on success
 */
int bq25792_force_ico(const bq25792_t *dev);

/**
 * @brief Read ICO-determined current limit from REG0x19.
 * @param dev   Driver handle
 * @param ma    Pointer to result in mA
 * @return 0 on success
 */
int bq25792_get_ico_limit(const bq25792_t *dev, uint16_t *ma);

/* =========================================================
 * Miscellaneous
 * ========================================================= */

/**
 * @brief Software reset all registers to POR defaults (REGRST bit).
 *        The bit auto-clears after reset.
 * @param dev  Driver handle
 * @return 0 on success
 */
int bq25792_reg_reset(const bq25792_t *dev);

/**
 * @brief Read Part Information register (REG0x48).
 * @param dev      Driver handle
 * @param part_id  Pointer to result byte
 * @return 0 on success
 */
int bq25792_read_part_info(const bq25792_t *dev, uint8_t *part_id);

/**
 * @brief Set switching frequency via PWMFREQ bit (REG0x13 bit[5]).
 * @param dev   Driver handle
 * @param freq  0 = 1.5 MHz, 1 = 750 kHz
 * @return 0 on success
 */
int bq25792_set_pwm_freq(const bq25792_t *dev, uint8_t freq);

/**
 * @brief Force VINDPM re-detection (FORCEVINDPMDET bit).
 *        Only valid when VBAT >= VSYSMIN.
 * @param dev  Driver handle
 * @return 0 on success
 */
int bq25792_force_vindpm_detect(const bq25792_t *dev);

/**
 * @brief Enable or disable HVDCP handshake (HVDCPEN bit).
 * @param dev     Driver handle
 * @param enable  true = HVDCP enabled
 * @return 0 on success
 */
int bq25792_set_hvdcp_enable(const bq25792_t *dev, bool enable);

/**
 * @brief Configure DPDM driver register (REG0x47) for D+/D- output levels.
 * @param dev  Driver handle
 * @param val  Raw register byte
 * @return 0 on success
 */
int bq25792_set_dpdm_driver(const bq25792_t *dev, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* BQ25792_H */
