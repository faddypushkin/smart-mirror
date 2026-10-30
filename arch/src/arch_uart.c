/**
 * @file
 * @brief	Arch-specific UART interface
 */

#include <errno.h>

#include <arch_uart.h>

/**
 * @brief		Transmit one character through a MIK32 USART
 *
 * @param[in]	port	UART port descriptor
 * @param[in]	ch		character to transmit
 *
 * @return		error code
 */
static int mik32_uart_put_char(struct uart_port *port, char ch)
{
	USART_HandleTypeDef *uart = port->private_data;

	if (!uart)
		return -EINVAL;

	HAL_USART_WriteByte(uart, ch);
	while (!HAL_USART_TXC_ReadFlag(uart))
		;

	return 0;
}

/**
 * @brief		Transmit a buffer through a MIK32 USART
 *
 * @param[in]	port	UART port descriptor
 * @param[in]	buf		buffer to transmit
 * @param[in]	size	number of bytes to transmit
 *
 * @retval		<0		error code
 * @retval		>=0		number of input bytes accepted
 */
static int mik32_uart_write(struct uart_port *port, const void *buf, size_t size)
{
	const char *data = buf;
	size_t i;
	int ret;

	for (i = 0; i < size; ++i) {
		ret = mik32_uart_put_char(port, *data++);
		if (ret < 0)
			return ret;
	}

	return size;
}

static const struct uart_port_ops mik32_uart_port_ops = {
	.put_char	= mik32_uart_put_char,
	.write		= mik32_uart_write,
};

int arch_uart_init(struct arch_uart_port *port)
{
	if (!port)
		return -EINVAL;

	if (port->nr > 1)
		return -ENODEV;
	if (port->baud == 0)
		return -EINVAL;

	port->uart.Instance				= (port->nr == 0) ? UART_0 : UART_1;
	port->uart.transmitting			= Enable;
	port->uart.receiving			= Enable;
	port->uart.frame				= Frame_8bit;
	port->uart.parity_bit			= Disable;
	port->uart.parity_bit_inversion	= Disable;
	port->uart.bit_direction		= LSB_First;
	port->uart.data_inversion		= Disable;
	port->uart.tx_inversion			= Disable;
	port->uart.rx_inversion			= Disable;
	port->uart.swap					= Disable;
	port->uart.lbm					= Disable;
	port->uart.stop_bit				= StopBit_1;
	port->uart.mode					= Asynchronous_Mode;
	port->uart.xck_mode				= XCK_Mode3;
	port->uart.last_byte_clock		= Disable;
	port->uart.overwrite			= Disable;
	port->uart.rts_mode				= AlwaysEnable_mode;
	port->uart.dma_tx_request		= Disable;
	port->uart.dma_rx_request		= Disable;
	port->uart.channel_mode			= Duplex_Mode;
	port->uart.tx_break_mode		= Disable;
	port->uart.Interrupt.ctsie		= Disable;
	port->uart.Interrupt.eie		= Disable;
	port->uart.Interrupt.idleie		= Disable;
	port->uart.Interrupt.lbdie		= Disable;
	port->uart.Interrupt.peie		= Disable;
	port->uart.Interrupt.rxneie		= Disable;
	port->uart.Interrupt.tcie		= Disable;
	port->uart.Interrupt.txeie		= Disable;
	port->uart.Modem.rts			= Disable;
	port->uart.Modem.cts			= Disable;
	port->uart.Modem.dtr			= Disable;
	port->uart.Modem.dcd			= Disable;
	port->uart.Modem.dsr			= Disable;
	port->uart.Modem.ri				= Disable;
	port->uart.Modem.ddis			= Disable;
	port->uart.baudrate				= port->baud;
	if (HAL_USART_Init(&port->uart) != HAL_OK)
		return -EIO;

	port->ops = &mik32_uart_port_ops;
	port->private_data = &port->uart;

	return uart_core_init(port);
}
