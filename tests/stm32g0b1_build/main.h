/**
 * @file main.h
 * @author d.ponomarev
 * @date Jun 11, 2022
 */

#ifndef STM32G0XX_HAL_MOCK_H_
#define STM32G0XX_HAL_MOCK_H_

#define FLASH_TYPEERASE_PAGES     0x00U
#define FLASH_TYPEPROGRAM_FAST    0x02U

#include <stdint.h>

typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef struct
{
  uint32_t TypeErase;
  uint32_t Banks;
  uint32_t Page;
  uint32_t NbPages;
} FLASH_EraseInitTypeDef;

void HAL_FLASH_Init(uint32_t initial_addr);
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);

#endif  // STM32G0XX_HAL_MOCK_H_
