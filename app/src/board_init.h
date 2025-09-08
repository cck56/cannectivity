/*
 * Copyright (c) 2025 CANnectivity Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CANNECTIVITY_BOARD_INIT_H
#define CANNECTIVITY_BOARD_INIT_H

#include <zephyr/types.h>

/**
 * @brief Board-specific initialization hook
 *
 * This function is called early during system initialization to perform
 * board-specific setup that cannot be handled through devicetree or
 * Kconfig alone.
 *
 * @return 0 on success, negative errno on failure
 */
int cannectivity_board_init(void);

#endif /* CANNECTIVITY_BOARD_INIT_H */