/**
 * @file stm32f1xx_hal.h
 * @author d.ponomarev
 * @date Jun 11, 2022
 */

#ifndef STM32F1XX_HAL_MOCK_H_
#define STM32F1XX_HAL_MOCK_H_

#define FLASH_TYPEERASE_PAGES     0x00U
#define FLASH_TYPEPROGRAM_WORD    0x02U

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
  uint32_t PageAddress;
  uint32_t NbPages;
} FLASH_EraseInitTypeDef;

HAL_StatusTypeDef HAL_FLASH_Unlock(void) {return HAL_OK;}
HAL_StatusTypeDef HAL_FLASH_Lock(void) {return HAL_OK;}
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError) {return HAL_OK;}
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data) {return HAL_OK;}

#endif  // STM32F1XX_HAL_MOCK_H_
