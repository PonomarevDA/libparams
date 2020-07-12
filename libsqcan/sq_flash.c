/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file sq_flash.c
 * @author sainquake
 */

#include "sq_flash.h"
#include "uavcan.h"

void flash_unlock(){
	HAL_FLASH_Unlock();
}
void flash_lock(){
	HAL_FLASH_Lock();
}
void flash_erase(){
	  FLASH_EraseInitTypeDef FLASH_EraseInitStruct =
	  {
	      	.TypeErase = FLASH_TYPEERASE_PAGES,
			.PageAddress = (uint32_t)STORAGE_PAGE,
			.NbPages = 1
	   };
	   uint32_t error = 0;
	   HAL_FLASHEx_Erase(&FLASH_EraseInitStruct,&error);
}

uint8_t flash_write(uint32_t address, uint32_t *data, uint16_t size)
{
   for (int i = 0; i < size; i++)
     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (address)+(i*4), data[i]) != HAL_OK)
    	return 1;
   return 0;
}

/*uint8_t flash_write_params(uint32_t address, param_t *data, uint16_t size)
{
   for (int i = 0; i < size; i++)
     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (address)+(i*4), data[i].val) != HAL_OK)
    	return 1;
   return 0;
}*/
void flash_write_param(uint32_t address, uint32_t data)
{
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
}
