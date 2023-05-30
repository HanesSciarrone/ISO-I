/* ----------- Define to prevent recursive inclusion ---------- */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------- Includes ------------------------- */
#include "stm32f4xx_hal.h"

/* --------------- Exported functions prototypes -------------- */
void Error_Handler(void);

/* --------------------- Private defines --------------------- */
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H
