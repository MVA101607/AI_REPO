#pragma once
#include <stdint.h> // uint8_t/uint16_t

/* =========================================================
 * I2C Address
 * ========================================================= */
#define BQ25792_I2C_ADDR 0x6B

/* =========================================================
 * Register Addresses
 * ========================================================= */
#define BQ25792_REG_MIN_SYS_VOLT 0x00   /* Minimal System Voltage          */
#define BQ25792_REG_CHG_VOLT_LIM 0x01   /* Charge Voltage Limit  (16-bit)  */
#define BQ25792_REG_CHG_CURR_LIM 0x03   /* Charge Current Limit  (16-bit)  */
#define BQ25792_REG_INPUT_VOLT_LIM 0x05 /* Input Voltage Limit             */
#define BQ25792_REG_INPUT_CURR_LIM 0x06 /* Input Current Limit   (16-bit)  */
#define BQ25792_REG_PRECHG_CTRL 0x08    /* Precharge Control               */
#define BQ25792_REG_TERM_CTRL 0x09      /* Termination Control             */
#define BQ25792_REG_RECHG_CTRL 0x0A     /* Re-charge Control               */
#define BQ25792_REG_VOTG_REG 0x0B       /* VOTG Regulation       (16-bit)  */
#define BQ25792_REG_IOTG_REG 0x0D       /* IOTG Regulation                 */
#define BQ25792_REG_TIMER_CTRL 0x0E     /* Timer Control                   */
#define BQ25792_REG_CHG_CTRL0 0x0F      /* Charger Control 0               */
#define BQ25792_REG_CHG_CTRL1 0x10      /* Charger Control 1               */
#define BQ25792_REG_CHG_CTRL2 0x11      /* Charger Control 2               */
#define BQ25792_REG_CHG_CTRL3 0x12      /* Charger Control 3               */
#define BQ25792_REG_CHG_CTRL4 0x13      /* Charger Control 4               */
#define BQ25792_REG_CHG_CTRL5 0x14      /* Charger Control 5               */
#define BQ25792_REG_RESERVED 0x15       /* Reserved                        */
#define BQ25792_REG_TEMP_CTRL 0x16      /* Temperature Control             */
#define BQ25792_REG_NTC_CTRL0 0x17      /* NTC Control 0                   */
#define BQ25792_REG_NTC_CTRL1 0x18      /* NTC Control 1                   */
#define BQ25792_REG_ICO_CURR_LIM 0x19   /* ICO Current Limit     (16-bit)  */
#define BQ25792_REG_CHG_STAT0 0x1B      /* Charger Status 0                */
#define BQ25792_REG_CHG_STAT1 0x1C      /* Charger Status 1                */
#define BQ25792_REG_CHG_STAT2 0x1D      /* Charger Status 2                */
#define BQ25792_REG_CHG_STAT3 0x1E      /* Charger Status 3                */
#define BQ25792_REG_CHG_STAT4 0x1F      /* Charger Status 4                */
#define BQ25792_REG_FAULT_STAT0 0x20    /* Fault Status 0                  */
#define BQ25792_REG_FAULT_STAT1 0x21    /* Fault Status 1                  */
#define BQ25792_REG_CHG_FLAG0 0x22      /* Charger Flag 0                  */
#define BQ25792_REG_CHG_FLAG1 0x23      /* Charger Flag 1                  */
#define BQ25792_REG_CHG_FLAG2 0x24      /* Charger Flag 2                  */
#define BQ25792_REG_CHG_FLAG3 0x25      /* Charger Flag 3                  */
#define BQ25792_REG_FAULT_FLAG0 0x26    /* Fault Flag 0                    */
#define BQ25792_REG_FAULT_FLAG1 0x27    /* Fault Flag 1                    */
#define BQ25792_REG_CHG_MASK0 0x28      /* Charger Mask 0                  */
#define BQ25792_REG_CHG_MASK1 0x29      /* Charger Mask 1                  */
#define BQ25792_REG_CHG_MASK2 0x2A      /* Charger Mask 2                  */
#define BQ25792_REG_FAULT_MASK0 0x2B    /* Fault Mask 0                    */
#define BQ25792_REG_FAULT_MASK1 0x2C    /* Fault Mask 1                    */
#define BQ25792_REG_ADC_CTRL 0x2E       /* ADC Control                     */
#define BQ25792_REG_ADC_DIS0 0x2F       /* ADC Function Disable 0          */
#define BQ25792_REG_ADC_DIS1 0x30       /* ADC Function Disable 1          */
#define BQ25792_REG_IBUS_ADC 0x31       /* IBUS ADC              (16-bit)  */
#define BQ25792_REG_IBAT_ADC 0x33       /* IBAT ADC              (16-bit)  */
#define BQ25792_REG_VBUS_ADC 0x35       /* VBUS ADC              (16-bit)  */
#define BQ25792_REG_VAC1_ADC 0x37       /* VAC1 ADC              (16-bit)  */
#define BQ25792_REG_VAC2_ADC 0x39       /* VAC2 ADC              (16-bit)  */
#define BQ25792_REG_VBAT_ADC 0x3B       /* VBAT ADC              (16-bit)  */
#define BQ25792_REG_VSYS_ADC 0x3D       /* VSYS ADC              (16-bit)  */
#define BQ25792_REG_TS_ADC 0x3F         /* TS ADC                (16-bit)  */
#define BQ25792_REG_TDIE_ADC 0x41       /* Die Temperature ADC   (16-bit)  */
#define BQ25792_REG_DPLUS_ADC 0x43      /* D+ ADC                (16-bit)  */
#define BQ25792_REG_DMINUS_ADC 0x45     /* D- ADC                (16-bit)  */
#define BQ25792_REG_DPDM_DRIVER 0x47    /* DPDM Driver                     */
#define BQ25792_REG_PART_INFO 0x48      /* Part Information                */

/* =========================================================
 * Bit-field masks — REG0x0F Charger Control 0
 * ========================================================= */
#define BQ25792_ENCHG_MASK (1 << 5)
#define BQ25792_ENHIZ_MASK (1 << 2)
#define BQ25792_ENTERM_MASK (1 << 1)
#define BQ25792_ENICO_MASK (1 << 4)
#define BQ25792_FORCEICO_MASK (1 << 3)
#define BQ25792_ENAUTOIBATDIS_MASK (1 << 7)
#define BQ25792_FORCEIBATDIS_MASK (1 << 6)

/* =========================================================
 * Bit-field masks — REG0x12 Charger Control 3
 * ========================================================= */
#define BQ25792_DISACDRV_MASK (1 << 7)
#define BQ25792_ENOTG_MASK (1 << 6)

/* =========================================================
 * Bit-field masks — REG0x13 Charger Control 4
 * ========================================================= */
#define BQ25792_ENACDRV2_MASK (1 << 7)
#define BQ25792_ENACDRV1_MASK (1 << 6)

/* =========================================================
 * Bit-field masks — REG0x11 Charger Control 2
 * ========================================================= */
#define BQ25792_SDRVCTRL_MASK (0x03 << 1)
#define BQ25792_SDRVCTRL_IDLE (0x00 << 1)
#define BQ25792_SDRVCTRL_SHUTDOWN (0x01 << 1)
#define BQ25792_SDRVCTRL_SHIP (0x02 << 1)
#define BQ25792_SDRVCTRL_SYSRST (0x03 << 1)

/* =========================================================
 * Bit-field masks — REG0x10 Charger Control 1
 * ========================================================= */
#define BQ25792_WATCHDOG_MASK (0x03 << 4)
#define BQ25792_WDRST_MASK (1 << 3)
#define BQ25792_VACOVP_MASK (0x03 << 6)

/* =========================================================
 * Bit-field masks — REG0x2E ADC Control
 * ========================================================= */
#define BQ25792_ADCEN_MASK (1 << 7)
#define BQ25792_ADCRATE_MASK (1 << 6) /* 0=continuous, 1=one-shot */
#define BQ25792_ADCAVG_MASK (1 << 5)
#define BQ25792_ADCSAMPLE_MASK (0x03 << 0)

/* =========================================================
 * VACOVP threshold options (REG0x10 bits [7:6])
 * ========================================================= */
typedef enum
{
    BQ25792_VACOVP_26V = 0x00, /* 26 V (default) */
    BQ25792_VACOVP_22V = 0x01, /* 22 V           */
    BQ25792_VACOVP_12V = 0x02, /* 12 V           */
    BQ25792_VACOVP_7V = 0x03,  /* 7 V            */
} bq25792_vacovp_t;

/* =========================================================
 * Ship FET / mode control
 * ========================================================= */
typedef enum
{
    BQ25792_MODE_IDLE = 0,
    BQ25792_MODE_SHUTDOWN = 1,
    BQ25792_MODE_SHIP = 2,
    BQ25792_MODE_SYSRST = 3,
} bq25792_mode_t;

/* =========================================================
 * VBUS/charger status codes (REG0x1C bits [7:5])
 * ========================================================= */
typedef enum
{
    BQ25792_VBUS_NO_INPUT = 0,
    BQ25792_VBUS_USB_SDP = 1,
    BQ25792_VBUS_USB_CDP = 2,
    BQ25792_VBUS_USB_DCP = 3,
    BQ25792_VBUS_HVDCP = 4,
    BQ25792_VBUS_UNKNOWN = 5,
    BQ25792_VBUS_NON_STD = 6,
    BQ25792_VBUS_OTG = 7,
} bq25792_vbus_stat_t;

/* =========================================================
 * Charge status codes (REG0x1C bits [4:2])
 * ========================================================= */
typedef enum
{
    BQ25792_CHG_NOT_CHARGING = 0,
    BQ25792_CHG_TRICKLE = 1,
    BQ25792_CHG_PRECHARGE = 2,
    BQ25792_CHG_FAST_CC = 3,
    BQ25792_CHG_TAPER_CV = 4,
    BQ25792_CHG_TOPOFF = 6,
    BQ25792_CHG_DONE = 7,
} bq25792_chg_stat_t;

// =========================================================================
// Определения констант и макросов для значений полей регистров BQ25792
// =========================================================================

// --- Регистры 00h - 1Fh ---

/**
 * @brief REG00: Minimal System Voltage Register (Offset 0x00)
 *        VSYSMIN_5_0 field settings.
 *        Formula: RegisterValue = (DesiredVoltage_mV - 2500u) / 250u
 *        Inverse: DesiredVoltage_mV = (RegisterValue * 250u) + 2500u
 */
#define BQ25792_REG00_VSYSMIN_mV_TO_REG(mV) ((uint8_t)(((mV) > 2500u ? (mV) - 2500u : 0u) / 250u))
#define BQ25792_REG00_VSYSMIN_REG_TO_mV(reg_val) ((uint16_t)((reg_val) * 250u) + 2500u)
// Example values based on cell count:
#define BQ25792_REG00_VSYSMIN_1S_VOLTAGE_REG_VAL 0x02 // 1s: 3V
#define BQ25792_REG00_VSYSMIN_2S_VOLTAGE_REG_VAL 0x0E // 2s: 6V
#define BQ25792_REG00_VSYSMIN_3S_VOLTAGE_REG_VAL 0x1A // 3s: 9V
#define BQ25792_REG00_VSYSMIN_4S_VOLTAGE_REG_VAL 0x26 // 4s: 12V

/**
 * @brief REG01: Charge Voltage Limit Register (Offset 0x01)
 *        VREG_10_0 field settings (Charge Voltage Limit).
 *        Formula: RegisterValue = DesiredVoltage_mV / 10u
 *        Inverse: DesiredVoltage_mV = RegisterValue * 10u
 *        Field size is 11 bits.
 */
#define BQ25792_REG01_VREG_mV_TO_REG(mV) ((uint16_t)((mV) / 10u))
#define BQ25792_REG01_VREG_REG_TO_mV(reg_val) ((uint16_t)((reg_val) * 10u))
// Example values based on cell count (from documentation):
// These are full 16-bit values for the register field VREG_10_0.
#define BQ25792_REG01_VREG_1S_VOLTAGE_REG_VAL 420u  // Example for 1s: 4.2V
#define BQ25792_REG01_VREG_2S_VOLTAGE_REG_VAL 840u  // Example for 2s: 8.4V
#define BQ25792_REG01_VREG_3S_VOLTAGE_REG_VAL 1260u // Example for 3s: 12.6V
#define BQ25792_REG01_VREG_4S_VOLTAGE_REG_VAL 1680u // Example for 4s: 16.8V

/**
 * @brief REG03: Charge Current Limit Register (Offset 0x03)
 *        ICHG_8_0 field settings (Charge Current Limit).
 *        Formula: RegisterValue = DesiredCurrent_mA / 10u
 *        Inverse: DesiredCurrent_mA = RegisterValue * 10u
 *        Field size is 9 bits.
 */
#define BQ25792_REG03_ICHG_mA_TO_REG(mA) ((uint16_t)((mA) / 10u))
#define BQ25792_REG03_ICHG_REG_TO_mA(reg_val) ((uint16_t)((reg_val) * 10u))

/**
 * @brief REG05: Input Voltage Limit Register (Offset 0x05)
 *        VINDPM_7_0 field settings (Input Voltage Limit).
 *        Formula: RegisterValue = DesiredVoltage_mV / 100u
 *        Inverse: DesiredVoltage_mV = RegisterValue * 100u
 *        Field size is 8 bits.
 */
#define BQ25792_REG05_VINDPM_mV_TO_REG(mV) ((uint8_t)((mV) / 100u))
#define BQ25792_REG05_VINDPM_REG_TO_mV(reg_val) ((uint16_t)((reg_val) * 100u))

/**
 * @brief REG06: Input Current Limit Register (Offset 0x06)
 *        IINDPM_8_0 field settings (Input Current Limit).
 *        Formula: RegisterValue = DesiredCurrent_mA / 10u
 *        Inverse: DesiredCurrent_mA = RegisterValue * 10u
 *        Field size is 9 bits.
 */
#define BQ25792_REG06_IINDPM_mA_TO_REG(mA) ((uint16_t)((mA) / 10u))
#define BQ25792_REG06_IINDPM_REG_TO_mA(reg_val) ((uint16_t)((reg_val) * 10u))
// Example values based on adapter type:
#define BQ25792_REG06_IINDPM_USB_SDP_500MA_REG_VAL 0x032      // 500mA
#define BQ25792_REG06_IINDPM_USB_CDP_1_5A_REG_VAL 0x096       // 1.5A
#define BQ25792_REG06_IINDPM_USB_DCP_3_25A_REG_VAL 0x145      // 3.25A
#define BQ25792_REG06_IINDPM_HV_DCP_1_5A_REG_VAL 0x096        // 1.5A
#define BQ25792_REG06_IINDPM_UNKNOWN_ADAPTER_3A_REG_VAL 0x12C // 3A

/**
 * @brief REG08: Precharge Control Register (Offset 0x08)
 *        VBAT_LOWV_1_0 field settings (Battery voltage thresholds for precharge to fast charge transition).
 *        These are mapped values, not direct formulas.
 */
#define BQ25792_REG08_VBAT_LOWV_15PERCENT_VREG 0x00   // 0h = 15%*VREG
#define BQ25792_REG08_VBAT_LOWV_62_2PERCENT_VREG 0x01 // 1h = 62.2%*VREG
#define BQ25792_REG08_VBAT_LOWV_66_7PERCENT_VREG 0x02 // 2h = 66.7%*VREG
#define BQ25792_REG08_VBAT_LOWV_71_4PERCENT_VREG 0x03 // 3h = 71.4%*VREG (default)

/**
 * @brief REG0A: Re-charge Control Register (Offset 0x0A)
 *        TRECHG field settings (Battery recharge deglitch time).
 *        These are mapped values.
 */
#define BQ25792_REG0A_TRECHG_64MS 0x00   // 0h = 64ms
#define BQ25792_REG0A_TRECHG_256MS 0x01  // 1h = 256ms
#define BQ25792_REG0A_TRECHG_1024MS 0x02 // 2h = 1024ms (default)
#define BQ25792_REG0A_TRECHG_2048MS 0x03 // 3h = 2048ms

/**
 * @brief REG0A: Re-charge Control Register (Offset 0x0A)
 *        CELL field settings (Battery cell count).
 *        These are mapped values.
 */
#define BQ25792_REG0A_CELL_COUNT_1S 0x00 // 0h = 1s
#define BQ25792_REG0A_CELL_COUNT_2S 0x01 // 1h = 2s
#define BQ25792_REG0A_CELL_COUNT_3S 0x02 // 2h = 3s
#define BQ25792_REG0A_CELL_COUNT_4S 0x03 // 3h = 4s

/**
 * @brief REG0B: VOTG Regulation Register (Offset 0x0B)
 *        VOTG_10_0 field settings (OTG mode regulation voltage).
 *        Formula: RegisterValue = (DesiredVoltage_mV - 2800u) / 10u
 *        Inverse: DesiredVoltage_mV = (RegisterValue * 10u) + 2800u
 *        Field size is 11 bits.
 */
#define BQ25792_REG0B_VOTG_mV_TO_REG(mV) ((uint16_t)(((mV) > 2800u ? (mV) - 2800u : 0u) / 10u))
#define BQ25792_REG0B_VOTG_REG_TO_mV(reg_val) ((uint16_t)((reg_val) * 10u) + 2800u)

/**
 * @brief REG0D: IOTG Regulation Register (Offset 0x0D)
 *        IOTG_6_0 field settings (OTG current limit).
 *        Formula: RegisterValue = DesiredCurrent_mA / 40u
 *        Inverse: DesiredCurrent_mA = RegisterValue * 40u
 *        Field size is 7 bits.
 */
#define BQ25792_REG0D_IOTG_mA_TO_REG(mA) ((uint8_t)(((mA) > 0u ? (mA) : 0u) / 40u)) // Min current is 120mA, so 0 is not a valid setting for range
#define BQ25792_REG0D_IOTG_REG_TO_mA(reg_val) ((uint16_t)((reg_val) * 40u))

/**
 * @brief REG0E: Timer Control Register (Offset 0x0E)
 *        TOPOFF_TMR_1_0 field settings (Top-off timer control).
 *        These are mapped values.
 */
#define BQ25792_REG0E_TOPOFF_TMR_DISABLED 0x00 // 0h = Disabled (default)
#define BQ25792_REG0E_TOPOFF_TMR_15MINS 0x01   // 1h = 15 mins
#define BQ25792_REG0E_TOPOFF_TMR_30MINS 0x02   // 2h = 30 mins
#define BQ25792_REG0E_TOPOFF_TMR_45MINS 0x03   // 3h = 45 mins

/**
 * @brief REG0E: Timer Control Register (Offset 0x0E)
 *        CHG_TMR field settings (Fast charge timer setting).
 *        These are mapped values.
 */
#define BQ25792_REG0E_CHG_TMR_5HRS 0x00  // 0h = 5 hrs
#define BQ25792_REG0E_CHG_TMR_8HRS 0x01  // 1h = 8 hrs
#define BQ25792_REG0E_CHG_TMR_12HRS 0x02 // 2h = 12 hrs (default)
#define BQ25792_REG0E_CHG_TMR_24HRS 0x03 // 3h = 24 hrs

/**
 * @brief REG10: Charger Control 1 Register (Offset 0x10)
 *        WATCHDOG_2_0 field settings (Watchdog timer settings).
 *        These are mapped values.
 */
#define BQ25792_REG10_WATCHDOG_DISABLE 0x00 // 0h = Disable
#define BQ25792_REG10_WATCHDOG_0_5S 0x01    // 1h = 0.5s
#define BQ25792_REG10_WATCHDOG_1S 0x02      // 2h = 1s
#define BQ25792_REG10_WATCHDOG_2S 0x03      // 3h = 2s
#define BQ25792_REG10_WATCHDOG_20S 0x04     // 4h = 20s
#define BQ25792_REG10_WATCHDOG_40S 0x05     // 5h = 40s (default)
#define BQ25792_REG10_WATCHDOG_80S 0x06     // 6h = 80s
#define BQ25792_REG10_WATCHDOG_160S 0x07    // 7h = 160s

/**
 * @brief REG10: Charger Control 1 Register (Offset 0x10)
 *        VAC_OVP_1_0 field settings (VAC_OVP thresholds).
 *        These are mapped values.
 */
#define BQ25792_REG10_VAC_OVP_26V 0x00 // 0h = 26V (default)
#define BQ25792_REG10_VAC_OVP_18V 0x01 // 1h = 18V
#define BQ25792_REG10_VAC_OVP_12V 0x02 // 2h = 12V
#define BQ25792_REG10_VAC_OVP_7V 0x03  // 3h = 7V

/**
 * @brief REG11: Charger Control 2 Register (Offset 0x11)
 *        SDRV_CTRL_1_0 field settings (SFET control modes).
 *        These are mapped values.
 */
#define BQ25792_REG11_SFET_CTRL_IDLE 0x00      // 00b = IDLE (default)
#define BQ25792_REG11_SFET_CTRL_SHUTDOWN 0x01  // 01b = Shutdown Mode
#define BQ25792_REG11_SFET_CTRL_SHIP 0x02      // 02b = Ship Mode
#define BQ25792_REG11_SFET_CTRL_SYS_RESET 0x03 // 03b = System Power Reset

/**
 * @brief REG14: Charger Control 5 Register (Offset 0x14)
 *        IBAT_REG_1_0 field settings (Battery discharging current regulation in OTG mode).
 *        These are mapped values.
 */
#define BQ25792_REG14_IBAT_REG_3A 0x00      // 00b = 3A
#define BQ25792_REG14_IBAT_REG_4A 0x01      // 01b = 4A
#define BQ25792_REG14_IBAT_REG_5A 0x02      // 02b = 5A (default)
#define BQ25792_REG14_IBAT_REG_DISABLE 0x03 // 03b = Disable

/**
 * @brief REG16: Temperature Control Register (Offset 0x16)
 *        TREG_1_0 field settings (Thermal regulation thresholds).
 *        These are mapped values.
 */
#define BQ25792_REG16_TREG_60C 0x00  // 0h = 60°C
#define BQ25792_REG16_TREG_80C 0x01  // 1h = 80°C
#define BQ25792_REG16_TREG_100C 0x02 // 2h = 100°C
#define BQ25792_REG16_TREG_120C 0x03 // 3h = 120°C (default)

/**
 * @brief REG16: Temperature Control Register (Offset 0x16)
 *        TSHUT_1_0 field settings (Thermal shutdown thresholds).
 *        These are mapped values.
 */
#define BQ25792_REG16_TSHUT_150C 0x00 // 0h = 150°C (default)
#define BQ25792_REG16_TSHUT_130C 0x01 // 1h = 130°C
#define BQ25792_REG16_TSHUT_120C 0x02 // 2h = 120°C
#define BQ25792_REG16_TSHUT_85C 0x03  // 3h = 85°C

/**
 * @brief REG18: NTC Control 1 Register (Offset 0x18)
 *        BHOT_1_0 field settings (OTG mode TS HOT temperature threshold).
 *        These are mapped values.
 */
#define BQ25792_REG18_BHOT_55C 0x00 // 0h = 55°C
#define BQ25792_REG18_BHOT_60C 0x01 // 1h = 60°C (default)
#define BQ25792_REG18_BHOT_65C 0x02 // 2h = 65°C
// 3h is defined as "Disable"

/**
 * @brief REG1C: Charger Status 1 Register (Offset 0x1C)
 *        VBUS_STAT_3_0 field settings (VBUS status).
 *        These are mapped values.
 */
#define BQ25792_REG1C_VBUS_STAT_NO_INPUT 0x00        // Oh: No Input or BHOT or BCOLD in OTG mode
#define BQ25792_REG1C_VBUS_STAT_USB_SDP 0x01         // 1h: USB SDP (500mA)
#define BQ25792_REG1C_VBUS_STAT_USB_CDP 0x02         // 2h: USB CDP (1.5A)
#define BQ25792_REG1C_VBUS_STAT_USB_DCP 0x03         // 3h: USB DCP (3.25A)
#define BQ25792_REG1C_VBUS_STAT_HV_DCP 0x04          // 4h: Adjustable High Voltage DCP (HVDCP) (1.5A)
#define BQ25792_REG1C_VBUS_STAT_UNKNOWN_ADAPTER 0x05 // 5h: Unknown adaptor (3A)
#define BQ25792_REG1C_VBUS_STAT_OTG_MODE 0x07        // 7h: In OTG mode
// Other values (8h-Fh) are reserved or have specific meanings like "Device directly powered from VBUS".

/**
 * @brief REG2E: ADC Control Register (Offset 0x2E)
 *        ADC_SAMPLE_1_0 field settings (ADC sample speed / resolution).
 *        These are mapped values.
 */
#define BQ25792_REG2E_ADC_SAMPLE_15BIT 0x00 // 0h = 15 bit effective resolution
#define BQ25792_REG2E_ADC_SAMPLE_14BIT 0x01 // 1h = 14 bit effective resolution
#define BQ25792_REG2E_ADC_SAMPLE_13BIT 0x02 // 2h = 13 bit effective resolution
#define BQ25792_REG2E_ADC_SAMPLE_12BIT 0x03 // 3h = 12 bit effective resolution

/**
 * @brief REG47: DPDM Driver Register (Offset 0x47)
 *        DPLUS_DAC_2_0 field settings (D+ Output Driver voltage).
 *        These are mapped values.
 */
#define BQ25792_REG47_DPLUS_DAC_HIZ 0x00   // 0h = HIZ
#define BQ25792_REG47_DPLUS_DAC_0V 0x01    // 1h = 0V
#define BQ25792_REG47_DPLUS_DAC_0_6V 0x02  // 2h = 0.6V
#define BQ25792_REG47_DPLUS_DAC_1_2V 0x03  // 3h = 1.2V
#define BQ25792_REG47_DPLUS_DAC_2_0V 0x04  // 4h = 2.0V
#define BQ25792_REG47_DPLUS_DAC_2_7V 0x05  // 5h = 2.7V
#define BQ25792_REG47_DPLUS_DAC_3_3V 0x06  // 6h = 3.3V
#define BQ25792_REG47_DPLUS_DAC_SHORT 0x07 // 7h = D+/D- Short

/**
 * @brief REG47: DPDM Driver Register (Offset 0x47)
 *        DMINUS_DAC_2_0 field settings (D- Output Driver voltage).
 *        These are mapped values.
 */
#define BQ25792_REG47_DMINUS_DAC_HIZ 0x00  // 0h = HIZ
#define BQ25792_REG47_DMINUS_DAC_0V 0x01   // 1h = 0V
#define BQ25792_REG47_DMINUS_DAC_0_6V 0x02 // 2h = 0.6V
#define BQ25792_REG47_DMINUS_DAC_1_2V 0x03 // 3h = 1.2V
#define BQ25792_REG47_DMINUS_DAC_2_0V 0x04 // 4h = 2.0V
#define BQ25792_REG47_DMINUS_DAC_2_7V 0x05 // 5h = 2.7V
#define BQ25792_REG47_DMINUS_DAC_3_3V 0x06 // 6h = 3.3V
// 7h is reserved

// --- Определения типов регистров ---

// =========================================================================
// Регистры из диапазона 00h - 1Fh (с учетом 16-битных)
// =========================================================================

/**
 * @brief REG00: Minimal System Voltage Register (Offset 0x00)
 *        Sets the minimum system voltage threshold.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t VSYSMIN : 6;      // Bits 5-0: Minimal System Voltage setting.
        uint8_t RESERVED_7_6 : 2; // Bits 7-6: Reserved.
    } BITS;
} BQ25792_REG00_Minimal_System_Voltage_t;

/**
 * @brief REG01: Charge Voltage Limit Register (Offset 0x01)
 *        Configures the charge voltage limit.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint16_t VREG : 11;          // Charge Voltage Limit setting.
        uint16_t RESERVED_15_11 : 5; // Bits 15-11 (MSB): Reserved.
    } BITS;
} BQ25792_REG01_Charge_Voltage_Limit_t;

/**
 * @brief REG03: Charge Current Limit Register (Offset 0x03)
 *        Configures the charge current limit.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint16_t ICHG : 9;          // Charge Current Limit setting.
        uint16_t RESERVED_15_9 : 7; // Bits 15-9 (MSB): Reserved.
    } BITS;
} BQ25792_REG03_Charge_Current_Limit_t;

/**
 * @brief REG05: Input Voltage Limit Register (Offset 0x05)
 *        Configures the input voltage limit.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t VINDPM : 8; // Bits 7-0: Input Voltage Limit setting (VINDPM Threshold).
    } BITS;
} BQ25792_REG05_Input_Voltage_Limit_t;

/**
 * @brief REG06: Input Current Limit Register (Offset 0x06)
 *        Configures the input current limit.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint16_t IINDPM : 9;        // Input Current Limit setting.
        uint16_t RESERVED_15_9 : 7; // Bits 15-9 (MSB): Reserved.
    } BITS;
} BQ25792_REG06_Input_Current_Limit_t;

/**
 * @brief REG08: Precharge Control Register (Offset 0x08)
 *        Controls pre-charge settings.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t IPRECHG : 6;   // Bits 5-0: Precharge current limit.
        uint8_t VBAT_LOWV : 2; // Bits 7-6: Battery voltage thresholds.
    } BITS;
} BQ25792_REG08_Precharge_Control_t;

/**
 * @brief REG09: Termination Control Register (Offset 0x09)
 *        Controls charging termination settings.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t ITERM : 5;      // Bits 4-0: Lower 5 bits of ITERM.
        uint8_t RESERVED_5 : 1; // Bit 5: Reserved.
        uint8_t REG_RST : 1;    // Bit 6: Reset registers.
        uint8_t RESERVED_7 : 1; // Bit 7: Reserved.
    } BITS;
} BQ25792_REG09_Termination_Control_t;

/**
 * @brief REG0A: Re-charge Control Register (Offset 0x0A)
 *        Configures recharge settings.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t VRECHG : 4; // Bits 3-0: Battery Recharge Threshold Offset setting.
        uint8_t TRECHG : 2; // Bits 5-4: Battery recharge deglitch time setting.
        uint8_t CELL : 2;   // Bits 7-6: Battery cell count setting.
    } BITS;
} BQ25792_REG0A_Recharge_Control_t;

/**
 * @brief REG0B: VOTG Regulation Register (Offset 0x0B)
 *        Sets the OTG mode regulation voltage (VOTG).
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint16_t VOTG : 11;        // VOTG setting.
        uint16_t RESERVED_MSB : 5; // Bits 15-11 (MSB): Reserved.
    } BITS;
} BQ25792_REG0B_VOTG_regulation_t;

/**
 * @brief REG0D: IOTG Regulation Register (Offset 0x0D)
 *        Sets the OTG mode current regulation limit (IOTG).
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t IOTG : 7;       // Bits 6-0: OTG current limit setting.
        uint8_t PRECHG_TMR : 1; // Bit 7: Pre-charge safety timer setting.
    } BITS;
} BQ25792_REG0D_IOTG_regulation_t;

/**
 * @brief REG0E: Timer Control Register (Offset 0x0E)
 *        Controls various timers.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t TMR2X_EN : 1;       // Bit 0: TMR2X Enable control.
        uint8_t CHG_TMR : 2;        // Bits 2-1: Fast charge timer setting.
        uint8_t EN_CHG_TMR : 1;     // Bit 3: Enable fast charge timer.
        uint8_t EN_PRECHG_TMR : 1;  // Bit 4: Enable pre-charge timer.
        uint8_t EN_TRICHG_TMR : 1;  // Bit 5: Enable trickle charge timer.
        uint8_t TOPOFF_TMR_1_0 : 2; // Bits 7-6: Top-off timer control.
    } BITS;
} BQ25792_REG0E_Timer_Control_t;

/**
 * @brief REG0F: Charger Control 0 Register (Offset 0x0F)
 *        Controls charging enable, ICO, HIZ, etc.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t RESERVED_0 : 1;      // Bit 0: Reserved.
        uint8_t EN_TERM : 1;         // Bit 1: Enable Termination.
        uint8_t EN_HIZ : 1;          // Bit 2: Enable HIZ mode.
        uint8_t FORCE_ICO : 1;       // Bit 3: Force start Input Current Optimizer.
        uint8_t EN_ICO : 1;          // Bit 4: Input Current Optimizer (ICO) Enable.
        uint8_t EN_CHG : 1;          // Bit 5: Charger Enable Configuration.
        uint8_t FORCE_IBATDIS : 1;   // Bit 6: Force battery discharging current.
        uint8_t EN_AUTO_IBATDIS : 1; // Bit 7: Enable auto battery discharging during OVP.
    } BITS;
} BQ25792_REG0F_Charger_Control_0_t;

/**
 * @brief REG10: Charger Control 1 Register (Offset 0x10)
 *        Controls VAC over-voltage thresholds and watchdog timer settings.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t WATCHDOG_2_0 : 3; // Bits 2-0: Watchdog timer settings.
        uint8_t WD_RST : 1;       // Bit 3: I2C watchdog timer reset.
        uint8_t VAC_OVP_1_0 : 2;  // Bits 5-4: VAC_OVP thresholds.
        uint8_t RESERVED_6 : 1;   // Bit 6: Reserved.
        uint8_t RESERVED_7 : 1;   // Bit 7: Reserved.
    } BITS;
} BQ25792_REG10_Charger_Control_1_t;

/**
 * @brief REG11: Charger Control 2 Register (Offset 0x11)
 *        Controls D+/D- detection, HVDC settings, and ship FET control.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t SDRV_DLY : 1;      // Bit 0: Delay time added to the action in SDRV_CTRL.
        uint8_t SDRV_CTRL_1_0 : 2; // Bits 2-1: SFET control (ship FET modes).
        uint8_t HVDCP_EN : 1;      // Bit 3: High voltage DCP enable.
        uint8_t EN_9V : 1;         // Bit 4: Enable 9V HVDC mode.
        uint8_t EN_12V : 1;        // Bit 5: Enable 12V HVDC mode.
        uint8_t AUTO_INDET_EN : 1; // Bit 6: Automatic D+/D- Detection Enable.
        uint8_t FORCE_INDET : 1;   // Bit 7: Force D+/D- detection.
    } BITS;
} BQ25792_REG11_Charger_Control_2_t;

/**
 * @brief REG12: Charger Control 3 Register (Offset 0x12)
 *        Controls various modes: OTG, PFM, WKUP_DLY, LDO disable, OOA disable.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t DIS_FWD_OOA : 1; // Bit 0: Disable OOA in forward mode.
        uint8_t DIS_OTG_OOA : 1; // Bit 1: Disable OOA in OTG mode.
        uint8_t DIS_LDO : 1;     // Bit 2: Disable BATFET LDO mode in pre-charge stage.
        uint8_t WKUP_DLY : 1;    // Bit 3: Wake up delay time from ship mode.
        uint8_t PFM_FWD_DIS : 1; // Bit 4: Disable PFM in forward mode.
        uint8_t PFM_OTG_DIS : 1; // Bit 5: Disable PFM in OTG mode.
        uint8_t EN_OTG : 1;      // Bit 6: OTG mode control.
        uint8_t DIS_ACDRV : 1;   // Bit 7: When set, forces ACDRV1/2 off.
    } BITS;
} BQ25792_REG12_Charger_Control_3_t;

/**
 * @brief REG13: Charger Control 4 Register (Offset 0x13)
 *        Controls ACDRV gate drivers, PWM frequency, STAT output, etc.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t EN_IBUS_OCP : 1;      // Bit 0: Enable IBUS_OCP in forward mode.
        uint8_t FORCE_VINDPM_DET : 1; // Bit 1: Force VINDPM detection.
        uint8_t DIS_VOTG_UVP : 1;     // Bit 2: Disable OTG mode VOTG UVP hiccup protection.
        uint8_t DIS_VSYS_SHORT : 1;   // Bit 3: Disable forward mode VSYS short hiccup protection.
        uint8_t DIS_STAT : 1;         // Bit 4: Disable the STAT pin output.
        uint8_t PWM_FREQ : 1;         // Bit 5: Switching frequency selection.
        uint8_t EN_ACDRV1 : 1;        // Bit 6: External ACFET1-RBFET1 gate driver control.
        uint8_t EN_ACDRV2 : 1;        // Bit 7: External ACFET2-RBFET2 gate driver control.
    } BITS;
} BQ25792_REG13_Charger_Control_4_t;

/**
 * @brief REG14: Charger Control 5 Register (Offset 0x14)
 *        Controls ship FET presence, IBAT sensing, etc.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t EN_BATOC : 1;     // Bit 0: Enable battery discharging current OCP.
        uint8_t EN_EXTILIM : 1;   // Bit 1: Enable the external ILIM_HIZ pin input current regulation.
        uint8_t EN_IINDPM : 1;    // Bit 2: Enable the internal IINDPM register input current regulation.
        uint8_t IBAT_REG_1_0 : 2; // Bits 3-4: Battery discharging current regulation in OTG mode.
        uint8_t EN_IBAT : 1;      // Bit 5: IBAT current sensing enable.
        uint8_t RESERVED_6 : 1;   // Bit 6: Reserved.
        uint8_t SFET_PRESENT : 1; // Bit 7: Indicates if a ship FET is populated.
    } BITS;
} BQ25792_REG14_Charger_Control_5_t;

/**
 * @brief REG15: Reserved Register (Offset 0x15)
 *        Reserved register.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t RESERVED_7_0 : 8; // Bits 7-0: Reserved.
    } BITS;
} BQ25792_REG15_Reserved_t;

/**
 * @brief REG16: Temperature Control Register (Offset 0x16)
 *        Controls temperature-related settings.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_0 : 1; // Bit 0: Reserved.
        uint8_t VAC2_PD_EN : 1; // Bit 1: Enable VAC2 pull down.
        uint8_t VAC1_PD_EN : 1; // Bit 2: Enable VAC1 pull down.
        uint8_t VBUS_PD_EN : 1; // Bit 3: Enable VBUS pull down.
        uint8_t TSHUT_1_0 : 2;  // Bits 5-4: Thermal shutdown thresholds.
        uint8_t TREG_1_0 : 2;   // Bits 7-6: Thermal regulation thresholds.
    } BITS;
} BQ25792_REG16_Temperature_Control_t;

/**
 * @brief REG17: NTC Control 0 Register (Offset 0x17)
 *        Configures NTC thermistor settings for JEITA profiles.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_0 : 1;      // Bit 0: Reserved.
        uint8_t JEITA_ISETC_1_0 : 2; // Bits 2-1: JEITA low temp current.
        uint8_t JEITA_ISETH_1_0 : 2; // Bits 4-3: JEITA high temp current.
        uint8_t JEITA_VSET_2_0 : 3;  // Bits 7-5: JEITA high temp voltage.
    } BITS;
} BQ25792_REG17_NTC_Control_0_t;

/**
 * @brief REG18: NTC Control 1 Register (Offset 0x18)
 *        Configures NTC thermistor settings for JEITA profiles and OTG TS thresholds.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TS_IGNORE : 1;   // Bit 0: Ignore TS feedback.
        uint8_t BCOLD : 1;       // Bit 1: OTG TS COLD threshold.
        uint8_t BHOT_1_0 : 2;    // Bits 3-2: OTG TS HOT threshold.
        uint8_t TS_WARM_1_0 : 2; // Bits 5-4: JEITA VT3 thresholds.
        uint8_t TS_COOL_1_0 : 2; // Bits 7-6: JEITA VT2 thresholds.
    } BITS;
} BQ25792_REG18_NTC_Control_1_t;

/**
 * @brief REG19: ICO Current Limit Register (Offset 0x19)
 *        Sets the Input Current Optimizer (ICO) current limit.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint16_t ICO_ILIM : 9;      // Bits 8-0  ICO current limit setting.
        uint16_t RESERVED_15_9 : 7; // Bits 15-9 (MSB): Reserved.
    } BITS;
} BQ25792_REG19_ICO_Current_Limit_t;

/**
 * @brief REG1B: Charger Status 0 Register (Offset 0x1B)
 *        Provides status information about the charger operation.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VBUS_PRESENT_STAT : 1; // Bit 0: VBUS present.
        uint8_t AC1_PRESENT_STAT : 1;  // Bit 1: VAC1 present.
        uint8_t AC2_PRESENT_STAT : 1;  // Bit 2: VAC2 present.
        uint8_t PG_STAT : 1;           // Bit 3: Power good.
        uint8_t POORSRC_STAT : 1;      // Bit 4: Poor source.
        uint8_t WD_STAT : 1;           // Bit 5: Watchdog status.
        uint8_t VINDPM_STAT : 1;       // Bit 6: VINDPM status.
        uint8_t INIDPM_STAT : 1;       // Bit 7: IINDPM status.
    } BITS;
} BQ25792_REG1B_Charger_Status_0_t;

/**
 * @brief REG1C: Charger Status 1 Register (Offset 0x1C)
 *        Provides status information about the charger operation.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t BC1_2_DONE_STAT : 1; // Bit 0: BC1.2 done.
        uint8_t VBUS_STAT_3_0 : 4;   // Bits 4-1: VBUS status.
        uint8_t CHG_STAT_2_0 : 3;    // Bits 7-5: Charge status.
    } BITS;
} BQ25792_REG1C_Charger_Status_1_t;

/**
 * @brief REG1D: Charger Status 2 Register (Offset 0x1D)
 *        Provides status information about the charger operation.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VBAT_PRESENT_STAT : 1; // Bit 0: Battery present.
        uint8_t TPDM_STAT : 1;         // Bit 1: D+/D- detection.
        uint8_t TREG_STAT : 1;         // Bit 2: Thermal regulation.
        uint8_t RESERVED_5_3 : 3;      // Bits 5-3: Reserved.
        uint8_t ICO_STAT_1_0 : 2;      // Bits 7-6: ICO status.
    } BITS;
} BQ25792_REG1D_Charger_Status_2_t;

/**
 * @brief REG1E: Charger Status 3 Register (Offset 0x1E)
 *        Provides status information about the charger operation.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_0 : 1;      // Bit 0: Reserved.
        uint8_t PRECHG_TMR_STAT : 1; // Bit 1: Precharge timer.
        uint8_t TRICHG_TMR_STAT : 1; // Bit 2: Trickle timer.
        uint8_t CHG_TMR_STAT : 1;    // Bit 3: Fast charge timer.
        uint8_t VSYS_STAT : 1;       // Bit 4: VSYS regulation.
        uint8_t ADC_DONE_STAT : 1;   // Bit 5: ADC done.
        uint8_t ACRB1_STAT : 1;      // Bit 6: ACFET1 status.
        uint8_t ACRB2_STAT : 1;      // Bit 7: ACFET2 status.
    } BITS;
} BQ25792_REG1E_Charger_Status_3_t;

/**
 * @brief REG1F: Charger Status 4 Register (Offset 0x1F)
 *        Provides status information about the charger operation.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TS_HOT_STAT : 1;      // Bit 0: TS hot.
        uint8_t TS_WARM_STAT : 1;     // Bit 1: TS warm.
        uint8_t TS_COOL_STAT : 1;     // Bit 2: TS cool.
        uint8_t RESERVED_3 : 1;       // Bit 3: Reserved.
        uint8_t VBATOTG_LOW_STAT : 1; // Bit 4: VBAT too low for OTG.
        uint8_t RESERVED_7_5 : 3;     // Bits 7-5: Reserved.
    } BITS;
} BQ25792_REG1F_Charger_Status_4_t;

/**
 * @brief REG20: FAULT Status 0 Register (Offset 0x20)
 *        Contains fault status flags.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VAC1_OVP_STAT : 1; // Bit 0: VAC1 OVP.
        uint8_t VAC2_OVP_STAT : 1; // Bit 1: VAC2 OVP.
        uint8_t CONV_OCP_STAT : 1; // Bit 2: Converter OCP.
        uint8_t IBAT_OCP_STAT : 1; // Bit 3: IBAT OCP.
        uint8_t IBUS_OCP_STAT : 1; // Bit 4: IBUS OCP.
        uint8_t VBAT_OVP_STAT : 1; // Bit 5: VBAT OVP.
        uint8_t VBUS_OVP_STAT : 1; // Bit 6: VBUS OVP.
        uint8_t IBAT_REG_STAT : 1; // Bit 7: IBAT regulation.
    } BITS;
} BQ25792_REG20_FAULT_Status_0_t;

/**
 * @brief REG21: FAULT Status 1 Register (Offset 0x21)
 *        Contains fault status flags.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_1_0 : 2;    // Bits 1-0: Reserved.
        uint8_t TSHUT_STAT : 1;      // Bit 2: Thermal shutdown.
        uint8_t RESERVED_3 : 1;      // Bit 3: Reserved.
        uint8_t OTG_UVP_STAT : 1;    // Bit 4: OTG UVP.
        uint8_t OTG_OVP_STAT : 1;    // Bit 5: OTG OVP.
        uint8_t VSYS_OVP_STAT : 1;   // Bit 6: VSYS OVP.
        uint8_t VSYS_SHORT_STAT : 1; // Bit 7: VSYS short.
    } BITS;
} BQ25792_REG21_FAULT_Status_1_t;

// --- Регистры 22h - 27h (Charger Flags) ---

/**
 * @brief REG22: Charger Flag 0 Register (Offset 0x22)
 *        Contains flags related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VBUS_PRESENT_FLAG : 1;
        uint8_t AC1_PRESENT_FLAG : 1;
        uint8_t AC2_PRESENT_FLAG : 1;
        uint8_t PG_FLAG : 1;
        uint8_t POORSRC_FLAG : 1;
        uint8_t WD_FLAG : 1;
        uint8_t VINDPM_FLAG : 1;
        uint8_t IINDPM_FLAG : 1;
    } BITS;
} BQ25792_REG22_Charger_Flag_0_t;

/**
 * @brief REG23: Charger Flag 1 Register (Offset 0x23)
 *        Contains flags related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t BC1_2_DONE_FLAG : 1;   // Bit 0: BC1.2 detection status flag.
        uint8_t VBAT_PRESENT_FLAG : 1; // Bit 1: VBAT present flag.
        uint8_t TREG_FLAG : 1;         // Bit 2: IC thermal regulation flag.
        uint8_t RESERVED_3 : 1;        // Bit 3: Reserved.
        uint8_t VBUS_FLAG : 1;         // Bit 4: VBUS status flag.
        uint8_t RESERVED_5 : 1;        // Bit 5: Reserved.
        uint8_t ICO_FLAG : 1;          // Bit 6: ICO status flag.
        uint8_t CHG_FLAG : 1;          // Bit 7: Charge status flag.
    } BITS;

} BQ25792_REG23_Charger_Flag_1_t;

/**
 * @brief REG24: Charger Flag 2 Register (Offset 0x24)
 *        Contains flags related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TOPOFF_TMR_FLAG : 1;
        uint8_t PRECHG_TMR_FLAG : 1;
        uint8_t TRICHG_TMR_FLAG : 1;
        uint8_t CHG_TMR_FLAG : 1;
        uint8_t VSYS_FLAG : 1;
        uint8_t ADC_DONE_FLAG : 1;
        uint8_t DPDM_DONE_FLAG : 1;
        uint8_t RESERVED_7 : 1;
    } BITS;
} BQ25792_REG24_Charger_Flag_2_t;

/**
 * @brief REG25: Charger Flag 3 Register (Offset 0x25)
 *        Contains flags related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TS_HOT_FLAG : 1;
        uint8_t TS_WARM_FLAG : 1;
        uint8_t TS_COOL_FLAG : 1;
        uint8_t TS_COLD_FLAG : 1;
        uint8_t VBATOTG_LOW_FLAG : 1;
        uint8_t RESERVED_5 : 1;
        uint8_t RESERVED_6 : 1;
        uint8_t RESERVED_7 : 1;
    } BITS;
} BQ25792_REG25_Charger_Flag_3_t;

/**
 * @brief REG26: FAULT Flag 0 Register (Offset 0x26)
 *        Contains fault flags related to various protection mechanisms.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VAC1_OVP_FLAG : 1;
        uint8_t VAC2_OVP_FLAG : 1;
        uint8_t CONV_OCP_FLAG : 1;
        uint8_t IBAT_OCP_FLAG : 1;
        uint8_t IBUS_OCP_FLAG : 1;
        uint8_t VBAT_OVP_FLAG : 1;
        uint8_t VBUS_OVP_FLAG : 1;
        uint8_t IBAT_REG_FLAG : 1;
    } BITS;
} BQ25792_REG26_FAULT_Flag_0_t;

/**
 * @brief REG27: FAULT Flag 1 Register (Offset 0x27)
 *        Contains fault flags related to various protection mechanisms.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_1_0 : 2;
        uint8_t TSHUT_FLAG : 1;
        uint8_t RESERVED_3 : 1;
        uint8_t OTG_UVP_FLAG : 1;
        uint8_t OTG_OVP_FLAG : 1;
        uint8_t VSYS_OVP_FLAG : 1;
        uint8_t VSYS_SHORT_FLAG : 1;
    } BITS;
} BQ25792_REG27_FAULT_Flag_1_t;

/**
 * @brief REG28: Charger Mask 0 Register (Offset 0x28)
 *        Contains masks related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VBUS_PRESENT_MASK : 1;
        uint8_t AC1_PRESENT_MASK : 1;
        uint8_t AC2_PRESENT_MASK : 1;
        uint8_t PG_MASK : 1;
        uint8_t POORSRC_MASK : 1;
        uint8_t WD_MASK : 1;
        uint8_t VINDPM_MASK : 1;
        uint8_t IINDPM_MASK : 1;
    } BITS;
} BQ25792_REG28_Charger_Mask_0_t;

/**
 * @brief REG29: Charger Mask 1 Register (Offset 0x29)
 *        Contains masks related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t BC1_2_DONE_MASK : 1;   // Bit 0: BC1.2 detection status mask.
        uint8_t VBAT_PRESENT_MASK : 1; // Bit 1: VBAT present mask.
        uint8_t TREG_MASK : 1;         // Bit 2: IC thermal regulation mask.
        uint8_t RESERVED_3 : 1;        // Bit 3: Reserved.
        uint8_t VBUS_MASK : 1;         // Bit 4: VBUS status mask.
        uint8_t RESERVED_5 : 1;        // Bit 5: Reserved.
        uint8_t ICO_MASK : 1;          // Bit 6: ICO status mask.
        uint8_t CHG_MASK : 1;          // Bit 7: Charge status mask.
    } BITS;
} BQ25792_REG29_Charger_Mask_1_t;

/**
 * @brief REG2A: Charger Mask 2 Register (Offset 0x2A)
 *        Contains masks related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TOPOFF_TMR_MASK : 1;
        uint8_t PRECHG_TMR_MASK : 1;
        uint8_t TRICHG_TMR_MASK : 1;
        uint8_t CHG_TMR_MASK : 1;
        uint8_t VSYS_MASK : 1;
        uint8_t ADC_DONE_MASK : 1;
        uint8_t DPDM_DONE_MASK : 1;
        uint8_t RESERVED_7 : 1;
    } BITS;
} BQ25792_REG2A_Charger_Mask_2_t;

/**
 * @brief REG2B: Charger Mask 3 Register (Offset 0x2B)
 *        Contains masks related to charger status events.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t TS_HOT_MASK : 1;
        uint8_t TS_WARM_MASK : 1;
        uint8_t TS_COOL_MASK : 1;
        uint8_t TS_COLD_MASK : 1;
        uint8_t VBATOTG_LOW_MASK : 1;
        uint8_t RESERVED_5 : 1;
        uint8_t RESERVED_6 : 1;
        uint8_t RESERVED_7 : 1;
    } BITS;
} BQ25792_REG2B_Charger_Mask_3_t;

/**
 * @brief REG2C: FAULT Mask 0 Register (Offset 0x2C)
 *        Contains fault masks related to various protection mechanisms.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t VAC1_OVP_MASK : 1;
        uint8_t VAC2_OVP_MASK : 1;
        uint8_t CONV_OCP_MASK : 1;
        uint8_t IBAT_OCP_MASK : 1;
        uint8_t IBUS_OCP_MASK : 1;
        uint8_t VBAT_OVP_MASK : 1;
        uint8_t VBUS_OVP_MASK : 1;
        uint8_t IBAT_REG_MASK : 1;
    } BITS;
} BQ25792_REG2C_FAULT_Mask_0_t;

/**
 * @brief REG2D: FAULT Mask 1 Register (Offset 0x2D)
 *        Contains fault masks related to various protection mechanisms.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_1_0 : 2;
        uint8_t TSHUT_MASK : 1;
        uint8_t RESERVED_3 : 1;
        uint8_t OTG_UVP_MASK : 1;
        uint8_t OTG_OVP_MASK : 1;
        uint8_t VSYS_OVP_MASK : 1;
        uint8_t VSYS_SHORT_MASK : 1;
    } BITS;
} BQ25792_REG2D_FAULT_Mask_1_t;

/**
 * @brief REG2E: ADC Control Register (Offset 0x2E)
 *        Controls ADC operation parameters.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_1_0 : 2;   // Bits 1-0: Reserved.
        uint8_t ADC_AVG_INIT : 1;   // Bit 2: ADC average init.
        uint8_t ADC_AVG : 1;        // Bit 3: ADC average.
        uint8_t ADC_SAMPLE_1_0 : 2; // Bits 5-4: ADC sample rate.
        uint8_t ADC_RATE : 1;       // Bit 6: ADC rate.
        uint8_t ADC_EN : 1;         // Bit 7: ADC enable.
    } BITS;
} BQ25792_REG2E_ADC_Control_t;

/**
 * @brief REG2F: ADC Function Disable 0 Register (Offset 0x2F)
 *        Disables specific ADC functions.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED : 1;     // Bit 0: Reserved.
        uint8_t TDIE_ADC_DIS : 1; // Bit 1: TDIE ADC disable.
        uint8_t TS_ADC_DIS : 1;   // Bit 2: TS ADC disable.
        uint8_t VSYS_ADC_DIS : 1; // Bit 3: VSYS ADC disable.
        uint8_t VBAT_ADC_DIS : 1; // Bit 4: VBAT ADC disable.
        uint8_t VBUS_ADC_DIS : 1; // Bit 5: VBUS ADC disable.
        uint8_t IBAT_ADC_DIS : 1; // Bit 6: IBAT ADC disable.
        uint8_t IBUS_ADC_DIS : 1; // Bit 7: IBUS ADC disable.
    } BITS;
} BQ25792_REG2F_ADC_Function_Disable_0_t;

/**
 * @brief REG30: ADC Function Disable 1 Register (Offset 0x30)
 *        Disables specific ADC functions.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE;
    struct
    {
        uint8_t RESERVED_3_0 : 4; // Bits 3-0: Reserved.
        uint8_t VAC1_ADC_DIS : 1; // Bit 4: VAC1 ADC disable.
        uint8_t VAC2_ADC_DIS : 1; // Bit 5: VAC2 ADC disable.
        uint8_t DM_ADC_DIS : 1;   // Bit 6: D- ADC disable.
        uint8_t DP_ADC_DIS : 1;   // Bit 7: D+ ADC disable.
    } BITS;
} BQ25792_REG30_ADC_Function_Disable_1_t;

/**
 * @brief REG31: IBUS ADC Register (Offset 0x31)
 *        IBUS ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t IBUS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of IBUS ADC reading.
        uint8_t IBUS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of IBUS ADC reading.
    } BITS;
} BQ25792_REG31_IBUS_ADC_t;

/**
 * @brief REG33: IBAT ADC Register (Offset 0x33)
 *        IBAT ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t IBAT_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of IBAT ADC reading.
        uint8_t IBAT_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of IBAT ADC reading.
    } BITS;
} BQ25792_REG33_IBAT_ADC_t;

/**
 * @brief REG35: VBUS ADC Register (Offset 0x35)
 *        VBUS ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t VBUS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of VBUS ADC reading.
        uint8_t VBUS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of VBUS ADC reading.
    } BITS;
} BQ25792_REG35_VBUS_ADC_t;

/**
 * @brief REG37: VAC1 ADC Register (Offset 0x37)
 *        VAC1 ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t VAC1_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of VAC1 ADC reading.
        uint8_t VAC1_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of VAC1 ADC reading.
    } BITS;
} BQ25792_REG37_VAC1_ADC_t;

/**
 * @brief REG39: VAC2 ADC Register (Offset 0x39)
 *        VAC2 ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t VAC2_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of VAC2 ADC reading.
        uint8_t VAC2_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of VAC2 ADC reading.
    } BITS;
} BQ25792_REG39_VAC2_ADC_t;

/**
 * @brief REG3B: VBAT ADC Register (Offset 0x3B)
 *        VBAT ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t VBAT_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of VBAT ADC reading.
        uint8_t VBAT_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of VBAT ADC reading.
    } BITS;
} BQ25792_REG3B_VBAT_ADC_t;

/**
 * @brief REG3D: VSYS ADC Register (Offset 0x3D)
 *        VSYS ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t VSYS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of VSYS ADC reading.
        uint8_t VSYS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of VSYS ADC reading.
    } BITS;
} BQ25792_REG3D_VSYS_ADC_t;

/**
 * @brief REG3F: TS ADC Register (Offset 0x3F)
 *        TS ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t TS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of TS ADC reading.
        uint8_t TS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of TS ADC reading.
    } BITS;
} BQ25792_REG3F_TS_ADC_t;

/**
 * @brief REG41: TDIE ADC Register (Offset 0x41)
 *        TDIE ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t TDIE_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of TDIE ADC reading.
        uint8_t TDIE_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of TDIE ADC reading.
    } BITS;
} BQ25792_REG41_TDIE_ADC_t;

/**
 * @brief REG43: D+ ADC Register (Offset 0x43)
 *        D+ ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t DPLUS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of D+ ADC reading.
        uint8_t DPLUS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of D+ ADC reading.
    } BITS;
} BQ25792_REG43_DPLUS_ADC_t;

/**
 * @brief REG45: D- ADC Register (Offset 0x45)
 *        D- ADC reading.
 *        Defined as 16-bit.
 */
typedef union
{
    volatile uint8_t BYTE[2]; // [0] = LSB, [1] = MSB
    uint16_t WORD;            // Direct 16-bit access
    struct
    {
        uint8_t DMINUS_ADC_7_0 : 8;  // Bits 7-0 (LSB): Lower 8 bits of D- ADC reading.
        uint8_t DMINUS_ADC_15_8 : 8; // Bits 15-8 (MSB): Upper 8 bits of D- ADC reading.
    } BITS;
} BQ25792_REG45_DMINUS_ADC_t;

/**
 * @brief REG47: DPDM Driver Register (Offset 0x47)
 *        Controls D+/D- output drivers.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t RESERVED_1_0 : 2;   // Bits 1-0: Reserved.
        uint8_t DMINUS_DAC_2_0 : 3; // Bits 4-2: D- Output Driver setting.
        uint8_t DPLUS_DAC_2_0 : 3;  // Bits 7-5: D+ Output Driver setting.
    } BITS;
} BQ25792_REG47_DPDM_Driver_t;

/**
 * @brief REG48: Part Information Register (Offset 0x48)
 *        Contains part number and revision information.
 *        Strictly 8-bit.
 */
typedef union
{
    volatile uint8_t BYTE; // Single 8-bit register
    struct
    {
        uint8_t DEV_REV : 3;      // Bits 2-0: Device Revision.
        uint8_t PN : 3;           // Bits 5-3: Device Part number.
        uint8_t RESERVED_7_6 : 2; // Bits 7-6: Reserved.
    } BITS;
} BQ25792_REG48_Part_Information_t;

// =========================================================================
// Полная карта регистров BQ25792 (основные управляющие и статус регистры)
// =========================================================================

/**
 * @brief Полная карта регистров BQ25792
 *        Регистры, которые могут быть представлены как 16-битные, имеют
 *        члены BYTE[2] и WORD. Регистры, которые помещаются в 8 бит, остаются 8-битными.
 */

#pragma pack(push, 1)
typedef union
{
    volatile uint8_t BYTE[0x49];
    struct
    {
        BQ25792_REG00_Minimal_System_Voltage_t REG00_MinimalSystemVoltage; // 00h
        BQ25792_REG01_Charge_Voltage_Limit_t REG01_ChargeVoltageLimit;     // 01h (16-bit)
        BQ25792_REG03_Charge_Current_Limit_t REG03_ChargeCurrentLimit;     // 03h (16-bit)
        BQ25792_REG05_Input_Voltage_Limit_t REG05_InputVoltageLimit;       // 05h
        BQ25792_REG06_Input_Current_Limit_t REG06_InputCurrentLimit;       // 06h (16-bit)
        BQ25792_REG08_Precharge_Control_t REG08_PrechargeControl;          // 08h
        BQ25792_REG09_Termination_Control_t REG09_TerminationControl;      // 09h
        BQ25792_REG0A_Recharge_Control_t REG0A_RechargeControl;            // 0Ah
        BQ25792_REG0B_VOTG_regulation_t REG0B_VOTG_regulation;             // 0Bh (16-bit)
        BQ25792_REG0D_IOTG_regulation_t REG0D_IOTG_regulation;             // 0Dh
        BQ25792_REG0E_Timer_Control_t REG0E_TimerControl;                  // 0Eh
        BQ25792_REG0F_Charger_Control_0_t REG0F_ChargerControl0;           // 0Fh
        BQ25792_REG10_Charger_Control_1_t REG10_ChargerControl1;           // 10h
        BQ25792_REG11_Charger_Control_2_t REG11_ChargerControl2;           // 11h
        BQ25792_REG12_Charger_Control_3_t REG12_ChargerControl3;           // 12h
        BQ25792_REG13_Charger_Control_4_t REG13_ChargerControl4;           // 13h
        BQ25792_REG14_Charger_Control_5_t REG14_ChargerControl5;           // 14h
        BQ25792_REG15_Reserved_t REG15_Reserved;                           // 15h
        BQ25792_REG16_Temperature_Control_t REG16_TemperatureControl;      // 16h
        BQ25792_REG17_NTC_Control_0_t REG17_NTC_Control0;                  // 17h
        BQ25792_REG18_NTC_Control_1_t REG18_NTC_Control1;                  // 18h
        BQ25792_REG19_ICO_Current_Limit_t REG19_ICO_CurrentLimit;          // 19h (16-bit)
        BQ25792_REG1B_Charger_Status_0_t REG1B_ChargerStatus0;             // 1Bh
        BQ25792_REG1C_Charger_Status_1_t REG1C_ChargerStatus1;             // 1Ch
        BQ25792_REG1D_Charger_Status_2_t REG1D_ChargerStatus2;             // 1Dh
        BQ25792_REG1E_Charger_Status_3_t REG1E_ChargerStatus3;             // 1Eh
        BQ25792_REG1F_Charger_Status_4_t REG1F_ChargerStatus4;             // 1Fh
        BQ25792_REG20_FAULT_Status_0_t REG20_FaultStatus0;                 // 20h
        BQ25792_REG21_FAULT_Status_1_t REG21_FaultStatus1;                 // 21h
        BQ25792_REG22_Charger_Flag_0_t REG22_ChargerFlag0;                 // 22h
        BQ25792_REG23_Charger_Flag_1_t REG23_ChargerFlag1;                 // 23h
        BQ25792_REG24_Charger_Flag_2_t REG24_ChargerFlag2;                 // 24h
        BQ25792_REG25_Charger_Flag_3_t REG25_ChargerFlag3;                 // 25h
        BQ25792_REG26_FAULT_Flag_0_t REG26_FaultFlag0;                     // 26h
        BQ25792_REG27_FAULT_Flag_1_t REG27_FaultFlag1;                     // 27h
        BQ25792_REG28_Charger_Mask_0_t REG28_ChargerMask0;                 // 28h
        BQ25792_REG29_Charger_Mask_1_t REG29_ChargerMask1;                 // 29h
        BQ25792_REG2A_Charger_Mask_2_t REG2A_ChargerMask2;                 // 2Ah
        BQ25792_REG2B_Charger_Mask_3_t REG2B_ChargerMask3;                 // 2Bh
        BQ25792_REG2C_FAULT_Mask_0_t REG2C_FaultMask0;                     // 2Ch
        BQ25792_REG2D_FAULT_Mask_1_t REG2D_FaultMask1;                     // 2Dh
        BQ25792_REG2E_ADC_Control_t REG2E_ADCControl;                      // 2Eh
        BQ25792_REG2F_ADC_Function_Disable_0_t REG2F_ADCFuncDisable0;      // 2Fh
        BQ25792_REG30_ADC_Function_Disable_1_t REG30_ADCFuncDisable1;      // 30h
        BQ25792_REG31_IBUS_ADC_t REG31_IBUS_ADC;                           // 31h (16-bit)
        BQ25792_REG33_IBAT_ADC_t REG33_IBAT_ADC;                           // 33h (16-bit)
        BQ25792_REG35_VBUS_ADC_t REG35_VBUS_ADC;                           // 35h (16-bit)
        BQ25792_REG37_VAC1_ADC_t REG37_VAC1_ADC;                           // 37h (16-bit)
        BQ25792_REG39_VAC2_ADC_t REG39_VAC2_ADC;                           // 39h (16-bit)
        BQ25792_REG3B_VBAT_ADC_t REG3B_VBAT_ADC;                           // 3Bh (16-bit)
        BQ25792_REG3D_VSYS_ADC_t REG3D_VSYS_ADC;                           // 3Dh (16-bit)
        BQ25792_REG3F_TS_ADC_t REG3F_TS_ADC;                               // 3Fh (16-bit)
        BQ25792_REG41_TDIE_ADC_t REG41_TDIE_ADC;                           // 41h (16-bit)
        BQ25792_REG43_DPLUS_ADC_t REG43_DPLUS_ADC;                         // 43h (16-bit)
        BQ25792_REG45_DMINUS_ADC_t REG45_DMINUS_ADC;                       // 45h (16-bit)
        BQ25792_REG47_DPDM_Driver_t REG47_DPDM_Driver;                     // 47h
        BQ25792_REG48_Part_Information_t REG48_Part_Information;           // 48h
    } reg;
} BQ25792_RegisterMap_t;
#pragma pack(pop)

/* =========================================================
 * Driver handle — user fills in I2C callbacks
 * ========================================================= */
typedef struct
{
    /**
     * @brief Write bytes to device over I2C.
     * @param addr  7-bit I2C address (0x6B)
     * @param reg   Register address
     * @param data  Pointer to data buffer
     * @param len   Number of bytes
     * @return 0 on success, non-zero on error
     */
    int (*i2c_write)(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len);

    /**
     * @brief Read bytes from device over I2C.
     * @param addr  7-bit I2C address (0x6B)
     * @param reg   Register address
     * @param data  Pointer to receive buffer
     * @param len   Number of bytes to read
     * @return 0 on success, non-zero on error
     */
    int (*i2c_read)(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);
} bq25792_t;

/* =========================================================
 * Low-level register access
 * ========================================================= */

/**
 * @brief Write one byte to a register.
 * @param dev  Driver handle
 * @param reg  Register address
 * @param val  Value to write
 * @return 0 on success
 */
int bq25792_write_reg(const bq25792_t *dev, uint8_t reg, uint8_t val);

/**
 * @brief Read one byte from a register.
 * @param dev  Driver handle
 * @param reg  Register address
 * @param val  Pointer to store result
 * @return 0 on success
 */
int bq25792_read_reg(const bq25792_t *dev, uint8_t reg, uint8_t *val);

/**
 * @brief Write two bytes (16-bit register, MSB first).
 * @param dev  Driver handle
 * @param reg  Register address (MSB register)
 * @param val  16-bit value
 * @return 0 on success
 */
int bq25792_write_reg16(const bq25792_t *dev, uint8_t reg, uint16_t val);

/**
 * @brief Read two bytes (16-bit register, MSB first).
 * @param dev  Driver handle
 * @param reg  Register address (MSB register)
 * @param val  Pointer to store 16-bit result
 * @return 0 on success
 */
int bq25792_read_reg16(const bq25792_t *dev, uint8_t reg, uint16_t *val);

/*
 * Пример использования:
 *
 * // Предполагая, что BASE_ADDRESS_BQ25792 - это определенный базовый адрес
 * // в памяти, где отображены регистры BQ25792.
 * extern volatile BQ25792_RegisterMap_t* const bq25792_regs;
 * // или:
 * // BQ25792_RegisterMap_t* const bq25792_regs = (BQ25792_RegisterMap_t*)BASE_ADDRESS_BQ25792;
 *
 * // Пример доступа к полю регистра:
 * // Получить значение Charge Voltage Limit (16 бит)
 * // uint16_t charge_voltage_limit = bq25792_regs->REG01_ChargeVoltageLimit.WORD;
 *
 * // Установить значение IBUS ADC (16 бит)
 * // uint16_t new_ibus_value = 1234; // Пример значения
 * // bq25792_regs->REG31_IBUS_ADC.WORD = new_ibus_value;
 *
 * // Прочитать значение всей структуры BITS для REG0B (VOTG)
 * // uint8_t lsb_part = bq25792_regs->REG0B_VOTG_regulation.BITS.VOTG_LSB;
 * // uint8_t msb_part = bq25792_regs->REG0B_VOTG_regulation.BITS.VOTG_MSB_HIGH;

  * // Пример доступа к полю регистра:
 * // Установить VAC_OVP threshold на 12V (REG10, VAC_OVP_1_0 = 2h)
 * // bq25792_regs->REG10_ChargerControl1.BITS.VAC_OVP_1_0 = BQ25792_REG10_VAC_OVP_12V;
 *
 * // Получить значение IBAT_REG_1_0 (Battery discharging current regulation in OTG mode)
 * // uint8_t discharge_current_setting = bq25792_regs->REG14_ChargerControl5.BITS.IBAT_REG_1_0;
 * // if (discharge_current_setting == BQ25792_REG14_IBAT_REG_5A) {
 * //     // Установлен режим 5A
 * // }
 *
 * // Установить D+ Output Driver на 2.0V (REG47, DPLUS_DAC_2_0 = 4h)
 * // bq25792_regs->REG47_DPDM_Driver.BITS.DPLUS_DAC_2_0 = BQ25792_REG47_DPLUS_DAC_2_0V;
 */