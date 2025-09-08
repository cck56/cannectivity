/*
 * Copyright (c) 2025 CANnectivity Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Example custom board initialization implementation
 *
 * This file demonstrates how to implement custom board-specific
 * initialization routines for boards that need special setup
 * beyond what's provided by the standard initialization.
 *
 * To use this:
 * 1. Copy this file to your board directory
 * 2. Rename and modify as needed
 * 3. Add to your CMakeLists.txt
 * 4. Enable CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM in your board config
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(board_init);

/**
 * @brief Custom board initialization
 *
 * This function is called during board initialization when
 * CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM is enabled.
 *
 * Example use cases:
 * - Configure special GPIO pins
 * - Set up board-specific power management
 * - Initialize external peripherals
 * - Configure board-specific clocks
 *
 * @return 0 on success, negative errno on failure
 */
int cannectivity_board_init_custom(void)
{
	int ret;

	LOG_INF("Running custom board initialization");

	/* Example: Configure a board-specific enable pin */
#ifdef CONFIG_BOARD_HAS_CAN_ENABLE_PIN
	const struct gpio_dt_spec can_enable = GPIO_DT_SPEC_GET(
		DT_NODELABEL(can_enable_pin), gpios);

	if (gpio_is_ready_dt(&can_enable)) {
		ret = gpio_pin_configure_dt(&can_enable, GPIO_OUTPUT_ACTIVE);
		if (ret < 0) {
			LOG_ERR("Failed to configure CAN enable pin: %d", ret);
			return ret;
		}
		LOG_INF("CAN transceiver enabled");
	}
#endif

	/* Example: Configure board-specific LED patterns */
#ifdef CONFIG_BOARD_CUSTOM_LED_PATTERN
	/* Set up custom LED initialization pattern */
	LOG_INF("Configuring custom LED pattern");
	/* Your LED configuration code here */
#endif

	/* Example: Initialize external watchdog */
#ifdef CONFIG_BOARD_HAS_EXTERNAL_WATCHDOG
	LOG_INF("Initializing external watchdog");
	/* Your watchdog initialization code here */
#endif

	LOG_INF("Custom board initialization complete");
	return 0;
}