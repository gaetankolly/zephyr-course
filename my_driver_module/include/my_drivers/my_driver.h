/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 * Copyright (c) 2022 Carlo Caione <ccaione@baylibre.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_MY_DRIVER_H_
#define ZEPHYR_INCLUDE_MY_DRIVER_H_

#include <zephyr/device.h>

/**
 * @file
 * @brief cache API interface
 */

#ifdef __cplusplus
extern "C" {
#endif

int my_driver_set_cnt(const struct device *dev, int cnt);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_CACHE_H_ */