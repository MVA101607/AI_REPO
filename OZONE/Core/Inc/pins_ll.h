#pragma once
#include "main.h"

// === Управление питанием (active HIGH) ===
#define PIN_EN_5VPI_PI    EN_5Vpi_Pin
#define PIN_EN_5VPI_PORT  EN_5Vpi_GPIO_Port

#define PIN_EN_8VSD_PI    EN_8Vsd_Pin
#define PIN_EN_8VSD_PORT  EN_8Vsd_GPIO_Port

#define PIN_EN_VPA_PI     EN_VPA_Pin
#define PIN_EN_VPA_PORT   EN_VPA_GPIO_Port

#define PIN_EN_HEAT_PI    EN_HEAT_Pin
#define PIN_EN_HEAT_PORT  EN_HEAT_GPIO_Port

// Boost enable (active LOW: 0 = ON)
#define PIN_BSTDIS_PI     BST_Dis_Pin
#define PIN_BSTDIS_PORT   BST_Dis_GPIO_Port

// RS-485
#define PIN_UDE_PI        UDE_Pin
#define PIN_UDE_PORT      UDE_GPIO_Port

// === Входы ===
#define PIN_EN_FAN_PI     EN_FAN_Pin
#define PIN_EN_FAN_PORT   EN_FAN_GPIO_Port

#define PIN_PAEN1_PI      PAen_Pin
#define PIN_PAEN1_PORT    PAen_GPIO_Port

#define PIN_ONEW1_PI      OneW_Pin
#define PIN_ONEW1_PORT    OneW_GPIO_Port

// === OPI reset / boot / recovery ===
#define PIN_PI_RST1_PI   PI_RST_Pin
#define PIN_PI_RST1_PORT PI_RST_GPIO_Port

#define PIN_PI_BOOT1_PI   PI_Boot_Pin
#define PIN_PI_BOOT1_PORT PI_Boot_GPIO_Port

#define PIN_PI_RCRY1_PI   PI_RCRY_Pin
#define PIN_PI_RCRY1_PORT PI_RCRY_GPIO_Port