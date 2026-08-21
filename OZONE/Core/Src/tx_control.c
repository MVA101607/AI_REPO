#include "tx_control.h"
#include "at_commands.h"
#include "pins_ll.h"
#include "main.h"
#include <string.h>

static bool _tx_enabled  = false;
static bool _power_locked = false;

void tx_control_init(void)
{
    _tx_enabled = false;
    ResetPin(EN_VPA);  // amplifier OFF (LOW)
    SetPin(BST_Dis);   // boost OFF (HIGH) при старте — нет внешнего питания
}

bool tx_enable(void)
{
    if (_power_locked)
        return false;
    ResetPin(BST_Dis); // boost ON
    SetPin(EN_VPA);    // amplifier ON
    _tx_enabled = true;
    return true;
}

bool tx_disable(void)
{
    ResetPin(EN_VPA);  // amplifier OFF
    SetPin(BST_Dis);   // boost OFF
    _tx_enabled = false;
    return true;
}

bool tx_is_enabled(void)
{
    return _tx_enabled;
}

bool tx_at_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd;
    if (params == NULL) {
        at_send(AT_ERROR);
        return true;
    }

    if (strcmp(params, "?") == 0) {
        at_send(tx_is_enabled() ? "AT+TX=1" : "AT+TX=0");
        return true;
    }
    if (strcmp(params, "1") == 0) {
        if (!tx_enable()) {
            at_send(AT_ERROR);
            return true;
        }
        at_send(AT_OK);
        return true;
    }
    if (strcmp(params, "0") == 0) {
        tx_disable();
        at_send(AT_OK);
        return true;
    }

    at_send(AT_ERROR);
    return true;
}

void tx_set_power_lockout(bool locked)
{
    _power_locked = locked;
}

bool tx_is_locked(void)
{
    return _power_locked;
}