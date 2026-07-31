#include "saml10.h"

void HardFault_Handler_C(uint32_t *stacked);

__stackless void HardFault_Handler(void)
{
  __asm volatile(
    "TST LR, #4           \n" // проверить, PSP или MSP
    "ITE EQ               \n"
    "MRSEQ R0, MSP        \n"
    "MRSNE R0, PSP        \n"
    "B HardFault_Handler_C\n"
  );
}

void HardFault_Handler_C(uint32_t *stacked)
{
    volatile uint32_t r0  = stacked[0];
    volatile uint32_t r1  = stacked[1];
    volatile uint32_t r2  = stacked[2];
    volatile uint32_t r3  = stacked[3];
    volatile uint32_t r12 = stacked[4];
    volatile uint32_t lr  = stacked[5];  // return address
    volatile uint32_t pc  = stacked[6];  // PC на момент fault
    volatile uint32_t psr = stacked[7];

    volatile uint32_t icsr = SCB->ICSR;
    volatile uint32_t hfsr = SCB->HFSR;
    volatile uint32_t cfsr = SCB->CFSR;   // если реализован в M23 для SAML10
    volatile uint32_t bfar = SCB->BFAR;
    volatile uint32_t mmfar = SCB->MMFAR;

    (void)r0; (void)r1; (void)r2; (void)r3;
    (void)r12; (void)lr; (void)pc; (void)psr;
    (void)icsr; (void)hfsr; (void)cfsr; (void)bfar; (void)mmfar;

    // Поставь breakpoint на строку ниже
    while (1) {
        __NOP();
    }
}