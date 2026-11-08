/**
 * @file
 * @brief	Arch-specific GPIO API
 */

#ifndef ARCH_GPIO_H_
#define ARCH_GPIO_H_

#include <gpio.h>

#include <gpio_core.h>

/** Arch-specific GPIO chip descriptor */
struct arch_gpio_chip {
	/** GPIO chip descriptor */
	struct gpio_chip;
	/** MIK32 GPIO register block */
	GPIO_TypeDef			*gpio;
};

/**
 * @brief		Initialize an arch-specific GPIO chip
 *
 * @param[in]	chip	Arch-specific GPIO chip descriptor
 *
 * @return		error code
 */
int arch_gpio_init(struct arch_gpio_chip *chip);

#endif /* ARCH_GPIO_H_ */
