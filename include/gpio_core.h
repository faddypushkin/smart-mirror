/**
 * @file
 * @brief	GPIO API
 */

#ifndef GPIO_CORE_H_
#define GPIO_CORE_H_

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

#include <bits.h>

#define GPIO_CORE_FlAG_DIR_OUT		BIT(0)
#define GPIO_CORE_FlAG_PULL_UP		BIT(1)
#define GPIO_CORE_FlAG_PULL_DOWN	BIT(2)

struct gpio_chip;
struct gpio_pin;

struct gpio_chip_ops {
	int (*pin_config)(struct gpio_pin *pin, off_t offset);
	int (*pin_set)(struct gpio_pin *pin, int value);
	int (*pin_get)(struct gpio_pin *pin);
	int (*pin_toggle)(struct gpio_pin *pin);
};

struct gpio_chip {
	const struct gpio_chip_ops	*ops;
	size_t						ngpio;
	void						*private_data;
};

struct gpio_pin {
	struct gpio_chip		*chip;
	uint32_t				offset;
	uint8_t					flags;
};

int gpio_core_pin_toggle(struct gpio_pin *pin);

int gpio_core_pin_set(struct gpio_pin *pin, int value);

int gpio_core_pin_get(struct gpio_pin *pin);

int gpio_core_pin_config(struct gpio_pin *pin, off_t offset, uint8_t flags);

int gpio_core_init(struct gpio_chip *gpio);

#endif /* GPIO_CORE_H_ */
