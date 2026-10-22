/**
 * @file
 * @brief	Platform-specific board interface
 */

#include <mik32_memory_map.h>

#include <board.h>
#include <arch_gpio.h>

static struct arch_gpio_chip gpio0 = { .gpio = GPIO_0 };
static struct gpio_pin led = { .chip = &gpio0 };

void board_run(void)
{
	static int step;

	++step;
	if (step < 10000)
		return;

	step = 0;
	gpio_core_pin_toggle(&led);
}

bool board_init(void)
{
	arch_gpio_init(&gpio0);
	gpio_core_pin_config(&led, 9, GPIO_CORE_FlAG_DIR_OUT);

	return true;
}
