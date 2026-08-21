// onewire_ll.c — bit-bang 1-Wire на основе LL GPIO.
// Реализация совместима с Arduino-OneWire по таймингам, но
// ВАЖНО: __NOP()-busy-loop заменён на DWT->CYCCNT, иначе компилятор
// при -Og выкидывает задержки и DS18B20 теряет presence.
#include "onewire_ll.h"
#include "pins_ll.h"
#include "main.h"

// Точные значения ниже — для кристалла STM32F103 с тактированием 64 МГц.
// Используется пустой busy-loop на __NOP(). Каждая единица цикла ~15.6 нс.

static inline void delay_us(uint32_t us)
{
    /* Точные задержки через DWT cycle counter — при -Og компилятор
     * может выкинуть busy-loop на __NOP(), что добавляет 100% таймингов
     * у OneWire и DS18B20 теряет presence. */
    extern uint32_t SystemCoreClock;
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = (uint32_t)((uint64_t)us * SystemCoreClock / 1000000u);
    while ((uint32_t)(DWT->CYCCNT - start) < ticks)
    {
        __NOP();
    }
}

static inline void pin_low(void)
{
    /* Push-pull OUTPUT LOW. ОДИН переход: режим OUTPUT, pin=0.
     * Предыдущая версия делала странный SetHigh->ResetLow, что создаёт
     * ложный rising edge во время reset-slot и DS18B20 теряет presence. */
    LL_GPIO_SetPinMode(PIN_ONEW1_PORT, PIN_ONEW1_PI, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(PIN_ONEW1_PORT, PIN_ONEW1_PI, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_ResetOutputPin(PIN_ONEW1_PORT, PIN_ONEW1_PI);
}

static inline void pin_release(void)
{
    // INPUT + PULLUP — линия отпускается в HIGH (внешний/внутренний pullup)
    LL_GPIO_SetPinMode(PIN_ONEW1_PORT, PIN_ONEW1_PI, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(PIN_ONEW1_PORT, PIN_ONEW1_PI, LL_GPIO_PULL_UP);
}

static inline bool pin_read(void)
{
    return (LL_GPIO_IsInputPinSet(PIN_ONEW1_PORT, PIN_ONEW1_PI) != 0);
}

void onewire_init(void)
{
    /* Включаем DWT cycle counter — иначе delay_us() выдаёт 0 циклов. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    pin_release(); // стартовое состояние — линия отпущена
}

bool onewire_reset(void)
{
    bool presence = false;

    pin_low();
    delay_us(500); // >= 480 µs reset pulse
    pin_release();
    delay_us(70); // ждём presence detect

    // Если подтяжка есть и устройство на шине — линию притянет в LOW на время presence.
    if (!pin_read())
    {
        presence = true;
    }
    delay_us(420); // дожидаемся конца presence (>= 480 µs всего)
    return presence;
}

void onewire_write_bit(bool bit)
{
    if (bit)
    {
        pin_low();
        delay_us(6);
        pin_release();
        delay_us(64);
    }
    else
    {
        pin_low();
        delay_us(60);
        pin_release();
        delay_us(10);
    }
}

bool onewire_read_bit(void)
{
    bool bit;
    pin_low();
    delay_us(2);
    pin_release();
    delay_us(10); // sample window 10-15 µs от фронта rising
    bit = pin_read();
    delay_us(55); // slot time >= 60 µs
    return bit;
}

void onewire_write_byte(uint8_t b)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        onewire_write_bit((b & 0x01) != 0);
        b >>= 1;
    }
}

uint8_t onewire_read_byte(void)
{
    uint8_t b = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (onewire_read_bit())
        {
            b |= (1u << i);
        }
    }
    return b;
}