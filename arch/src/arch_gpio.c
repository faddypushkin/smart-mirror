/**
 * @file
 * @brief	Arch-specific GPIO controller interface
 */

#include <errno.h>

#include <mik32_hal_pcc.h>
#include <mik32_ll_gpio.h>

#include <arch_gpio.h>

#define MIK32_GPIO_0_PIN_NUM	16
#define MIK32_GPIO_1_PIN_NUM	16
#define MIK32_GPIO_2_PIN_NUM	8

static int mik32_gpio_pin_check(struct gpio_pin *pin)
{
	GPIO_TypeDef *gpio = pin->chip->private_data;

	if ((gpio == GPIO_0 && (pin->offset < P0_0 || pin->offset > P0_15)) ||
	    (gpio == GPIO_1 && (pin->offset < P1_0 || pin->offset > P1_15)) ||
	    (gpio == GPIO_2 && (pin->offset < P2_0 || pin->offset > P2_7)))
		return -ENODEV;

	return 0;
}

static int mik32_gpio_pin_toggle(struct gpio_pin *pin)
{
	int ret;

	ret = mik32_gpio_pin_check(pin);
	if (ret < 0)
		return ret;

	ll_gpio_toggle(pin->offset);

	return 0;
}

static int mik32_gpio_pin_set(struct gpio_pin *pin, int value)
{
	int ret;

	ret = mik32_gpio_pin_check(pin);
	if (ret < 0)
		return ret;

	ll_gpio_write(pin->offset, !!value);

	return 0;
}

static int mik32_gpio_pin_get(struct gpio_pin *pin)
{
	int ret;

	ret = mik32_gpio_pin_check(pin);
	if (ret < 0)
		return ret;

	return ll_gpio_read(pin->offset);
}

static int mik32_gpio_pin_config(struct gpio_pin *pin, off_t offset)
{
	GPIO_TypeDef *gpio = pin->chip->private_data;

	if (gpio == GPIO_0)
		pin->offset = P0_0;
	else if (gpio == GPIO_1)
		pin->offset = P1_0;
	else if (gpio == GPIO_2)
		pin->offset = P2_0;
	else
		return -ENODEV;
	pin->offset += offset;

	ll_gpio_set_mode(pin->offset, PIN_AS_GPIO);

	if (pin->flags & GPIO_CORE_FlAG_DIR_OUT)
		ll_gpio_set_output(pin->offset);
	else
		ll_gpio_set_input(pin->offset);

	if (pin->flags & GPIO_CORE_FlAG_PULL_UP)
		ll_gpio_set_pull_up(pin->offset);
	else if (pin->flags & GPIO_CORE_FlAG_PULL_DOWN)
		ll_gpio_set_pull_down(pin->offset);

	ll_gpio_set_ds(pin->offset, PIN_DS_2);

	return 0;
}

static const struct gpio_chip_ops mik32_gpio_chip_ops = {
	.pin_config	= mik32_gpio_pin_config,
	.pin_get	= mik32_gpio_pin_get,
	.pin_set	= mik32_gpio_pin_set,
	.pin_toggle	= mik32_gpio_pin_toggle,
};

int arch_gpio_init(struct arch_gpio_chip *chip)
{
	if (!chip)
		return -EINVAL;

	if (chip->gpio == GPIO_0) {
		__HAL_PCC_GPIO_0_CLK_ENABLE();
		chip->ngpio = MIK32_GPIO_0_PIN_NUM;
	} else if (chip->gpio == GPIO_1) {
		__HAL_PCC_GPIO_1_CLK_ENABLE();
		chip->ngpio = MIK32_GPIO_1_PIN_NUM;
	} else if (chip->gpio == GPIO_2) {
		__HAL_PCC_GPIO_2_CLK_ENABLE();
		chip->ngpio = MIK32_GPIO_2_PIN_NUM;
	} else {
		return -ENODEV;
	}

	chip->ops = &mik32_gpio_chip_ops;
	chip->private_data = chip->gpio;

	return gpio_core_init(chip);
}
