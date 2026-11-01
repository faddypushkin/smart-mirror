/**
 * @file
 * @brief	UART core interface
 */

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>

#include <uart_core.h>

/* Size of the stack buffer used to format uart_core_printf() output. */
#define UART_CORE_PRINTF_BUF_LEN	100

/**
 * @brief		Default UART port
 *
 * The first UART port successfully initialized by uart_core_init()
 * becomes the default port. Subsequent ports do not replace it.
 */
static struct uart_port *default_port;

int uart_core_put_char(struct uart_port *port, char ch)
{
	if (!port || !port->ops)
		return -EINVAL;

	if (!port->ops->put_char)
		return -EOPNOTSUPP;
	return port->ops->put_char(port, ch);
}

int uart_core_write(struct uart_port *port, const void *buf, size_t size)
{
	if (!port || !port->ops)
		return -EINVAL;

	if (size == 0)
		return size;
	else if (size > INT_MAX)
		return -EOVERFLOW;
	if (buf == NULL)
		return -EINVAL;

	if (!port->ops->write)
		return -EOPNOTSUPP;
	return port->ops->write(port, buf, size);
}

int uart_core_printf(const char *format, ...)
{
	char buf[UART_CORE_PRINTF_BUF_LEN];
	va_list ap;
	int len;

	if (!default_port)
		return -ENODEV;

	va_start(ap, format);

	len = vsnprintf(buf, sizeof(buf), format, ap);
	if (len > 0) {
		len = len < sizeof(buf) ? len : sizeof(buf) - 1;
		if (uart_core_write(default_port, buf, len) < 0)
			return -EIO;
	}

	va_end(ap);

	return len;
}

struct uart_port *uart_core_get_default_port(void)
{
	return default_port;
}

int uart_core_init(struct uart_port *port)
{
	if (!port || !port->ops)
		return -EINVAL;

	if (!default_port)
		default_port = port;

	return 0;
}
