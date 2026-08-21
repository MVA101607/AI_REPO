// boot.c — главный конечный автомат

#include "boot.h"
#include "at_commands.h"
#include "bq_compat.h"
#include "config.h"
#include "ds18b20.h"
#include "pins_ll.h"
#include "platform.h"
#include "serial_at_ll.h"
#include "tx_control.h"
#include "main.h"
#include <stdint.h>

static BootState _state = INIT;
static uint32_t _timer = 0;
static uint32_t _bq_timer = 0;
static BatInfo _bat_info;
static bool _bq_ok = false;
static bool _ext_power_present = false;

// Power button state
static bool _pwr_btn_last_state = false; // false = not pressed (HIGH via pullup)
static uint32_t _pwr_btn_debounce_timer = 0;
static bool _pwr_btn_stable_state = false; // false = not pressed
static uint32_t _pwr_btn_hold_timer = 0;

// =========================================================
// pins_init — в оригинале все пины уже сконфигурированы в MX_GPIO_Init().
// Здесь — только начальные состояния (по требованию Arduino boot flow).
// =========================================================
static void pins_init(void)
{
    // Boost enable: HIGH (disabled) — не стартует сразу.
    SetPin(BST_Dis);
    // Все остальные питания/выходы — выключены (LOW).
    ResetPin(EN_5Vpi); // OPI
    ResetPin(EN_8Vsd); // SDR
    ResetPin(EN_VPA);  // PA
    ResetPin(EN_HEAT); // Heater
    ResetPin(UDE);     // RS-485
    // PI RST/BOOT/RECOVERY — LOW.
    ResetPin(PI_RST);
    ResetPin(PI_Boot);
    ResetPin(PI_RCRY);
    // EN_FAN и PAen — INPUT (CubeMX-INIT-Pin уже в INPUT/OFF).
}

// =========================================================
// bq_tick — опрос чарджера по таймеру + watchdog reset.
// При ошибках watchdog — выполнить I2C-bus recovery и переинициализацию.
// =========================================================
static void bq_tick(bool force)
{
    if (!_bq_ok)
        return;

    bool need_tick = force || ((uint32_t)(g_tick_ms - _bq_timer) >= BQ_POLL_INTERVAL_MS);
    if (!need_tick)
        return;

    _bq_timer = (uint32_t)g_tick_ms;

    int err = bq_update(&_bat_info);
    if (err != 0)
    {
        _bq_ok = false;
        return;
    }

    // FSM чарджера: только переходы между состояниями, без GPIO.
    bq_fsm_run(&_bat_info);

    // Сброс WDT; ошибка → recovery + reinit.
    err = bq_reset_watchdog();
    if (err != 0)
    {
        bq_recover_i2c();
        _bq_ok = (bq_init() == 0);
        if (_bq_ok)
        {
            delay_ms(200);
        }
    }
}

// =========================================================
// pwr_btn_tick — polling с дебаунсом и hold-таймером.
// EN_FAN: INPUT_PULLUP, active LOW (кнопка нажата = LOW).
// =========================================================
static bool pwr_btn_tick(void)
{
    bool raw = (LL_GPIO_IsInputPinSet(PIN_EN_FAN_PORT, PIN_EN_FAN_PI) == 0);

    // Дебаунс: смена состояния сбрасывает таймер.
    if (raw != _pwr_btn_last_state)
    {
        _pwr_btn_last_state = raw;
        _pwr_btn_debounce_timer = (uint32_t)g_tick_ms;
    }

    if (((uint32_t)g_tick_ms - _pwr_btn_debounce_timer) < PWR_BTN_DEBOUNCE_MS)
    {
        return false;
    }

    if (raw != _pwr_btn_stable_state)
    {
        _pwr_btn_stable_state = raw;
        if (_pwr_btn_stable_state)
        {
            _pwr_btn_hold_timer = (uint32_t)g_tick_ms;
        }
        else
        {
            _pwr_btn_hold_timer = 0;
        }
    }

    if (_pwr_btn_stable_state &&
        _pwr_btn_hold_timer != 0 &&
        ((uint32_t)g_tick_ms - _pwr_btn_hold_timer) >= PWR_BTN_HOLD_MS)
    {
        _pwr_btn_hold_timer = 0; // однократное срабатывание
        return true;
    }
    return false;
}

// =========================================================
// power_tick — мониторинг наличия внешнего питания + TX-lockout.
// =========================================================
static void power_tick(void)
{
    if (_bat_info.vbus_mv < VBUS_MIN_THRESHOLD_MV && _ext_power_present)
    {
        _ext_power_present = false;
        tx_set_power_lockout(true);
        if (tx_is_enabled())
        {
            tx_disable();
            at_send("AT+TX=0");
        }
    }
    else if (_bat_info.vbus_mv >= VBUS_MIN_THRESHOLD_MV && !_ext_power_present)
    {
        _ext_power_present = true;
        tx_set_power_lockout(false);
    }
}

// =========================================================
// Public API
// =========================================================
void boot_init(void)
{
    pins_init();
    tx_control_init();

    // USART2 уже сконфигурирован (115200 8N1) в MX_USART2_UART_Init().
    at_init();
    at_send(AT_STM_READY);
    at_register_handler(AT_CMD, at_cmd_handler);
    at_register_handler(AT_CMD_SYNC, at_error_handler);
    at_register_handler(AT_ERROR, at_error_handler);
    at_register_handler(AT_PING, at_ping_handler);
    at_register_handler(AT_CMD_TX, tx_at_handler);
    at_register_handler(AT_CMD_CHRG, bq_at_handler);
    at_register_handler(AT_CMD_TEMP, ds_at_handler);

    // I2C1 уже сконфигурирован (100 кГц Fast-mode в MX_I2C1_Init()).
    _bq_ok = (bq_init() == 0);
    delay_ms(100);

    _pwr_btn_debounce_timer = (uint32_t)g_tick_ms;
    _pwr_btn_last_state = (LL_GPIO_IsInputPinSet(PIN_EN_FAN_PORT, PIN_EN_FAN_PI) == 0);
    _pwr_btn_stable_state = _pwr_btn_last_state;

    _state = INIT;
}

void boot_update(void)
{
    switch (_state)
    {
    case INIT:
        _timer = (uint32_t)g_tick_ms;
        _state = OPI_STARTING;
        at_send("INIT -> OPI_STARTING");
        break;

    case OPI_STARTING:
        SetPin(EN_5Vpi);
        _timer = (uint32_t)g_tick_ms;
        _state = SDR_STARTING;
        break;

    case SDR_STARTING:
        SetPin(EN_8Vsd);
        _state = THERM_INIT;
        break;

    case THERM_INIT:
        ds_init();
        ds_update();
        _state = WAIT_OPI_BOOT;
        break;

    case WAIT_OPI_BOOT:
        at_process();
        ds_update();
        bq_tick(false);

        if ((uint32_t)(g_tick_ms - _timer) >= OPI_BOOT_DELAY_MS)
        {
            _timer = (uint32_t)g_tick_ms;
            _state = WAIT_APP_READY;
            at_send("WAIT_OPI_BOOT -> WAIT_APP_READY");
        }

        if (pwr_btn_tick())
        {
            _state = SHUTDOWN;
            at_send("WAIT_OPI_BOOT -> SHUTDOWN");
        }
        break;

    case WAIT_APP_READY:
        at_process();
        ds_update();
        bq_tick(false);
        power_tick();

        if ((uint32_t)(g_tick_ms - _timer) >= OPI_BOOT_FAIL_MS)
        {
            ResetPin(EN_5Vpi);
            ResetPin(EN_8Vsd);
            serial_at_disable();

            delay_ms(OPI_BOOT_RESTART_MS);

            NVIC_SystemReset();
        }

        if (at_app_ready_received())
        {
            _state = RUNNING;
            at_send("WAIT_APP_READY -> RUNNING");
        }
        if (pwr_btn_tick())
        {
            _state = SHUTDOWN;
            at_send("WAIT_APP_READY -> SHUTDOWN");
        }
        break;

    case RUNNING:
        at_process();
        ds_update();
        bq_tick(false);
        power_tick();
        if (pwr_btn_tick())
        {
            tx_disable();
            at_send(AT_CMD_POWEROFF);
            _timer = (uint32_t)g_tick_ms;
            _state = POWEROFF_PENDING;
            at_send("RUNNING -> POWEROFF_PENDING");
        }
        break;

    case POWEROFF_PENDING:
        at_process();
        ds_update();
        bq_tick(false);
        if ((uint32_t)(g_tick_ms - _timer) >= OPI_POWEROFF_MS)
        {
            _state = SHUTDOWN;
            at_send("POWEROFF_PENDING -> SHUTDOWN");
        }
        break;

    case SHUTDOWN:
        tx_disable();
        ResetPin(EN_8Vsd);
        ResetPin(EN_5Vpi);

        serial_at_disable();
        delay_ms(OPI_POWEROFF_MS);
        serial_at_reinit();

        _state = SLEEP_PENDING;
        at_send("SHUTDOWN -> SLEEP_PENDING");
        break;

    case SLEEP_PENDING:
        delay_ms(1000);
        at_process();
        ds_update();
        bq_tick(false);

        bq_at_handler("AT+CHRG", "?");

        if (LL_GPIO_IsInputPinSet(PIN_EN_FAN_PORT, PIN_EN_FAN_PI) == 0)
        {
            delay_ms(100);
            NVIC_SystemReset();
        }
        if (!_bat_info.vbus_present)
        {
            _state = SLEEP;
            at_send("SLEEP_PENDING -> SLEEP");
        }
        break;

    case SLEEP:
        delay_ms(5000);
        at_process();
        ds_update();
        bq_tick(false);

        bq_at_handler("AT+CHRG", "?");
        at_send("going to sleep in 30s...");

        delay_ms(100);
        bq_ship_mode();
        delay_ms(BQ_SHIP_DELAY_MS);

        at_send("this should never happen");
        _state = SLEEP_PENDING;
        at_send("SLEEP -> SLEEP_PENDING");
        break;

    case UART_OPI_START:
        // Зарезервировано в исходнике (не используется).
        break;
    }
}

BootState boot_get_state(void) { return _state; }