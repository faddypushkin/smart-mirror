/**
 * @file
 * @brief	GPIO core API
 */

#ifndef GPIO_CORE_H_
#define GPIO_CORE_H_

#include <stddef.h>
#include <stdint.h>

#include <bits.h>

/**
 * @defgroup	GPIO core flags
 * @{
 */
#define GPIO_CORE_FLAG_DIR_OUT		BIT(0)
#define GPIO_CORE_FLAG_PULL_UP		BIT(1)
#define GPIO_CORE_FLAG_PULL_DOWN	BIT(2)
/**
 * @}
 */

struct gpio_chip;
struct gpio_pin;

/** GPIO chip operations */
struct gpio_chip_ops {
	/** Configure a GPIO pin */
	int (*pin_config)(struct gpio_pin *pin);
	/** Set the output level of a GPIO pin */
	int (*pin_set)(struct gpio_pin *pin, int value);
	/** Read the level of a GPIO pin */
	int (*pin_get)(struct gpio_pin *pin);
	/** Toggle the output level of a GPIO pin */
	int (*pin_toggle)(struct gpio_pin *pin);
};

/** GPIO chip descriptor */
struct gpio_chip {
	/** GPIO chip operations */
	const struct gpio_chip_ops	*ops;
	/** Number of pins controlled by this GPIO chip */
	size_t						ngpio;
	/** GPIO chip-specific data */
	void						*private_data;
};

/** GPIO pin descriptor */
struct gpio_pin {
	/** GPIO chip descriptor */
	struct gpio_chip		*chip;
	/** Logical offset of the pin within the GPIO chip */
	uint32_t			    offset;
	/** GPIO pin flags */
	uint8_t					flags;
};

/**
 * @brief		Toggle the output level of a GPIO pin
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @return		error code
 */
int gpio_core_pin_toggle(struct gpio_pin *pin);

/**
 * @brief		Set the output level of a GPIO pin
 *
 * @param[in]	pin		GPIO pin descriptor
 * @param[in]	value	output level
 *
 * @return		error code
 */
int gpio_core_pin_set(struct gpio_pin *pin, int value);

/**
 * @brief		Read the level of a GPIO pin
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @retval		<0	error code
 * @retval		>=0	the level of a GPIO pin
 */
int gpio_core_pin_get(struct gpio_pin *pin);

/**
 * @brief		Configure a GPIO pin
 *
 * @param[in,out]	pin		GPIO pin descriptor
 * @param[in]		offset	GPIO pin offset in the range [0, pin->chip->ngpio)
 * @param[in]		flags	GPIO pin flags
 *
 * @return		error code
 */
int gpio_core_pin_config(struct gpio_pin *pin, uint32_t offset, uint8_t flags);

/**
 * @brief		Initialize a GPIO chip
 *
 * @param[in]	chip	GPIO chip descriptor
 *
 * @return		error code
 */
int gpio_core_init(struct gpio_chip *chip);

#endif /* GPIO_CORE_H_ */
