/**
 * @file
 * @brief	Arch-specific UART API
 */

#ifndef ARCH_UART_H_
#define ARCH_UART_H_

#include <mik32_hal_usart.h>

#include <uart_core.h>

/** Arch-specific UART port descriptor */
struct arch_uart_port {
	/** UART port descriptor */
	struct uart_port;
	/** MIK32 USART HAL handle */
	USART_HandleTypeDef		uart;
};

/**
 * @brief		Initialize a MIK32 UART port
 *
 * @param[in]	chip	Arch-specific UART port descriptor
 *
 * @return		error code
 */
int arch_uart_init(struct arch_uart_port *port);

#endif /* ARCH_UART_H_ */
