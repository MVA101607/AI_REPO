// serial_at_ll.c — HAL для USART2 с ring buffer (RX) и polling TX.
#include "serial_at_ll.h"
#include "main.h"
#include "config.h"

#define AT_RX_BUF_SIZE 128

static volatile uint8_t _rx_buf[AT_RX_BUF_SIZE];
static volatile uint16_t _rx_head = 0; // producer (ISR)
static volatile uint16_t _rx_tail = 0; // consumer (main)

static void _tx_byte(char c)
{
    uint32_t timeout = 100000UL;
    LL_USART_TransmitData8(USART2, (uint8_t)c);
    while (!LL_USART_IsActiveFlag_TXE(USART2) && (timeout > 0))
    {
        timeout--;
    }
}

void serial_at_init(void)
{
    _rx_head = 0;
    _rx_tail = 0;
    // USART2 уже сконфигурирован в MX_USART2_UART_Init() (115200 8N1).
}

void serial_at_disable(void)
{
    // Запрет IRQ приёмника (на случай, если в момент disable уже шёл байт).
    LL_USART_DisableIT_RXNE(USART2);

    // Снять NVIC, чтобы ISR не дёргался после отключения периферии.
    NVIC_DisableIRQ(USART2_IRQn);

    // Сам периферийный блок.
    LL_USART_Disable(USART2);
    LL_USART_DeInit(USART2);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);

    // PA2/PA3 → FLOATING/HI-Z. Без этого TX-пин подтягивал бы линию
    // и мешал корректному обесточиванию одноплатника, который получает
    // питание через эти же линии.
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    gpio.Mode = LL_GPIO_MODE_FLOATING;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &gpio);
}

void serial_at_reinit(void)
{
    // Полная переинициализация периферии (CubeMX-style).
    MX_USART2_UART_Init();

    // Включить прерывание по приёму и NVIC заново — см. main().
    LL_USART_EnableIT_RXNE(USART2);
    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    // Сбросить приёмный ring-buffer, чтобы старые байты не подвесили парсер.
    _rx_head = 0;
    _rx_tail = 0;
}

void serial_at_push_byte(char c)
{
    uint16_t next = (uint16_t)((_rx_head + 1u) % AT_RX_BUF_SIZE);
    if (next != _rx_tail)
    {
        _rx_buf[_rx_head] = (uint8_t)c;
        _rx_head = next;
    }
    // else: переполнение, байт теряется (AT пересинхронизируется по SYNC)
}

int serial_at_available(void)
{
    return (_rx_head != _rx_tail) ? 1 : 0;
}

char serial_at_read(void)
{
    char c = 0;
    if (_rx_head != _rx_tail)
    {
        c = (char)_rx_buf[_rx_tail];
        _rx_tail = (uint16_t)((_rx_tail + 1u) % AT_RX_BUF_SIZE);
    }
    return c;
}

void serial_at_write(char c)
{
    _tx_byte(c);
}

void serial_at_print(const char *s)
{
    if (s == NULL)
        return;
    while (*s != '\0')
    {
        _tx_byte(*s++);
    }
}