/*
 * Copyright (c) 2024 Your Organization
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <stm32g0xx_hal.h>
#include <stm32g0xx_hal_flash_ex.h>

LOG_MODULE_REGISTER(board_dualcanfd_g0, LOG_LEVEL_INF);

#define BOARD_INIT_PRIORITY 65

static int board_dualcanfd_g0_init(void)
{
	FLASH_OBProgramInitTypeDef ob_config = {0};
	HAL_StatusTypeDef status;
	bool need_update = false;

	/* Read current option bytes configuration */
	HAL_FLASHEx_OBGetConfig(&ob_config);

	/* Check if nBOOT_SEL needs to be updated */
	if ((ob_config.USERConfig & OB_BOOT0_FROM_OB) != 0) {
		LOG_INF("nBOOT_SEL is currently set to 1 (BOOT0 from option bit)");
		need_update = true;
	} else {
		LOG_INF("nBOOT_SEL is already set to 0 (BOOT0 from pin)");
		return 0;
	}

	if (need_update) {
		LOG_INF("Updating option bytes to set nBOOT_SEL = 0");

		/* Unlock Flash and Option Bytes */
		status = HAL_FLASH_Unlock();
		if (status != HAL_OK) {
			LOG_ERR("Failed to unlock flash");
			return -EIO;
		}

		status = HAL_FLASH_OB_Unlock();
		if (status != HAL_OK) {
			LOG_ERR("Failed to unlock option bytes");
			HAL_FLASH_Lock();
			return -EIO;
		}

		/* Configure option bytes */
		ob_config.OptionType = OPTIONBYTE_USER;
		ob_config.USERType = OB_USER_nBOOT_SEL;
		/* Clear nBOOT_SEL bit to set it to 0 (BOOT0 from pin) */
		ob_config.USERConfig = ob_config.USERConfig & ~OB_BOOT0_FROM_OB;

		/* Program option bytes */
		status = HAL_FLASHEx_OBProgram(&ob_config);
		if (status != HAL_OK) {
			LOG_ERR("Failed to program option bytes");
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
			return -EIO;
		}

		/* Launch option bytes loading */
		status = HAL_FLASH_OB_Launch();
		if (status != HAL_OK) {
			LOG_ERR("Failed to launch option bytes");
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
			return -EIO;
		}

		/* Lock Flash and Option Bytes */
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();

		LOG_INF("Option bytes updated successfully, system will reset");
		
		/* Note: The system will reset after OB_Launch */
		/* This code should not be reached */
	}

	return 0;
}

/* Initialize at POST_KERNEL level to ensure flash subsystem is ready */
SYS_INIT(board_dualcanfd_g0_init, POST_KERNEL, BOARD_INIT_PRIORITY);
