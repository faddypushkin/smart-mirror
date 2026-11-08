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
	int ret, i, len = 0;
	va_list ap;

	if (!default_port)
		return -ENODEV;

	va_start(ap, format);
	ret = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	if (ret < 0)
		return ret;
	if (ret > sizeof(buf))
		return -ENOSPC;

	for (i = 0; i < ret; ++i) {
		if (buf[i] == '\n') {
			if (uart_core_put_char(default_port, '\r') < 0)
				return -EIO;
			++len;
		}

		if (uart_core_put_char(default_port, buf[i]) < 0)
			return -EIO;
		++len;
	}

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
