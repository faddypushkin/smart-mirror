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

#include <i2c/sht2x.h>

static struct arch_gpio_chip gpio0 = { .gpio = GPIO_0 };
static struct gpio_pin led = { .chip = &gpio0 };

static struct arch_uart_port uart0 = { .nr = 0, .baud = 115200 };

static struct arch_i2c_adapter i2c1 = { .nr = 1 };
static struct sht2x sht = { .adap = &i2c1 };

void board_run(void)
{
	static int step;
	struct sht2x_temperature *temp;
	struct sht2x_humidity *hum;

	++step;
	if (step < 10000)
		return;

	temp = sht2x_get_temperature(&sht);
	hum  = sht2x_get_humidity(&sht);

	step = 0;
	gpio_core_pin_toggle(&led);
	uart_core_printf("Temperature = %d.%d°C\n", temp->integer, temp->decimal);
	uart_core_printf("Humidity    = %d.%d%%\n", hum->integer, hum->decimal);
}

bool board_init(void)
{
	char str[] = "Hello, world!\r\n";

	arch_gpio_init(&gpio0);
	gpio_core_pin_config(&led, 9, GPIO_CORE_FLAG_DIR_OUT);

	arch_uart_init(&uart0);
	uart_core_write(&uart0, str, sizeof(str));

	arch_i2c_init(&i2c1);
	sht2x_init(&sht);

	return true;
}
