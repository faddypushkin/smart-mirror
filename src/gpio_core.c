/**
 * @file
 * @brief	GPIO core interface
 */

#include <errno.h>

#include <gpio_core.h>

/**
 * @brief		Validate a GPIO pin descriptor
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @return		error code
 */
static int gpio_core_check_pin(const struct gpio_pin *pin)
{
	if (!pin || !pin->chip)
		return -EINVAL;

	if (!pin->chip->ops)
		return -ENODEV;

	return 0;
}

int gpio_core_pin_toggle(struct gpio_pin *pin)
{
	int ret, value;

	ret = gpio_core_check_pin(pin);
	if (ret < 0)
		return ret;

	if (pin->chip->ops->pin_toggle)
		return pin->chip->ops->pin_toggle(pin);

	value = gpio_core_pin_get(pin);
	if (value < 0)
		return value;

	return gpio_core_pin_set(pin, !value);
}

int gpio_core_pin_set(struct gpio_pin *pin, int value)
{
	int ret;

	ret = gpio_core_check_pin(pin);
	if (ret < 0)
		return ret;

	if (!pin->chip->ops->pin_set)
		return -EOPNOTSUPP;
	return pin->chip->ops->pin_set(pin, !!value);
}

int gpio_core_pin_get(struct gpio_pin *pin)
{
	int ret;

	ret = gpio_core_check_pin(pin);
	if (ret < 0)
		return ret;

	if (!pin->chip->ops->pin_get)
		return -EOPNOTSUPP;
	return pin->chip->ops->pin_get(pin);
}

int gpio_core_pin_config(struct gpio_pin *pin, uint32_t offset, uint8_t flags)
{
	int ret;

	ret = gpio_core_check_pin(pin);
	if (ret < 0)
		return ret;

	if (offset >= pin->chip->ngpio)
		return -EINVAL;
	pin->offset = offset;

	if ((flags & GPIO_CORE_FLAG_PULL_UP) && (flags & GPIO_CORE_FLAG_PULL_DOWN))
		return -EINVAL;
	pin->flags = flags;

	if (!pin->chip->ops->pin_config)
		return -EOPNOTSUPP;
	return pin->chip->ops->pin_config(pin);
}

int gpio_core_init(struct gpio_chip *chip)
{
	if (!chip || !chip->ngpio || !chip->ops)
		return -EINVAL;

	if (!chip->ops->pin_config)
		return -ENOTSUP;

	return 0;
}
