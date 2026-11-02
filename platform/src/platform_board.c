/**
 * @file
 * @brief	Platform-specific board interface
 */

#include <mik32_memory_map.h>

#include <board.h>
#include <compiler_attributes.h>
#include <arch_gpio.h>
#include <arch_uart.h>

static struct arch_gpio_chip gpio0 = { .gpio = GPIO_0 };
static struct gpio_pin led = { .chip = &gpio0 };

static struct arch_uart_port uart0 = { .nr = 0, .baud = 115200 };

void board_run(void)
{
	static int step;
	struct uart_port *port = uart_core_get_default_port();
	char str[] = "Toggle!\r\n";

	++step;
	if (step < 10000)
		return;

	step = 0;
	gpio_core_pin_toggle(&led);
	uart_core_write(port, str, sizeof(str));
	uart_core_printf("step now = %d\r\n", step);
}

bool board_init(void)
{
	char str[] = "Hello, world!\r\n";

	arch_gpio_init(&gpio0);
	gpio_core_pin_config(&led, 9, GPIO_CORE_FLAG_DIR_OUT);

	arch_uart_init(&uart0);
	uart_core_write(&uart0, str, sizeof(str));

	return true;
}
