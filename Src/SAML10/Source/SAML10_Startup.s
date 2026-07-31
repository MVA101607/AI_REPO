;/**************************************************************************//**
; * @file     startup_ARMCM23.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM23 Device
; * @version  V1.1.0
; * @date     08. April 2021
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  CSTACK:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                PUBLIC   __vector_table_0x1c
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size

                #define __INITIAL_SP     sfe(CSTACK)
                #define __STACK_LIMIT    sfb(CSTACK)
                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                SECTION STACKSEAL:DATA:NOROOT(3)
                #define __STACK_SEAL     sfb(STACKSEAL)
                #endif

                DATA

__vector_table
                DCD      __INITIAL_SP                        ;     Top of Stack
                DCD      Reset_Handler                       ;     Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
__vector_table_0x1c
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      SVC_Handler                         ;  -5 SVCall Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      PendSV_Handler                      ;  -2 PendSV Handler
                DCD      SysTick_Handler                     ;  -1 SysTick Handler

                ;  Peripheral handlers ; 
                DCD     SYSTEM_Handler                ;   0 MCLK, OSCCTRL, OSC32KCTRL, PM, SUPC ; 
                DCD     WDT_Handler                   ;   1 Watchdog Timer ; 
                DCD     RTC_Handler                   ;   2 Real-Time Counter ; 
                DCD  EIC_0_Handler                 ;   3 External Interrupt Controller IRQ 0 ; 
                DCD  EIC_1_Handler                 ;   4 External Interrupt Controller IRQ 1 ; 
                DCD  EIC_2_Handler                 ;   5 External Interrupt Controller IRQ 2 ; 
                DCD  EIC_3_Handler                 ;   6 External Interrupt Controller IRQ 3 ; 
                DCD  EIC_4_Handler                 ;   7 External Interrupt Controller IRQ 4 ; 
                DCD  FREQM_Handler                 ;   8 Frequency Meter ; 
                DCD  NVMCTRL_Handler               ;   9 Non-Volatile Memory Controller ; 
                DCD  PORT_Handler                  ;  10 Port Module ; 
                DCD  DMAC_0_Handler                ;  11 Direct Memory Access Controller IRQ 0 ; 
                DCD  DMAC_1_Handler                ;  12 Direct Memory Access Controller IRQ 1 ; 
                DCD  DMAC_2_Handler                ;  13 Direct Memory Access Controller IRQ 2 ; 
                DCD  DMAC_3_Handler                ;  14 Direct Memory Access Controller IRQ 3 ; 
                DCD  DMAC_4_Handler                ;  15 Direct Memory Access Controller IRQ 4 ; 
                DCD  EVSYS_0_Handler               ;  16 Event System Interface IRQ 0 ; 
                DCD  EVSYS_1_Handler               ;  17 Event System Interface IRQ 1 ; 
                DCD  EVSYS_2_Handler               ;  18 Event System Interface IRQ 2 ; 
                DCD  EVSYS_3_Handler               ;  19 Event System Interface IRQ 3 ; 
                DCD  EVSYS_4_Handler               ;  20 Event System Interface IRQ 4 ; 
                DCD  PAC_Handler                   ;  21 Peripheral Access Controller ; 
                DCD  SERCOM0_0_Handler             ;  22 Serial Communication Interface 0 IRQ 0 ; 
                DCD  SERCOM0_1_Handler             ;  23 Serial Communication Interface 0 IRQ 1 ; 
                DCD  SERCOM0_2_Handler             ;  24 Serial Communication Interface 0 IRQ 2 ; 
                DCD  SERCOM0_3_Handler             ;  25 Serial Communication Interface 0 IRQ 3 ; 
                DCD  SERCOM1_0_Handler             ;  26 Serial Communication Interface 1 IRQ 0 ; 
                DCD  SERCOM1_1_Handler             ;  27 Serial Communication Interface 1 IRQ 1 ; 
                DCD  SERCOM1_2_Handler             ;  28 Serial Communication Interface 1 IRQ 2 ; 
                DCD  SERCOM1_3_Handler             ;  29 Serial Communication Interface 1 IRQ 3 ; 
                DCD  pvReserved30
                DCD  pvReserved31
                DCD  pvReserved32
                DCD  pvReserved33
                DCD  TC0_Handler                   ;  34 Basic Timer Counter 0 ; 
                DCD  TC1_Handler                   ;  35 Basic Timer Counter 1 ; 
                DCD  TC2_Handler                   ;  36 Basic Timer Counter 2 ; 
                DCD  ADC_0_Handler                 ;  37 Analog Digital Converter IRQ 0 ; 
                DCD  ADC_1_Handler                 ;  38 Analog Digital Converter IRQ 1 ; 
                DCD  AC_Handler                    ;  39 Analog Comparators ; 
                DCD  DAC_0_Handler                 ;  40 Digital Analog Converter IRQ 0 ; 
                DCD  DAC_1_Handler                 ;  41 Digital Analog Converter IRQ 1 ; 
                DCD  PTC_Handler                   ;  42 Peripheral Touch Controller ; 
                DCD  TRNG_Handler                  ;  43 True Random Generator ; 
                DCD  TRAM_Handler                  ;  44 TrustRAM ; 
                DS32    (179)                                ; Interrupts 10 .. 224 are left out
__Vectors_End

__Vectors       EQU      __vector_table
__Vectors_Size  EQU      __Vectors_End - __Vectors


                THUMB

; Reset Handler

                PUBWEAK  Reset_Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
Reset_Handler
                ldr      r0, =__INITIAL_SP
                msr      psp, r0

                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                ldr      r0, =__STACK_LIMIT
                msr      msplim, r0
                msr      psplim, r0

                ldr      r0, =__STACK_SEAL
                ldr      r1, =0xFEF5EDA5U
                str      r1,[r0,#0]
                str      r1,[r0,#4]
                #endif

                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__iar_program_start
                BX       R0


                PUBWEAK NMI_Handler
                PUBWEAK HardFault_Handler
                PUBWEAK SVC_Handler
                PUBWEAK PendSV_Handler
                PUBWEAK SysTick_Handler

                ;  Peripheral handlers ; 
                PUBWEAK  SYSTEM_Handler                ;   0 MCLK, OSCCTRL, OSC32KCTRL, PM, SUPC ; 
                PUBWEAK  WDT_Handler                   ;   1 Watchdog Timer ; 
                PUBWEAK  RTC_Handler                   ;   2 Real-Time Counter ; 
                PUBWEAK  EIC_0_Handler                 ;   3 External Interrupt Controller IRQ 0 ; 
                PUBWEAK  EIC_1_Handler                 ;   4 External Interrupt Controller IRQ 1 ; 
                PUBWEAK  EIC_2_Handler                 ;   5 External Interrupt Controller IRQ 2 ; 
                PUBWEAK  EIC_3_Handler                 ;   6 External Interrupt Controller IRQ 3 ; 
                PUBWEAK  EIC_4_Handler                 ;   7 External Interrupt Controller IRQ 4 ; 
                PUBWEAK  FREQM_Handler                 ;   8 Frequency Meter ; 
                PUBWEAK  NVMCTRL_Handler               ;   9 Non-Volatile Memory Controller ; 
                PUBWEAK  PORT_Handler                  ;  10 Port Module ; 
                PUBWEAK  DMAC_0_Handler                ;  11 Direct Memory Access Controller IRQ 0 ; 
                PUBWEAK  DMAC_1_Handler                ;  12 Direct Memory Access Controller IRQ 1 ; 
                PUBWEAK  DMAC_2_Handler                ;  13 Direct Memory Access Controller IRQ 2 ; 
                PUBWEAK  DMAC_3_Handler                ;  14 Direct Memory Access Controller IRQ 3 ; 
                PUBWEAK  DMAC_4_Handler                ;  15 Direct Memory Access Controller IRQ 4 ; 
                PUBWEAK  EVSYS_0_Handler               ;  16 Event System Interface IRQ 0 ; 
                PUBWEAK  EVSYS_1_Handler               ;  17 Event System Interface IRQ 1 ; 
                PUBWEAK  EVSYS_2_Handler               ;  18 Event System Interface IRQ 2 ; 
                PUBWEAK  EVSYS_3_Handler               ;  19 Event System Interface IRQ 3 ; 
                PUBWEAK  EVSYS_4_Handler               ;  20 Event System Interface IRQ 4 ; 
                PUBWEAK  PAC_Handler                   ;  21 Peripheral Access Controller ; 
                PUBWEAK  SERCOM0_0_Handler             ;  22 Serial Communication Interface 0 IRQ 0 ; 
                PUBWEAK  SERCOM0_1_Handler             ;  23 Serial Communication Interface 0 IRQ 1 ; 
                PUBWEAK  SERCOM0_2_Handler             ;  24 Serial Communication Interface 0 IRQ 2 ; 
                PUBWEAK  SERCOM0_3_Handler             ;  25 Serial Communication Interface 0 IRQ 3 ; 
                PUBWEAK  SERCOM1_0_Handler             ;  26 Serial Communication Interface 1 IRQ 0 ; 
                PUBWEAK  SERCOM1_1_Handler             ;  27 Serial Communication Interface 1 IRQ 1 ; 
                PUBWEAK  SERCOM1_2_Handler             ;  28 Serial Communication Interface 1 IRQ 2 ; 
                PUBWEAK  SERCOM1_3_Handler             ;  29 Serial Communication Interface 1 IRQ 3 ; 
                PUBWEAK  pvReserved30
                PUBWEAK  pvReserved31
                PUBWEAK  pvReserved32
                PUBWEAK  pvReserved33
                PUBWEAK  TC0_Handler                   ;  34 Basic Timer Counter 0 ; 
                PUBWEAK  TC1_Handler                   ;  35 Basic Timer Counter 1 ; 
                PUBWEAK  TC2_Handler                   ;  36 Basic Timer Counter 2 ; 
                PUBWEAK  ADC_0_Handler                 ;  37 Analog Digital Converter IRQ 0 ; 
                PUBWEAK  ADC_1_Handler                 ;  38 Analog Digital Converter IRQ 1 ; 
                PUBWEAK  AC_Handler                    ;  39 Analog Comparators ; 
                PUBWEAK  DAC_0_Handler                 ;  40 Digital Analog Converter IRQ 0 ; 
                PUBWEAK  DAC_1_Handler                 ;  41 Digital Analog Converter IRQ 1 ; 
                PUBWEAK  PTC_Handler                   ;  42 Peripheral Touch Controller ; 
                PUBWEAK  TRNG_Handler                  ;  43 True Random Generator ; 
                PUBWEAK  TRAM_Handler                  ;  44 TrustRAM ; 
                SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
HardFault_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler

  ;  Peripheral handlers ; 
SYSTEM_Handler                ;   0 MCLK, OSCCTRL, OSC32KCTRL, PM, SUPC ; 
WDT_Handler                   ;   1 Watchdog Timer ; 
RTC_Handler                   ;   2 Real-Time Counter ; 
EIC_0_Handler                 ;   3 External Interrupt Controller IRQ 0 ; 
EIC_1_Handler                 ;   4 External Interrupt Controller IRQ 1 ; 
EIC_2_Handler                 ;   5 External Interrupt Controller IRQ 2 ; 
EIC_3_Handler                 ;   6 External Interrupt Controller IRQ 3 ; 
EIC_4_Handler                 ;   7 External Interrupt Controller IRQ 4 ; 
FREQM_Handler                 ;   8 Frequency Meter ; 
NVMCTRL_Handler               ;   9 Non-Volatile Memory Controller ; 
PORT_Handler                  ;  10 Port Module ; 
DMAC_0_Handler                ;  11 Direct Memory Access Controller IRQ 0 ; 
DMAC_1_Handler                ;  12 Direct Memory Access Controller IRQ 1 ; 
DMAC_2_Handler                ;  13 Direct Memory Access Controller IRQ 2 ; 
DMAC_3_Handler                ;  14 Direct Memory Access Controller IRQ 3 ; 
DMAC_4_Handler                ;  15 Direct Memory Access Controller IRQ 4 ; 
EVSYS_0_Handler               ;  16 Event System Interface IRQ 0 ; 
EVSYS_1_Handler               ;  17 Event System Interface IRQ 1 ; 
EVSYS_2_Handler               ;  18 Event System Interface IRQ 2 ; 
EVSYS_3_Handler               ;  19 Event System Interface IRQ 3 ; 
EVSYS_4_Handler               ;  20 Event System Interface IRQ 4 ; 
PAC_Handler                   ;  21 Peripheral Access Controller ; 
SERCOM0_0_Handler             ;  22 Serial Communication Interface 0 IRQ 0 ; 
SERCOM0_1_Handler             ;  23 Serial Communication Interface 0 IRQ 1 ; 
SERCOM0_2_Handler             ;  24 Serial Communication Interface 0 IRQ 2 ; 
SERCOM0_3_Handler             ;  25 Serial Communication Interface 0 IRQ 3 ; 
SERCOM1_0_Handler             ;  26 Serial Communication Interface 1 IRQ 0 ; 
SERCOM1_1_Handler             ;  27 Serial Communication Interface 1 IRQ 1 ; 
SERCOM1_2_Handler             ;  28 Serial Communication Interface 1 IRQ 2 ; 
SERCOM1_3_Handler             ;  29 Serial Communication Interface 1 IRQ 3 ; 
pvReserved30
pvReserved31
pvReserved32
pvReserved33
TC0_Handler                   ;  34 Basic Timer Counter 0 ; 
TC1_Handler                   ;  35 Basic Timer Counter 1 ; 
TC2_Handler                   ;  36 Basic Timer Counter 2 ; 
ADC_0_Handler                 ;  37 Analog Digital Converter IRQ 0 ; 
ADC_1_Handler                 ;  38 Analog Digital Converter IRQ 1 ; 
AC_Handler                    ;  39 Analog Comparators ; 
DAC_0_Handler                 ;  40 Digital Analog Converter IRQ 0 ; 
DAC_1_Handler                 ;  41 Digital Analog Converter IRQ 1 ; 
PTC_Handler                   ;  42 Peripheral Touch Controller ; 
TRNG_Handler                  ;  43 True Random Generator ; 
TRAM_Handler                  ;  44 TrustRAM ; 
Default_Handler
                B        .


                END