/**
 * @file
 * @brief	Platform-specific board interface
 */

#include <mik32_memory_map.h>

#include <board.h>
#include <compiler_attributes.h>
#include <arch_gpio.h>
#include <arch_uart.h>
#include <arch_i2c.h>
#include <arch_spi.h>

#include <sensors/sht2x.h>

#include <platform_display.h>

static struct arch_gpio_chip gpio0 = { .gpio = GPIO_0 };
static struct arch_gpio_chip gpio1 = { .gpio = GPIO_1 };

static struct gpio_pin led = { .chip = &gpio0 };

static struct gpio_pin display_res = { .chip = &gpio1 };
static struct gpio_pin display_dc  = { .chip = &gpio1 };
static struct gpio_pin display_cs  = { .chip = &gpio1 };
static struct gpio_pin display_bl  = { .chip = &gpio1 };

static struct arch_uart_port uart0 = { .nr = 0, .baud = 115200 };

static struct arch_i2c_adapter i2c1 = { .nr = 1 };
static struct sensors_sht2x sht = { .client.adap = &i2c1 };

struct arch_spi_controller spi0 = { .nr = 0 };
struct st7789 display = { 	.dev = {
								.controller = &spi0,
								.cs = &display_cs,
							},
							.res	= &display_res,
							.dc	= &display_dc,
							.bl	= &display_bl };

void board_run(void)
{
	static int step;

	++step;
	if (step < 10000)
		return;

	platform_display_run(&display, &sht);

	step = 0;
	gpio_core_pin_toggle(&led);
}

bool board_init(void)
{
	arch_gpio_init(&gpio0);
	gpio_core_pin_config(&led, 9, GPIO_CORE_FLAG_DIR_OUT);

	arch_gpio_init(&gpio1);
	gpio_core_pin_config(&display_res, 11, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&display_dc, 10, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&display_cs, 9, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&display_bl, 8, GPIO_CORE_FLAG_DIR_OUT);

	arch_uart_init(&uart0);

	arch_i2c_init(&i2c1);
	sensors_sht2x_init(&sht, SENSORS_SHT2X_HOLD);

	arch_spi_init(&spi0);
	platform_display_init(&display);

	return true;
}
