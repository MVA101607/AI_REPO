#include "ds18b20.h"
#include "at_commands.h"
#include "onewire_ll.h"
#include <stdio.h>
#include "string.h"

// DS18B20 ROM-command 0xCC — SKIP ROM (одиночное устройство на шине).
#define DS_CMD_SKIP_ROM 0xCC
#define DS_CMD_CONVERT_T 0x44
#define DS_CMD_READ_SCRATCH 0xBE

// Таймаут конверсии для 12-бит разрешения.
#define DS_CONVERT_MS 750

static float _temp = DS_INVALID_TEMP;
static bool _valid = false;
static bool _conversion_pending = false;
static uint32_t _last_ms = 0;

/* Диагностика: показывает, почему _valid == false. Сбрасывается в ds_init(). */
static uint32_t _diag_start_count = 0;
static uint32_t _diag_reset_count = 0;    /* всего попыток reset */
static uint32_t _diag_presence_count = 0; /* успешных presence */
static uint32_t _diag_crc_ok_count = 0;   /* CRC ок */
static uint32_t _diag_crc_fail_count = 0; /* CRC плохой */
static uint32_t _diag_read_count = 0;     /* удачных scratchpad чтений */
static int16_t _diag_last_raw = 0;        /* последний raw (для отладки) */
static uint8_t _diag_last_cfg_reg = 0;    /* последний CONFIGURATION byte */

// Прочитать scratchpad (9 байт); возвращает true если CRC ОК.
static bool ds_read_scratchpad(uint8_t buf[9])
{
    _diag_reset_count++;
    if (!onewire_reset())
        return false;
    _diag_presence_count++;
    onewire_write_byte(DS_CMD_SKIP_ROM);
    onewire_write_byte(DS_CMD_READ_SCRATCH);
    for (uint8_t i = 0; i < 9; i++)
    {
        buf[i] = onewire_read_byte();
    }

    // CRC8 Dallas — стандартный полином 0x8C.
    uint8_t crc = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t b = buf[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            uint8_t mix = (crc ^ b) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C;
            b >>= 1;
        }
    }
    if (crc != buf[8])
    {
        _diag_crc_fail_count++;
        return false;
    }
    _diag_crc_ok_count++;
    _diag_read_count++;
    _diag_last_raw = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
    _diag_last_cfg_reg = buf[7];
    return true;
}

static bool ds_start_conversion(void)
{
    _diag_start_count++;
    _diag_reset_count++;
    if (!onewire_reset())
        return false;
    _diag_presence_count++;
    onewire_write_byte(DS_CMD_SKIP_ROM);
    onewire_write_byte(DS_CMD_CONVERT_T);
    return true;
}

void ds_init(void)
{
    /* Самый первый инкремент — отметка что выполнение до сюда дошло */
    _diag_start_count += 1000;

    onewire_init();

    _valid = false;
    _temp = DS_INVALID_TEMP;
    _conversion_pending = false;
    _diag_reset_count = 0;
    _diag_presence_count = 0;
    _diag_crc_ok_count = 0;
    _diag_crc_fail_count = 0;
    _diag_read_count = 0;
    _diag_start_count = 0;
    _diag_last_raw = 0;
    _diag_last_cfg_reg = 0;

    // 12-bit разрешение по умолчанию после power-on
    if (ds_start_conversion())
    {
        _conversion_pending = true;
        _last_ms = 0; // засечём after first update
    }
}

void ds_update(void)
{
    // g_tick_ms предоставляется через main.c (Systick handler).
    extern volatile uint32_t g_tick_ms;
    uint32_t now = (uint32_t)g_tick_ms;
    if (_last_ms == 0)
    {
        _last_ms = now;
        return;
    }

    if (_conversion_pending && (now - _last_ms >= DS_CONVERT_MS))
    {
        uint8_t buf[9];
        if (ds_read_scratchpad(buf))
        {
            /* CRC OK. Если configuration register (buf[7]) == 0,
             * значит DS18B20 в 9-bit mode или scratchpad не проинициализирован. */
            if (buf[7] != 0)
            {
                int16_t raw = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
                _temp = (float)raw * 0.0625f;
                _valid = true;
            }
            else
            {
                _valid = false;
                _temp = DS_INVALID_TEMP;
            }
        }
        else
        {
            _valid = false;
            _temp = DS_INVALID_TEMP;
        }
        _conversion_pending = false;
    }

    if (!_conversion_pending && (now - _last_ms >= DS_READ_INTERVAL_MS))
    {
        _last_ms = now;
        if (ds_start_conversion())
        {
            _conversion_pending = true;
        }
    }
}

float ds_get_temp(void) { return _temp; }
bool ds_is_valid(void) { return _valid; }

bool ds_at_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd;
    char buf[64];

    if (params == NULL)
    {
        at_send(AT_ERROR);
        return true;
    }

    /* Диагностическая команда: AT+TEMP=STATUS */
    if (strncmp(params, "STATUS", 6) == 0)
    {
        snprintf(buf, sizeof(buf),
                 "AT+TEMP=STATUS,S=%lu,R=%lu,P=%lu,OK=%lu,FAIL=%lu,RD=%lu,V=%d,RAW=%d,CFG=%u",
                 (unsigned long)_diag_start_count,
                 (unsigned long)_diag_reset_count,
                 (unsigned long)_diag_presence_count,
                 (unsigned long)_diag_crc_ok_count,
                 (unsigned long)_diag_crc_fail_count,
                 (unsigned long)_diag_read_count,
                 _valid ? 1 : 0,
                 (int)_diag_last_raw,
                 (unsigned)_diag_last_cfg_reg);
        at_send(buf);
        return true;
    }

    /* Обычный запрос: AT+TEMP?... или AT+TEMP (no params = ?) */
    if (strcmp(params, "?") != 0 && params[0] != '\0')
    {
        at_send(AT_ERROR);
        return true;
    }

    if (_valid)
    {
        int whole = (int)_temp;
        int frac = (int)((_temp - (float)whole) * 10.0f);
        if (frac < 0)
            frac = -frac;
        snprintf(buf, sizeof(buf), "AT+TEMP=%d.%d", whole, frac);
    }
    else
    {
        at_send(AT_ERROR);
        return true;
    }
    at_send(buf);
    return true;
}