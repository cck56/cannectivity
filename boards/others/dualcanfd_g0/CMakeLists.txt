# Copyright (c) 2024 Your Organization
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BOARD_DUALCANFD_G0)
  zephyr_library()
  zephyr_library_sources(board.c)
  
  # Manually include STM32 HAL Flash sources for option bytes programming
  zephyr_library_sources(
    ${ZEPHYR_HAL_STM32_MODULE_DIR}/stm32cube/stm32g0xx/drivers/src/stm32g0xx_hal_flash.c
    ${ZEPHYR_HAL_STM32_MODULE_DIR}/stm32cube/stm32g0xx/drivers/src/stm32g0xx_hal_flash_ex.c
  )
endif()
