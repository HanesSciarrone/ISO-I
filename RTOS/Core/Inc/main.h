#ifndef __MAIN_H // Define to prevent recursive inclusion
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------- Includes ------------- */
#include "stm32f4xx_hal.h"


/* --- Exported functions prototypes -- */
void Error_Handler(void);


/* ---------- Private defines --------- */
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
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
