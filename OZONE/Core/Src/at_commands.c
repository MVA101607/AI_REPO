#include "at_commands.h"
#include "serial_at_ll.h"
#include <stddef.h>
#include <string.h>

struct HandlerEntry
{
    const char  *base_cmd;
    at_handler_t handler;
};

static struct HandlerEntry _handlers[AT_MAX_HANDLERS];
static uint8_t  _handler_count = 0;
static bool     _initialized   = false;
static char     _line_buf[AT_LINE_BUF_SIZE];
static uint8_t  _line_pos      = 0;
static bool     _app_ready     = false;
static bool     _app_done      = false;
static bool     _synced        = false;

void at_init(void)
{
    serial_at_init();
    _initialized   = true;
    _app_ready     = false;
    _app_done      = false;
    _line_pos      = 0;
    _handler_count = 0;
    _synced        = false;
    (void)memset(_line_buf, 0, sizeof(_line_buf));
    (void)memset(_handlers,  0, sizeof(_handlers));
}

void at_register_handler(const char *base_cmd, at_handler_t handler)
{
    if (_handler_count < AT_MAX_HANDLERS && handler != NULL && base_cmd != NULL)
    {
        _handlers[_handler_count].base_cmd = base_cmd;
        _handlers[_handler_count].handler  = handler;
        _handler_count++;
    }
}

void at_send(const char *response)
{
    if (response != NULL) {
        serial_at_print(response);
        serial_at_print("\r\n");
    }
}

bool at_app_ready_received(void) { return _app_ready; }
bool at_app_done_received(void)  { return _app_done;  }

static void _parse_and_dispatch(char *line)
{
    /* Trim leading/trailing whitespace */
    while (*line == ' ' || *line == '\t')
        line++;
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t')) {
        line[--len] = '\0';
    }
    if (len == 0)
        return;

    /* Convert to uppercase in-place */
    for (size_t i = 0; i < len; i++) {
        if (line[i] >= 'a' && line[i] <= 'z') {
            line[i] -= (char)32;
        }
    }

    /* Find separator: '=' или '?', или space */
    char *params = NULL;
    char *eq = strchr(line, '=');
    char *qm = strchr(line, '?');
    char *sp = strchr(line, ' ');

    if (eq) {
        *eq = '\0';
        params = eq + 1;
    } else if (qm) {
        *qm = '\0';
        params = (char *)"?";
    } else if (sp) {
        *sp = '\0';
        params = sp + 1;
        while (*params == ' ')
            params++;
    }

    const char *base_cmd = line;

    /* Built-in lifecycle commands (no response) */
    if (strcmp(base_cmd, AT_CMD_READY) == 0) {
        _app_ready = true;
        return;
    }
    if (strcmp(base_cmd, AT_CMD_DONE) == 0) {
        _app_done = true;
        return;
    }

    /* Registered handlers */
    for (uint8_t i = 0; i < _handler_count; i++) {
        if (strcmp(base_cmd, _handlers[i].base_cmd) == 0) {
            if (_handlers[i].handler(base_cmd, params)) {
                return;
            }
        }
    }

    /* Unknown command */
    at_send(AT_ERROR);
}

void at_process(void)
{
    if (!_initialized) return;

    while (serial_at_available()) {
        char c = serial_at_read();

        if (!_synced) {
            static char  sync_buf[16];
            static uint8_t sync_pos = 0;

            if (c == '\r' || c == '\n') {
                if (sync_pos > 0) {
                    sync_buf[sync_pos] = '\0';
                    if (strstr(sync_buf, AT_CMD_SYNC) || strstr(sync_buf, AT_CMD_READY)) {
                        _synced   = true;
                        _line_pos = 0;
                        return;
                    }
                    sync_pos = 0;
                }
            } else if (sync_pos < sizeof(sync_buf) - 1) {
                sync_buf[sync_pos++] = c;
            }
            continue;
        }

        if (c == '\n' || c == '\r') {
            if (_line_pos > 0) {
                _line_buf[_line_pos] = '\0';
                _parse_and_dispatch(_line_buf);
                _line_pos = 0;
            }
        } else {
            if (_line_pos < AT_LINE_BUF_SIZE - 1) {
                _line_buf[_line_pos++] = c;
            }
        }
    }
}

bool at_cmd_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd; (void)params;
    at_send(AT_OK);
    return true;
}

bool at_error_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd; (void)params;
    return true;
}

bool at_ping_handler(const char *base_cmd, const char *params)
{
    (void)base_cmd; (void)params;
    at_send(AT_PONG);
    return true;
}