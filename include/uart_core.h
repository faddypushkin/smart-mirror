/**
 * @file
 * @brief	UART core API
 */

#ifndef UART_CORE_H_
#define UART_CORE_H_

#include <stddef.h>
#include <stdint.h>

struct uart_port;

/** UART port operations */
struct uart_port_ops {
	/** Transmit one character */
	int (*put_char)(struct uart_port *port, char ch);
	/** Transmit a buffer */
	int (*write)(struct uart_port *port, const void *buf, size_t size);
};

/** UART port descriptor */
struct uart_port {
	/** UART port number */
	uint32_t					nr;
	/** UART baud rate */
	uint32_t					baud;
	/** UART port operations */
	const struct uart_port_ops	*ops;
	/** UART port-specific data */
	void						*private_data;
};

/**
 * @brief		Transmit one character
 *
 * @param[in]	port	UART port descriptor
 * @param[in]	ch		character to transmit
 *
 * @return		error code
 */
int uart_core_put_char(struct uart_port *port, char ch);

/**
 * @brief		Transmit a buffer
 *
 * @param[in]	port	UART port descriptor
 * @param[in]	buf		buffer to transmit
 * @param[in]	size	number of bytes to transmit
 *
 * @retval		<0	error code
 * @retval		>=0	number of input bytes accepted
 */
int uart_core_write(struct uart_port *port, const void *buf, size_t size);

/**
 * @brief		Get the default UART port
 *
 * @return		default UART port descriptor, or NULL if no port is set
 */
struct uart_port *uart_core_get_default_port(void);

/**
 * @brief		Initialize a UART port
 *
 * @param[in]	port	UART port descriptor
 *
 * @return		error code
 */
int uart_core_init(struct uart_port *port);

#endif /* UART_CORE_H_ */
