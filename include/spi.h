/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

