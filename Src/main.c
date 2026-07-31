#include <saml10.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FREQ_MCK                48000000UL

//#define OPAMP_REPEATER

#define TIME_MS_SELFDESTRUCT    2000
#define TIME_MS_RTX_OFF         1990
#define TIME_MS_STARTUP         300
#define MIN_FIRE_PUSLE_WIDTH    100
#define BUFLEN                  16
#define DMA_BUF_LEN             96
#define DMA_CHAN_CNT            1
#define MAX_VALUE               200
#define SORT_DIVISOR            8
#define BUF_MAX_VALUE           (MAX_VALUE/SORT_DIVISOR +1)
#define THRES_LOW               60
#define THRES_HI                160
#define THRES_STATISTICAL       10

DmacDescriptor DmaDescriptor[DMA_CHAN_CNT];
DmacDescriptor DmaWriteback[DMA_CHAN_CNT];

volatile uint8_t  bi = 0,
                  Is_Operating = 0,
                  miss_cnt = 0,
                  ready = 0;

volatile uint16_t tmp16,
                  sort_buf[BUF_MAX_VALUE],
                  dmabuf[DMA_BUF_LEN],
                  dmabuf2[DMA_BUF_LEN];

uint16_t  buf1[BUFLEN], buf2[BUFLEN], buf3[BUFLEN];

volatile uint32_t  Time_ms = 0;

void DMAC_Init(void){
  DMAC->BASEADDR.reg = (uint32_t)DmaDescriptor;
  DMAC->WRBADDR.reg  = (uint32_t)DmaWriteback;
  DMAC->PRICTRL0.reg = DMAC_PRICTRL0_LVLPRI1(1UL) |
                       DMAC_PRICTRL0_RRLVLEN1;
  DMAC->CHID.reg = 0;

  DMAC->CHCTRLB.reg  = DMAC_CHCTRLB_TRIGACT_BEAT |
                       DMAC_CHCTRLB_TRIGSRC(0x11) |   // TC2_MC0 
                       DMAC_CHCTRLB_LVL(1);

  DmaDescriptor[0].BTCTRL.reg   = DMAC_BTCTRL_BEATSIZE_HWORD |
                                  DMAC_BTCTRL_DSTINC |
                                  DMAC_BTCTRL_VALID;
  DmaDescriptor[0].BTCNT.reg    = DMA_BUF_LEN;
  DmaDescriptor[0].DSTADDR.reg  = (uint32_t)dmabuf + 2*DMA_BUF_LEN;
  DmaDescriptor[0].SRCADDR.reg  = (uint32_t)&REG_TC2_COUNT16_CC0;   // TC2 CC0
  DmaDescriptor[0].DESCADDR.reg = 0;

  DMAC->CTRL.reg = DMAC_CTRL_LVLEN1 |
                   DMAC_CTRL_DMAENABLE;
}

void DMAC_Start(void){
  DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE;
}

uint16_t DMAC_Stop(void){
  DMAC->CHCTRLA.reg = 0;
  uint16_t count = DMA_BUF_LEN - DMAC->ACTIVE.bit.BTCNT;

  uint16_t i = 0;
  for(; i < count; i++ ){
    dmabuf2[i] = dmabuf[i];
    dmabuf[i]  = 0;
  }
  for(; i < DMA_BUF_LEN; dmabuf[i++] = 0);
  while(DMAC->BUSYCH.bit.BUSYCH0);
  return count;
}

void Sort(uint16_t count){
  uint16_t max = 0, i, val = 0;

  for(i = 0; i < count; i++ ){
    val = dmabuf2[i] / SORT_DIVISOR;
    if (val < BUF_MAX_VALUE) sort_buf[val]++;
  }

  for(i = THRES_LOW/SORT_DIVISOR; i < THRES_HI/SORT_DIVISOR; i++ ){
    if(sort_buf[i] > max){
      max = sort_buf[i];
      val = i;
    }
  }

  val *= SORT_DIVISOR;

  if((max >= THRES_STATISTICAL)&&(val >= THRES_LOW)&&(val <= THRES_HI)){
    PORT->Group[0].OUTSET.reg = 1U << 1;
    miss_cnt = 0;
  } else {
    if(miss_cnt < MIN_FIRE_PUSLE_WIDTH) miss_cnt++;
   // else PORT->Group[0].OUTCLR.reg = 1U << 1;    //закоментить при проверке периода 1 мс
  }

  for(i = 0; i < BUF_MAX_VALUE; sort_buf[i++] = 0 );
}

void TC1_Handler(void){
  //PORT->Group[0].OUTTGL.reg = 1U << 1; //14; проверка периода 1 мс
  Time_ms++;
  ready = DMAC_Stop();

  DMAC_Start();
  TC1->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
}

void main(void){

  OSCCTRL->OSC16MCTRL.bit.FSEL     = OSCCTRL_OSC16MCTRL_FSEL_16_Val;
  OSCCTRL->OSC16MCTRL.bit.ENABLE   = 1;
  OSCCTRL->OSC16MCTRL.bit.ONDEMAND = 0;

  GCLK->PCHCTRL[17].bit.CHEN = 1;          // AC
  GCLK->PCHCTRL[ 6].bit.CHEN = 1;          // EVSYS
  GCLK->PCHCTRL[14].bit.CHEN = 1;          // TC0/TC1, нужен для TC1
  GCLK->PCHCTRL[TC2_GCLK_ID].bit.CHEN = 1; // TC2_GCLK_ID = 15

  PORT->Group[0].PINCFG[ 2].bit.PMUXEN = 1;
  PORT->Group[0].PINCFG[ 4].bit.PMUXEN = 1;


  PORT->Group[0].PMUX[ 1].reg = PORT_PMUX_PMUXE(1);
  PORT->Group[0].PMUX[ 2].reg = PORT_PMUX_PMUXE(1);
  PORT->Group[0].PMUX[ 9].reg = PORT_PMUX_PMUXE(7) | PORT_PMUX_PMUXO(7);


  PORT->Group[0].DIRSET.reg = 1U << 22;
  PORT->Group[0].DIRSET.reg = 1U << 14;
  PORT->Group[0].DIRSET.reg = 1U << 16;
  PORT->Group[0].DIRSET.reg = 1U << 1;

  PORT->Group[0].OUTSET.reg = 1U << 16;
  PORT->Group[0].OUTSET.reg = 1U << 22;

  OPAMP->CTRLA.reg = OPAMP_CTRLA_ENABLE;
  OPAMP->OPAMPCTRL[0].reg = OPAMP_OPAMPCTRL_MUXNEG(1) |
                            OPAMP_OPAMPCTRL_MUXPOS(2) |
                            OPAMP_OPAMPCTRL_POTMUX(7) |
                            OPAMP_OPAMPCTRL_RES1MUX(1) |
                            OPAMP_OPAMPCTRL_RES1EN |
                            OPAMP_OPAMPCTRL_RES2OUT |
                            OPAMP_OPAMPCTRL_BIAS(3);

#ifdef OPAMP_REPEATER
  OPAMP->OPAMPCTRL[1].reg = OPAMP_OPAMPCTRL_MUXNEG(3) |
                            OPAMP_OPAMPCTRL_MUXPOS(4) |
                            OPAMP_OPAMPCTRL_BIAS(3);

  OPAMP->OPAMPCTRL[2].reg = OPAMP_OPAMPCTRL_MUXNEG(3) |
                            OPAMP_OPAMPCTRL_MUXPOS(4) |
                            OPAMP_OPAMPCTRL_ANAOUT |
                            OPAMP_OPAMPCTRL_BIAS(3);
#else
  OPAMP->OPAMPCTRL[1].reg = OPAMP_OPAMPCTRL_MUXNEG(1) |
                            OPAMP_OPAMPCTRL_MUXPOS(2) |
                            OPAMP_OPAMPCTRL_POTMUX(7) |
                            OPAMP_OPAMPCTRL_RES1MUX(2) |
                            OPAMP_OPAMPCTRL_RES1EN |
                            OPAMP_OPAMPCTRL_RES2OUT |
                            OPAMP_OPAMPCTRL_BIAS(3);

  OPAMP->OPAMPCTRL[2].reg = OPAMP_OPAMPCTRL_MUXNEG(1) |
                            OPAMP_OPAMPCTRL_MUXPOS(2) |
                            OPAMP_OPAMPCTRL_POTMUX(7) |
                            OPAMP_OPAMPCTRL_RES1MUX(2) |
                            OPAMP_OPAMPCTRL_RES1EN |
                            OPAMP_OPAMPCTRL_RES2OUT |
                            OPAMP_OPAMPCTRL_ANAOUT |
                            OPAMP_OPAMPCTRL_BIAS(3);
#endif

  OPAMP->RESCTRL.reg = OPAMP_RESCTRL_REFBUFLEVEL(2) |
                       OPAMP_RESCTRL_RES1EN |
                       OPAMP_RESCTRL_RES1MUX;

  AC->EVCTRL.bit.COMPEO0 = 1;
  AC->COMPCTRL[0].reg = AC_COMPCTRL_OUT_OFF |
                        AC_COMPCTRL_HYSTEN |
                        AC_COMPCTRL_HYST_HYST50 |
                        AC_COMPCTRL_SPEED_LOW |
                        AC_COMPCTRL_MUXPOS_PIN0 |
                        AC_COMPCTRL_MUXNEG_PIN1;

  EVSYS->Channel[0].CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_RISING_EDGE |
                                  EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
                                  EVSYS_CHANNEL_EVGEN(0x28);   // COMP0

  EVSYS->USER[EVSYS_ID_USER_TC2_EVU].reg = 1;   // EVSYS_ID_USER_TC2_EVU = 13

  TC2->COUNT16.EVCTRL.reg = TC_EVCTRL_TCEI |
                            TC_EVCTRL_EVACT_PW;  // CC1=period, CC0=pulse width

  TC2->COUNT16.CTRLA.reg = TC_CTRLA_CAPTEN0 |
                           TC_CTRLA_CAPTEN1;

  TC1->COUNT16.INTENSET.bit.OVF = 1;
  TC1->COUNT16.PER.reg = FREQ_MCK/1000UL;

  TC2->COUNT16.CTRLA.bit.ENABLE = 1;
  TC1->COUNT16.CTRLA.bit.ENABLE = 1;

  OPAMP->OPAMPCTRL[0].bit.ENABLE = 1;
  OPAMP->OPAMPCTRL[1].bit.ENABLE = 1;
  OPAMP->OPAMPCTRL[2].bit.ENABLE = 1;

  AC->CTRLA.bit.ENABLE = 1;
  AC->COMPCTRL[0].bit.ENABLE = 1;

  /*  Конфигурация FDPLL96M на 48 МГц */


    // Пробрасываем OSC16M на GCLK1
    // ВНИМАНИЕ! Входной сигнал DFPLL должен быть в диапазоне 32 кГц–2 МГц
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC_OSC16M |
                           //GCLK_GENCTRL_DIVSEL     | // странно, но в этом режиме делится много больше
                           GCLK_GENCTRL_DIV(16) |   // 16 МГц / (15+1) = 1 МГц 
                            GCLK_GENCTRL_OE      |
                           GCLK_GENCTRL_GENEN;
    while(GCLK->SYNCBUSY.bit.GENCTRL1);

    // Подключаем GCLK к  PLL (вход GCLK_DPLL)
    GCLK->PCHCTRL[0].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;

    // Сначала выключаем для настройки
    OSCCTRL->DPLLCTRLA.bit.ENABLE = 0;
    while(OSCCTRL->DPLLSYNCBUSY.bit.ENABLE);

    OSCCTRL->DPLLCTRLA.bit.ONDEMAND = 0;
    while (OSCCTRL->DPLLSYNCBUSY.bit.ENABLE);   // либо просто небольшая синхронизация по регистру

    // умножаем входную частоту (1МГц ?) на целую часть LDR и дробную LDRFRAC/16
    OSCCTRL->DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDR(47) | OSCCTRL_DPLLRATIO_LDRFRAC(0);
    while(OSCCTRL->DPLLSYNCBUSY.bit.DPLLRATIO);

    // Настройка PLL на использование GCLK в качестве опоры
    OSCCTRL->DPLLCTRLB.reg =
        OSCCTRL_DPLLCTRLB_REFCLK_GCLK 
        | OSCCTRL_DPLLCTRLB_LTIME(0);   // auto lock time

    // настройка ножки PA22
    PORT->Group[0].PINCFG[22].bit.PMUXEN = 1;
    PORT->Group[0].PMUX[11].reg = PORT_PMUX_PMUXE(4);   //  4 - функция E - TC0/WO[0]  7- H  -  gclk

    // Включение PLL
    OSCCTRL->DPLLCTRLA.bit.ENABLE = 1;
    while (OSCCTRL->DPLLSYNCBUSY.bit.ENABLE);
    while (!OSCCTRL->DPLLSTATUS.bit.CLKRDY) ;
    while (!OSCCTRL->DPLLSTATUS.bit.LOCK);

    GCLK->GENCTRL[2].reg =     // GCLK_GENCTRL_SRC_OSC16M |
                                 GCLK_GENCTRL_SRC_FDPLL     |
                               // GCLK_GENCTRL_DIVSEL     |       // включить делитель
                               // GCLK_GENCTRL_DIV(0)     |       // делить на 2^(N+1)
                                GCLK_GENCTRL_OE         |
                                GCLK_GENCTRL_GENEN      ;

    while(GCLK->SYNCBUSY.bit.GENCTRL2);

  // Подать GCLK2 на TC0/TC1
  GCLK->PCHCTRL[14].reg = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN;
  while (!(GCLK->PCHCTRL[14].reg & GCLK_PCHCTRL_CHEN));

  // Остановить TC0 перед перенастройкой
  TC0->COUNT8.CTRLA.bit.ENABLE = 0;
  while (TC0->COUNT8.SYNCBUSY.bit.ENABLE);

  // Сброс TC0
  TC0->COUNT8.CTRLA.bit.SWRST = 1;
  while (TC0->COUNT8.SYNCBUSY.bit.SWRST || TC0->COUNT8.CTRLA.bit.SWRST);

  // Режим 8 бит, делитель 1, синхронизация от GCLK
  TC0->COUNT8.CTRLA.reg =
      TC_CTRLA_MODE_COUNT8 |
      TC_CTRLA_PRESCALER_DIV1 |
      TC_CTRLA_PRESCSYNC_GCLK;

  // PWM: выход высокий с начала периода, при совпадении с CC0 уходит в 0,
  // затем ждёт переполнения до TOP (= PER) и снова стартует.
  TC0->COUNT8.WAVE.reg = TC_WAVE_WAVEGEN_NPWM;
  while (TC0->COUNT8.SYNCBUSY.bit.CTRLB);

  TC0->COUNT8.PER.reg = 3;     // TOP - период, считаем с 0 (+1)
  while (TC0->COUNT8.SYNCBUSY.bit.PER);

  TC0->COUNT8.CC[0].reg = 1;     // порог - ширина импульса, тактов
  while (TC0->COUNT8.SYNCBUSY.bit.CC0);

// При необходимости инверсия полярности
// TC0->COUNT8.DRVCTRL.reg = TC_DRVCTRL_INVEN0;

  // Запуск TC0
  TC0->COUNT8.CTRLA.bit.ENABLE = 1;
  while (TC0->COUNT8.SYNCBUSY.bit.ENABLE);



  DMAC_Init();
  DMAC_Start();

  NVIC_EnableIRQ(TC1_IRQn);
  NVIC_SetPriority(TC1_IRQn,1);

  __enable_irq();

  while(1){
    if(Is_Operating && ready) {
      Sort(ready);              // breakpoint here. Watch 'ready' and 'dmabuf2' array
      ready = 0;
    }

    if(Is_Operating == 0)
      if((Time_ms < TIME_MS_RTX_OFF) && (Time_ms > TIME_MS_STARTUP))
        Is_Operating = 1;

    if(Time_ms > TIME_MS_SELFDESTRUCT)
      PORT->Group[0].OUTSET.reg = 1U << 1;

    if(Time_ms > TIME_MS_SELFDESTRUCT+100) {      // для проверки - сбрасываем и работаем по новой
      PORT->Group[0].OUTCLR.reg = 1U << 1;
      Time_ms = 0;  
    }
    
  }
}