/**
 * @file
 * @brief	Arch-specific GPIO API
 */

#ifndef ARCH_GPIO_H_
#define ARCH_GPIO_H_

#include <gpio.h>

#include <gpio_core.h>

struct arch_gpio_chip {
	struct gpio_chip;
	GPIO_TypeDef			*gpio;
};

int arch_gpio_init(struct arch_gpio_chip *chip);

#endif /* ARCH_GPIO_H_ */
