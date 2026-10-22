/**
 * @file
 * @brief	GPIO controller interface
 */

#include <errno.h>

#include <gpio_core.h>

int gpio_core_pin_toggle(struct gpio_pin *pin)
{
	int value;

	if (!pin)
		return -EINVAL;

	if (pin->chip->ops->pin_toggle)
		return pin->chip->ops->pin_toggle(pin);

	value = gpio_core_pin_get(pin);
	if (value < 0)
		return value;

	return gpio_core_pin_set(pin, !value);
}

int gpio_core_pin_set(struct gpio_pin *pin, int value)
{
	if (!(pin && pin->chip->ops->pin_set))
		return -EOPNOTSUPP;

	return pin->chip->ops->pin_set(pin, value);
}

int gpio_core_pin_get(struct gpio_pin *pin)
{
	if (!(pin && pin->chip->ops->pin_get))
		return -EOPNOTSUPP;

	return pin->chip->ops->pin_get(pin);
}

int gpio_core_pin_config(struct gpio_pin *pin, off_t offset, uint8_t flags)
{
	if (!(pin && pin->chip))
		return -EINVAL;

	if (offset < 0 || offset >= pin->chip->ngpio)
		return -EINVAL;

	if ((flags & GPIO_CORE_FlAG_PULL_UP) && (flags & GPIO_CORE_FlAG_PULL_DOWN))
		return -EINVAL;
	pin->flags = flags;

	if (!pin->chip->ops->pin_config)
		return -EOPNOTSUPP;
	return pin->chip->ops->pin_config(pin, offset);
}

int gpio_core_init(struct gpio_chip *chip)
{
	if (!(chip->ops && chip->ops->pin_config))
		return -ENOTSUP;

	return 0;
}
