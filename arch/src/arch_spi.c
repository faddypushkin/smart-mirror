/**
 * @file
 * @brief	Arch-specific SPI interface
 */

#include <errno.h>
#include <string.h>

#include <arch_spi.h>
#include <gpio_core.h>

static int mik32_spi_exchange(SPI_HandleTypeDef *controller,
							  uint8_t *write_buf, uint8_t *read_buf, size_t size)
{
	uint32_t txallowed = 1;
	uint32_t timeout_counter = 0;
	int ret;

	controller->ErrorCode	= HAL_SPI_ERROR_NONE;
	controller->pRxBuffPtr	= read_buf;
	controller->RxCount		= size;
	controller->pTxBuffPtr	= write_buf;
	controller->TxCount		= size;

	controller->Instance->TX_THR = 1;

	if (!(controller->Instance->ENABLE & SPI_ENABLE_M))
		__HAL_SPI_ENABLE(controller);

	while ((controller->TxCount > 0) || (controller->RxCount > 0)) {
		if ((controller->Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) &&
			(controller->TxCount > 0) && (txallowed == 1)) {
			controller->Instance->TXDATA = *(controller->pTxBuffPtr);
			controller->pTxBuffPtr++;
			controller->TxCount--;

			txallowed = 0;
		}

		if ((controller->Instance->INT_STATUS & SPI_INT_STATUS_RX_FIFO_NOT_EMPTY_M) &&
			(controller->RxCount > 0)) {
			*(controller->pRxBuffPtr) = controller->Instance->RXDATA;
			controller->pRxBuffPtr++;
			controller->RxCount--;

			txallowed = 1;
		}

		if (++timeout_counter > SPI_TIMEOUT_DEFAULT) {
			ret = -ETIMEDOUT;
			goto error;
		}
	}
	return 0;

error:
	if (!(controller->Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
		__HAL_SPI_DISABLE(controller);
		controller->Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M |
										SPI_ENABLE_CLEAR_RX_FIFO_M;
	}

	volatile uint32_t unused = controller->Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
	(void) unused;

	return ret;
}

static int mik32_spi_master_write(struct spi_device *dev, const void *buf, size_t size)
{
	SPI_HandleTypeDef *controller = dev->controller->private_data;
	uint8_t *data = (uint8_t *) buf;
	uint8_t input_buf[size];
	int ret = 0;

	gpio_core_pin_set(dev->cs, 0);
	if (mik32_spi_exchange(controller, data, input_buf, size))
		ret = -EIO;
	gpio_core_pin_set(dev->cs, 1);

	return ret;
}

static int mik32_spi_master_read(struct spi_device *dev, void *buf, size_t size)
{
	SPI_HandleTypeDef *controller = dev->controller->private_data;
	uint8_t output_buf[size];
	int ret = 0;

	memset(output_buf, 0xff, sizeof(output_buf));

	gpio_core_pin_set(dev->cs, 0);
	if (mik32_spi_exchange(controller, output_buf, buf, size))
		ret = -EIO;
	gpio_core_pin_set(dev->cs, 1);

	return ret;
}

static const struct spi_controller_ops mik32_spi_controller_ops = {
	.master_write		= mik32_spi_master_write,
	.master_read		= mik32_spi_master_read,
};

int arch_spi_init(struct arch_spi_controller *controller)
{
	if (!controller)
		return -EINVAL;

	if (controller->nr > 1)
		return -ENODEV;

	controller->spi.Instance			= (controller->nr == 0) ? SPI_0
																: SPI_1;
	controller->spi.Init.SPI_Mode		= HAL_SPI_MODE_MASTER;
	controller->spi.Init.CLKPhase		= SPI_PHASE_OFF;
	controller->spi.Init.CLKPolarity	= SPI_POLARITY_LOW;
	controller->spi.Init.ThresholdTX	= SPI_THRESHOLD_DEFAULT;
	controller->spi.Init.BaudRateDiv	= SPI_BAUDRATE_DIV4;
	controller->spi.Init.Decoder		= SPI_DECODER_NONE;
	controller->spi.Init.ManualCS		= SPI_MANUALCS_ON;
	controller->spi.Init.ChipSelect		= SPI_CS_NONE;
	
	if (HAL_SPI_Init(&controller->spi) != HAL_OK)
		return -EIO;

	controller->ops				= &mik32_spi_controller_ops;
	controller->private_data	= &controller->spi;

	return spi_core_init(controller);
}
