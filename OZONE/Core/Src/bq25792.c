/**
 * @file bq25792.c
 * @brief BQ25792 I2C Buck-Boost Battery Charger Driver — Implementation
 */

#include "bq25792.h"

/* =========================================================
 * Helpers
 * ========================================================= */

static inline int _write(const bq25792_t *dev, uint8_t reg, uint8_t val) {
    return dev->i2c_write(BQ25792_I2C_ADDR, reg, &val, 1);
}

static inline int _read(const bq25792_t *dev, uint8_t reg, uint8_t *val) {
    return dev->i2c_read(BQ25792_I2C_ADDR, reg, val, 1);
}


/**
 * Таблица смещений для адресов 0x00 - 0x48:
 * 0:  8-битный регистр, смещение не нужно.
 * 1, -1:  смещение адреса чтобы поменять местами половинки 16-битного регистра
 * Использование таблицы смещений (lookup table) — это самый быстрый способ 
 * «на лету» приводить адреса регистров в соответствие с требованиями протокола 
 * I2C (Big Endian) и архитектурой ARM (Little Endian).
 */
const int8_t reg_offset_map[0x48] = {
    // 00-07   00   01   02   03   04   05   06   07
    [0x00] =    0,   1,  -1,   1,  -1,   0,   1,  -1, 
    // 08-0F   08   09   0A   0B   0C   0D   0E   0F
    [0x08] =    0,   0,   0,   1,  -1,   0,   0,   0,
    // 10-17   10   11   12   13   14   15   16   17
    [0x10] =    0,   0,   0,   0,   0,   0,   0,   0,
    // 18-1F   18   19   1A   1B   1C   1D   1E   1F
    [0x18] =    0,   1,  -1,   0,   0,   0,   0,   0,
    // 20-27   20   21   22   23   24   25   26   27
    [0x20] =    0,   0,   0,   0,   0,   0,   0,   0,
    // 28-2F   28   29   2A   2B   2C   2D   2E   2F
    [0x28] =    0,   0,   0,   0,   0,   0,   0,   0,
    // 30-37   30   31   32   33   34   35   36   37
    [0x30] =    0,   1,  -1,   1,  -1,   1,  -1,   1,
    // 38-3F   38   39   3A   3B   3C   3D   3E   3F
    [0x38] =   -1,  1,  -1,   1,  -1,   1,  -1,   1,
    // 40-47   40   41   42   43   44   45   46   47
    [0x40] =   -1,  1,  -1,   1,  -1,   1,  -1,   0
};

static inline int8_t reg_offset(uint8_t addr){
  return (addr >= 0x48) ? addr : addr + reg_offset_map[addr];
}
/* =========================================================
 * Low-level register access
 * ========================================================= */

int bq25792_smart_write(const bq25792_t *dev, uint8_t reg, BQ25792_RegisterMap_t * map) {
    uint8_t buf[2];
    if(reg_offset_map[reg]){
    // если регистр 16-битный
        buf[0] = map->BYTE[reg+1];
        buf[1] = map->BYTE[reg];
        return dev->i2c_write(BQ25792_I2C_ADDR, reg, buf, 2);        
    }else{
    // если регистр 8-битный
        buf[0] = map->BYTE[reg];
        return dev->i2c_write(BQ25792_I2C_ADDR, reg, buf, 1);
    }
}

int bq25792_smart_read(const bq25792_t *dev, uint8_t reg, BQ25792_RegisterMap_t * map) {
    uint8_t buf[2];
    int ret = 0;
    if(reg_offset_map[reg]){
    // если регистр 16-битный
        ret = dev->i2c_read(BQ25792_I2C_ADDR, reg, buf, 2);
            if (ret == 0) {
                map->BYTE[reg+1] = buf[0];
                map->BYTE[reg]   = buf[1];
            }
        return ret;       
    }else{
    // если регистр 8-битный
        ret = dev->i2c_read(BQ25792_I2C_ADDR, reg, buf, 1);
        if (ret == 0) map->BYTE[reg] = buf[0];
        return ret;
    }
}

int bq25792_buf_write(const bq25792_t *dev, uint8_t reg, uint8_t val) {
    return dev->i2c_write(BQ25792_I2C_ADDR, reg_offset(reg), &val, 1);
}


int bq25792_buf_read(const bq25792_t *dev, uint8_t reg, uint8_t *val) {
  return dev->i2c_read(BQ25792_I2C_ADDR, reg_offset(reg), val, 1);
}

int bq25792_write_reg(const bq25792_t *dev, uint8_t reg, uint8_t val) {
  return dev->i2c_write(BQ25792_I2C_ADDR, reg, &val, 1);
}

int bq25792_read_reg(const bq25792_t *dev, uint8_t reg, uint8_t *val) {
  return dev->i2c_read(BQ25792_I2C_ADDR, reg, val, 1);
}

int bq25792_write_reg16(const bq25792_t *dev, uint8_t reg, uint16_t val) {
    uint8_t buf[2] = { (uint8_t)(val >> 8), (uint8_t)(val & 0xFF) };
    return dev->i2c_write(BQ25792_I2C_ADDR, reg, buf, 2);
}

int bq25792_read_reg16(const bq25792_t *dev, uint8_t reg, uint16_t *val) {
    uint8_t buf[2];
    int ret = dev->i2c_read(BQ25792_I2C_ADDR, reg, buf, 2);
    if (ret == 0) {
        *val = ((uint16_t)buf[0] << 8) | buf[1];
    }
    return ret;
}

int bq25792_modify_reg(const bq25792_t *dev, uint8_t reg, uint8_t mask, uint8_t val) {
    uint8_t tmp;
    int ret = _read(dev, reg, &tmp);
    if (ret != 0) return ret;
    tmp = (tmp & ~mask) | (val & mask);
    return _write(dev, reg, tmp);
}

/* =========================================================
 * Charging configuration
 * ========================================================= */

int bq25792_set_charge_voltage(const bq25792_t *dev, uint16_t mv) {
    /* VREG[10:0] = (mv - 0) / 10, offset 0, step 10 mV */
    uint16_t raw = mv / 10u;
    return bq25792_write_reg16(dev, BQ25792_REG_CHG_VOLT_LIM, raw & 0x07FFu);
}

int bq25792_get_charge_voltage(const bq25792_t *dev, uint16_t *mv) {
    uint16_t raw;
    int ret = bq25792_read_reg16(dev, BQ25792_REG_CHG_VOLT_LIM, &raw);
    if (ret == 0) *mv = (raw & 0x07FFu) * 10u;
    return ret;
}

int bq25792_set_charge_current(const bq25792_t *dev, uint16_t ma) {
    /* ICHG[8:0] = ma / 10, step 10 mA */
    uint16_t raw = ma / 10u;
    return bq25792_write_reg16(dev, BQ25792_REG_CHG_CURR_LIM, raw & 0x01FFu);
}

int bq25792_get_charge_current(const bq25792_t *dev, uint16_t *ma) {
    uint16_t raw;
    int ret = bq25792_read_reg16(dev, BQ25792_REG_CHG_CURR_LIM, &raw);
    if (ret == 0) *ma = (raw & 0x01FFu) * 10u;
    return ret;
}

int bq25792_set_min_sys_voltage(const bq25792_t *dev, uint16_t mv) {
    /* VSYSMIN[5:0], offset 2500, step 250 mV  → bits in REG0x00 [5:0] */
    uint8_t raw = (uint8_t)((mv - 2500u) / 250u);
    return bq25792_modify_reg(dev, BQ25792_REG_MIN_SYS_VOLT, 0x3Fu, raw & 0x3Fu);
}

int bq25792_get_min_sys_voltage(const bq25792_t *dev, uint16_t *mv) {
    uint8_t raw;
    int ret = _read(dev, BQ25792_REG_MIN_SYS_VOLT, &raw);
    if (ret == 0) *mv = 2500u + (uint16_t)(raw & 0x3Fu) * 250u;
    return ret;
}

int bq25792_set_input_voltage_limit(const bq25792_t *dev, uint16_t mv) {
    /* VINDPM[7:0], offset 3600, step 100 mV */
    uint8_t raw = (uint8_t)((mv - 3600u) / 100u);
    return _write(dev, BQ25792_REG_INPUT_VOLT_LIM, raw);
}

int bq25792_get_input_voltage_limit(const bq25792_t *dev, uint16_t *mv) {
    uint8_t raw;
    int ret = _read(dev, BQ25792_REG_INPUT_VOLT_LIM, &raw);
    if (ret == 0) *mv = 3600u + (uint16_t)raw * 100u;
    return ret;
}

int bq25792_set_input_current_limit(const bq25792_t *dev, uint16_t ma) {
    /* IINDPM[8:0], offset 0, step 10 mA */
    uint16_t raw = ma / 10u;
    return bq25792_write_reg16(dev, BQ25792_REG_INPUT_CURR_LIM, raw & 0x01FFu);
}

int bq25792_get_input_current_limit(const bq25792_t *dev, uint16_t *ma) {
    uint16_t raw;
    int ret = bq25792_read_reg16(dev, BQ25792_REG_INPUT_CURR_LIM, &raw);
    if (ret == 0) *ma = (raw & 0x01FFu) * 10u;
    return ret;
}

int bq25792_set_prechg_current(const bq25792_t *dev, uint16_t ma) {
    /* IPRECHG[5:0] upper nibble REG0x08[7:4], step 40 mA */
    uint8_t raw = (uint8_t)((ma / 40u) - 1u);
    return bq25792_modify_reg(dev, BQ25792_REG_PRECHG_CTRL, 0xF0u, (raw & 0x0Fu) << 4);
}

int bq25792_set_term_current(const bq25792_t *dev, uint16_t ma) {
    /* ITERM[4:0] REG0x09 [4:0], step 40 mA */
    uint8_t raw = (uint8_t)((ma / 40u) - 1u);
    return bq25792_modify_reg(dev, BQ25792_REG_TERM_CTRL, 0x1Fu, raw & 0x1Fu);
}

int bq25792_set_rechg_threshold(const bq25792_t *dev, uint8_t reg_val) {
    return bq25792_modify_reg(dev, BQ25792_REG_RECHG_CTRL, 0x0Fu, reg_val & 0x0Fu);
}

/* =========================================================
 * Charger enable / HIZ
 * ========================================================= */

int bq25792_set_charge_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL0,
                              BQ25792_ENCHG_MASK,
                              enable ? BQ25792_ENCHG_MASK : 0);
}

int bq25792_set_hiz(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL0,
                              BQ25792_ENHIZ_MASK,
                              enable ? BQ25792_ENHIZ_MASK : 0);
}

int bq25792_set_term_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL0,
                              BQ25792_ENTERM_MASK,
                              enable ? BQ25792_ENTERM_MASK : 0);
}

/* =========================================================
 * OTG
 * ========================================================= */

int bq25792_set_otg_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL3,
                              BQ25792_ENOTG_MASK,
                              enable ? BQ25792_ENOTG_MASK : 0);
}

int bq25792_set_otg_voltage(const bq25792_t *dev, uint16_t mv) {
    /* VOTG[10:0], offset 2800, step 10 mV */
    uint16_t raw = (mv - 2800u) / 10u;
    return bq25792_write_reg16(dev, BQ25792_REG_VOTG_REG, raw & 0x07FFu);
}

int bq25792_set_otg_current(const bq25792_t *dev, uint16_t ma) {
    /* IOTG[6:0], offset 0, step 40 mA */
    uint8_t raw = (uint8_t)(ma / 40u);
    return bq25792_modify_reg(dev, BQ25792_REG_IOTG_REG, 0x7Fu, raw & 0x7Fu);
}

/* =========================================================
 * Input MUX / ACFET
 * ========================================================= */

int bq25792_set_acdrv1_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL4,
                              BQ25792_ENACDRV1_MASK,
                              enable ? BQ25792_ENACDRV1_MASK : 0);
}

int bq25792_set_acdrv2_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL4,
                              BQ25792_ENACDRV2_MASK,
                              enable ? BQ25792_ENACDRV2_MASK : 0);
}

int bq25792_set_disacdrv(const bq25792_t *dev, bool disable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL3,
                              BQ25792_DISACDRV_MASK,
                              disable ? BQ25792_DISACDRV_MASK : 0);
}

int bq25792_set_vacovp(const bq25792_t *dev, bq25792_vacovp_t ovp) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL1,
                              BQ25792_VACOVP_MASK,
                              (uint8_t)(ovp << 6));
}

/* =========================================================
 * Ship / shutdown
 * ========================================================= */

int bq25792_set_mode(const bq25792_t *dev, bq25792_mode_t mode) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL2,
                              BQ25792_SDRVCTRL_MASK,
                              (uint8_t)(mode << 1));
}

/* =========================================================
 * Watchdog
 * ========================================================= */

int bq25792_set_watchdog(const bq25792_t *dev, uint8_t timeout) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL1,
                              BQ25792_WATCHDOG_MASK,
                              (uint8_t)((timeout & 0x03u) << 4));
}

int bq25792_reset_watchdog(const bq25792_t *dev) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL1,
                              BQ25792_WDRST_MASK,
                              BQ25792_WDRST_MASK);
}

/* =========================================================
 * Timer control
 * ========================================================= */

int bq25792_set_chg_timer(const bq25792_t *dev, bool enable, uint8_t setting) {
    uint8_t mask = (1 << 3) | (0x03 << 1);
    uint8_t val  = (enable ? (1 << 3) : 0) | ((setting & 0x03u) << 1);
    return bq25792_modify_reg(dev, BQ25792_REG_TIMER_CTRL, mask, val);
}

int bq25792_set_prechg_timer_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_TIMER_CTRL,
                              (1 << 4),
                              enable ? (1 << 4) : 0);
}

int bq25792_set_topoff_timer(const bq25792_t *dev, uint8_t setting) {
    return bq25792_modify_reg(dev, BQ25792_REG_TIMER_CTRL,
                              (0x03u << 6),
                              (uint8_t)((setting & 0x03u) << 6));
}

/* =========================================================
 * ADC
 * ========================================================= */

int bq25792_set_adc_enable(const bq25792_t *dev, bool enable, bool continuous) {
    uint8_t val = (enable ? BQ25792_ADCEN_MASK : 0)
                | (continuous ? 0 : BQ25792_ADCRATE_MASK);
    return bq25792_modify_reg(dev, BQ25792_REG_ADC_CTRL,
                              BQ25792_ADCEN_MASK | BQ25792_ADCRATE_MASK,
                              val);
}

int bq25792_read_vbus(const bq25792_t *dev, uint16_t *mv) {
    return bq25792_read_reg16(dev, BQ25792_REG_VBUS_ADC, mv);
}

int bq25792_read_vac1(const bq25792_t *dev, uint16_t *mv) {
    return bq25792_read_reg16(dev, BQ25792_REG_VAC1_ADC, mv);
}

int bq25792_read_vac2(const bq25792_t *dev, uint16_t *mv) {
    return bq25792_read_reg16(dev, BQ25792_REG_VAC2_ADC, mv);
}

int bq25792_read_vbat(const bq25792_t *dev, uint16_t *mv) {
    return bq25792_read_reg16(dev, BQ25792_REG_VBAT_ADC, mv);
}

int bq25792_read_vsys(const bq25792_t *dev, uint16_t *mv) {
    return bq25792_read_reg16(dev, BQ25792_REG_VSYS_ADC, mv);
}

int bq25792_read_ibus(const bq25792_t *dev, uint16_t *ma) {
    return bq25792_read_reg16(dev, BQ25792_REG_IBUS_ADC, ma);
}

int bq25792_read_ibat(const bq25792_t *dev, int16_t *ma) {
    return bq25792_read_reg16(dev, BQ25792_REG_IBAT_ADC, (uint16_t *)ma);
}

int bq25792_read_tdie(const bq25792_t *dev, int16_t *temp_c) {
    return bq25792_read_reg16(dev, BQ25792_REG_TDIE_ADC, (uint16_t *)temp_c);
}

int bq25792_read_ts(const bq25792_t *dev, uint16_t *ts_pct16) {
    return bq25792_read_reg16(dev, BQ25792_REG_TS_ADC, ts_pct16);
}

/* =========================================================
 * Status readback
 * ========================================================= */

int bq25792_read_chg_status(const bq25792_t *dev, uint8_t stat[5]) {
    return dev->i2c_read(BQ25792_I2C_ADDR, BQ25792_REG_CHG_STAT0, stat, 5);
}

int bq25792_read_fault_status(const bq25792_t *dev, uint8_t fault[2]) {
    return dev->i2c_read(BQ25792_I2C_ADDR, BQ25792_REG_FAULT_STAT0, fault, 2);
}

int bq25792_read_chg_flags(const bq25792_t *dev, uint8_t flag[4]) {
    return dev->i2c_read(BQ25792_I2C_ADDR, BQ25792_REG_CHG_FLAG0, flag, 4);
}

int bq25792_read_fault_flags(const bq25792_t *dev, uint8_t flag[2]) {
    return dev->i2c_read(BQ25792_I2C_ADDR, BQ25792_REG_FAULT_FLAG0, flag, 2);
}

bq25792_vbus_stat_t bq25792_decode_vbus_stat(uint8_t stat1) {
    return (bq25792_vbus_stat_t)((stat1 >> 5) & 0x07u);
}

bq25792_chg_stat_t bq25792_decode_chg_stat(uint8_t stat1) {
    return (bq25792_chg_stat_t)((stat1 >> 2) & 0x07u);
}

/* =========================================================
 * Interrupt masks
 * ========================================================= */

int bq25792_set_chg_mask0(const bq25792_t *dev, uint8_t mask) {
    return _write(dev, BQ25792_REG_CHG_MASK0, mask);
}

int bq25792_set_chg_mask1(const bq25792_t *dev, uint8_t mask) {
    return _write(dev, BQ25792_REG_CHG_MASK1, mask);
}

int bq25792_set_chg_mask2(const bq25792_t *dev, uint8_t mask) {
    return _write(dev, BQ25792_REG_CHG_MASK2, mask);
}

int bq25792_set_fault_mask0(const bq25792_t *dev, uint8_t mask) {
    return _write(dev, BQ25792_REG_FAULT_MASK0, mask);
}

int bq25792_set_fault_mask1(const bq25792_t *dev, uint8_t mask) {
    return _write(dev, BQ25792_REG_FAULT_MASK1, mask);
}

/* =========================================================
 * NTC
 * ========================================================= */

int bq25792_set_ts_ignore(const bq25792_t *dev, bool ignore) {
    return bq25792_modify_reg(dev, BQ25792_REG_NTC_CTRL1,
                              0x01u,
                              ignore ? 0x01u : 0x00u);
}

/* =========================================================
 * ICO
 * ========================================================= */

int bq25792_set_ico_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL0,
                              BQ25792_ENICO_MASK,
                              enable ? BQ25792_ENICO_MASK : 0);
}

int bq25792_force_ico(const bq25792_t *dev) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL0,
                              BQ25792_FORCEICO_MASK,
                              BQ25792_FORCEICO_MASK);
}

int bq25792_get_ico_limit(const bq25792_t *dev, uint16_t *ma) {
    uint16_t raw;
    int ret = bq25792_read_reg16(dev, BQ25792_REG_ICO_CURR_LIM, &raw);
    if (ret == 0) *ma = (raw & 0x01FFu) * 10u;
    return ret;
}

/* =========================================================
 * Miscellaneous
 * ========================================================= */

int bq25792_reg_reset(const bq25792_t *dev) {
    /* REGRST bit is bit 6 of REG0x09 (TerminationControl) */
    return bq25792_modify_reg(dev, BQ25792_REG_TERM_CTRL,
                              (1 << 6), (1 << 6));
}

int bq25792_read_part_info(const bq25792_t *dev, uint8_t *part_id) {
    return _read(dev, BQ25792_REG_PART_INFO, part_id);
}

int bq25792_set_pwm_freq(const bq25792_t *dev, uint8_t freq) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL4,
                              (1 << 5),
                              freq ? (1 << 5) : 0);
}

int bq25792_force_vindpm_detect(const bq25792_t *dev) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL4,
                              (1 << 1), (1 << 1));
}

int bq25792_set_hvdcp_enable(const bq25792_t *dev, bool enable) {
    return bq25792_modify_reg(dev, BQ25792_REG_CHG_CTRL2,
                              (1 << 3),
                              enable ? (1 << 3) : 0);
}

int bq25792_set_dpdm_driver(const bq25792_t *dev, uint8_t val) {
    return _write(dev, BQ25792_REG_DPDM_DRIVER, val);
}
