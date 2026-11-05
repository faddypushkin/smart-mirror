#include <spi_core.h>
#include <uart_core.h>

#include <mik32_hal_spi.h>

SPI_HandleTypeDef hspi0;

void spi_write_buf(uint8_t *buf, size_t size)
{
	uint8_t input_buf[size];

	/* Передача и прием данных */
	HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi0, buf, input_buf,
							size, SPI_TIMEOUT_DEFAULT);
	if (SPI_Status != HAL_OK) {
		uart_core_printf("SPI_Error %d, OVR %d, MODF %d\n",
				 SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR,
				 hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
		HAL_SPI_ClearError(&hspi0);
	}
}

void spi_init(void)
{
	hspi0.Instance = SPI_0;

	/* Режим SPI */
	hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;
	
	/* Настройки */
	hspi0.Init.CLKPhase = SPI_PHASE_OFF;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;
	
	/* Настройки для ведущего */
	hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
	hspi0.Init.Decoder = SPI_DECODER_NONE;
	hspi0.Init.ManualCS = SPI_MANUALCS_ON;
	hspi0.Init.ChipSelect = SPI_CS_NONE;
	
	if (HAL_SPI_Init(&hspi0) != HAL_OK)
	    uart_core_printf("SPI_Init_Error\n");
}
