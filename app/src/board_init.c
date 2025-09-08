/*
 * Copyright (c) 2025 CANnectivity Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#include "board_init.h"

LOG_MODULE_REGISTER(board_init, CONFIG_CANNECTIVITY_LOG_LEVEL);

#ifdef CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES

#include <soc.h>
#include <stm32g0xx_ll_system.h>
#include <stm32g0xx_ll_flash.h>

/**
 * @brief Configure STM32G0 option bytes for BOOT0 pin
 *
 * This function checks and configures the nBOOT_SEL option bit to ensure
 * BOOT0 pin is used for boot mode selection on STM32G0 devices.
 * 
 * For dual CAN-FD boards based on STM32G0, we need to ensure:
 * - nBOOT_SEL = 0: BOOT0 pin is used for boot mode selection
 * - This allows proper boot mode control via hardware pin
 */
static int stm32g0_configure_option_bytes(void)
{
	uint32_t optr_reg;
	
	/* Read current option byte register */
	optr_reg = READ_REG(FLASH->OPTR);
	
	/* Check if nBOOT_SEL needs to be configured (bit should be 0) */
	if (optr_reg & FLASH_OPTR_nBOOT_SEL) {
		LOG_INF("nBOOT_SEL is set to 1, configuring to use BOOT0 pin");
		
		/* Unlock FLASH control register */
		WRITE_REG(FLASH->KEYR, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR, FLASH_KEY2);
		
		/* Check if FLASH is unlocked */
		if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
			LOG_ERR("Failed to unlock FLASH");
			return -EIO;
		}
		
		/* Unlock option bytes */
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY1);
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY2);
		
		/* Check if option bytes are unlocked */
		if (READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) != 0U) {
			/* Lock FLASH back */
			SET_BIT(FLASH->CR, FLASH_CR_LOCK);
			LOG_ERR("Failed to unlock option bytes");
			return -EIO;
		}
		
		/* Clear nBOOT_SEL bit to enable BOOT0 pin */
		optr_reg &= ~FLASH_OPTR_nBOOT_SEL;
		WRITE_REG(FLASH->OPTR, optr_reg);
		
		/* Start option byte programming */
		SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
		
		/* Wait for operation to complete */
		while (READ_BIT(FLASH->SR, FLASH_SR_BSY1) != 0U) {
			/* Wait for flash operation */
		}
		
		/* Check for errors */
		if (READ_BIT(FLASH->SR, FLASH_SR_OPTVERR) != 0U) {
			/* Clear error flag */
			SET_BIT(FLASH->SR, FLASH_SR_OPTVERR);
			LOG_ERR("Option byte verification error");
		}
		
		/* Trigger option byte reload */
		SET_BIT(FLASH->CR, FLASH_CR_OBL_LAUNCH);
		
		/* Lock option bytes and FLASH */
		SET_BIT(FLASH->CR, FLASH_CR_OPTLOCK);
		SET_BIT(FLASH->CR, FLASH_CR_LOCK);
		
		LOG_INF("Option bytes configured successfully");
		
		/* Note: The OBL_LAUNCH will cause a system reset */
		LOG_WRN("System will reset to apply option byte changes");
	} else {
		LOG_DBG("nBOOT_SEL already configured correctly (0)");
	}
	
	return 0;
}
#endif /* CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES */

#ifdef CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM
/* Weak implementation that can be overridden by board-specific code */
__weak int cannectivity_board_init_custom(void)
{
	/* Default: no custom initialization */
	return 0;
}
#endif

int cannectivity_board_init(void)
{
	int err = 0;

#ifdef CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES
	err = stm32g0_configure_option_bytes();
	if (err != 0) {
		LOG_ERR("Failed to configure STM32G0 option bytes (err %d)", err);
		return err;
	}
#endif

#ifdef CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM
	err = cannectivity_board_init_custom();
	if (err != 0) {
		LOG_ERR("Custom board initialization failed (err %d)", err);
		return err;
	}
#endif

	return 0;
}

/* Register board initialization to run early in the boot process */
#ifdef CONFIG_CANNECTIVITY_BOARD_INIT_EARLY
SYS_INIT(cannectivity_board_init, PRE_KERNEL_1, CONFIG_CANNECTIVITY_BOARD_INIT_PRIORITY);
#endif