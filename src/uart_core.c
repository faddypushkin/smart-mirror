/**
 * @file
 * @brief	UART core interface
 */

#include <errno.h>
#include <limits.h>

#include <uart_core.h>

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
