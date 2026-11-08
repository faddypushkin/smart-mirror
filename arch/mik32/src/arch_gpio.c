/**
 * @file
 * @brief	Arch-specific GPIO interface
 */

#include <errno.h>

#include <mik32_hal_pcc.h>
#include <mik32_ll_gpio.h>
#include <mik32_memory_map.h>

#include <arch_gpio.h>

/**
 * @defgroup	MIK32 GPIO pin counts
 * @{
 */
#define MIK32_GPIO_0_PIN_NUM	16
#define MIK32_GPIO_1_PIN_NUM	16
#define MIK32_GPIO_2_PIN_NUM	8
/**
 * @}
 */

/**
 * @brief		Convert a GPIO pin offset to a MIK32 hardware pin offset
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @retval		<0	error code
 * @retval		>=0	the MIK32 hardware pin offset
 */
static int mik32_gpio_pin_get_hw_offset(struct gpio_pin *pin)
{
	GPIO_TypeDef *gpio = pin->chip->private_data;
	uint32_t ret;

	if (gpio == GPIO_0)
		ret = P0_0;
	else if (gpio == GPIO_1)
		ret = P1_0;
	else if (gpio == GPIO_2)
		ret = P2_0;
	else
		return -ENODEV;

	if (pin->offset < pin->chip->ngpio)
		ret += pin->offset;
	else
		return -EINVAL;

	return ret;
}

/**
 * @brief		Toggle the output level of a GPIO pin
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @return		error code
 */
static int mik32_gpio_pin_toggle(struct gpio_pin *pin)
{
	int hw_offset = mik32_gpio_pin_get_hw_offset(pin);

	if (hw_offset < 0)
		return hw_offset;

	ll_gpio_toggle(hw_offset);

	return 0;
}

/**
 * @brief		Set the output level of a GPIO pin
 *
 * @param[in]	pin		GPIO pin descriptor
 * @param[in]	value	output level
 *
 * @return		error code
 */
static int mik32_gpio_pin_set(struct gpio_pin *pin, int value)
{
	int hw_offset = mik32_gpio_pin_get_hw_offset(pin);

	if (hw_offset < 0)
		return hw_offset;

	ll_gpio_write(hw_offset, !!value);

	return 0;
}

/**
 * @brief		Read the level of a GPIO pin
 *
 * @param[in]	pin	GPIO pin descriptor
 *
 * @retval		<0	error code
 * @retval		>=0	the level of a GPIO pin
 */
static int mik32_gpio_pin_get(struct gpio_pin *pin)
{
	int hw_offset = mik32_gpio_pin_get_hw_offset(pin);

	if (hw_offset < 0)
		return hw_offset;

	return ll_gpio_read(hw_offset);
}

/**
 * @brief		Configure a GPIO pin
 *
 * @param[in,out]	pin		GPIO pin descriptor
 *
 * @return		error code
 */
static int mik32_gpio_pin_config(struct gpio_pin *pin)
{
	int hw_offset = mik32_gpio_pin_get_hw_offset(pin);

	if (hw_offset < 0)
		return hw_offset;

	ll_gpio_set_mode(hw_offset, PIN_AS_GPIO);

	if (pin->flags & GPIO_CORE_FLAG_DIR_OUT)
		ll_gpio_set_output(hw_offset);
	else
		ll_gpio_set_input(hw_offset);

	if (pin->flags & GPIO_CORE_FLAG_PULL_UP)
		ll_gpio_set_pull(hw_offset, PIN_PULL_UP);
	else if (pin->flags & GPIO_CORE_FLAG_PULL_DOWN)
		ll_gpio_set_pull(hw_offset, PIN_PULL_DOWN);
	else
		ll_gpio_set_pull(hw_offset, PIN_PULL_NONE);

	ll_gpio_set_ds(hw_offset, PIN_DS_2);

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
