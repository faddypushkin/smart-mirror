/**
 * @file
 * @brief	Arch-specific SPI API
 */

#ifndef ARCH_SPI_H_
#define ARCH_SPI_H_

#include <mik32_hal_spi.h>

#include <spi_core.h>

/** Arch-specific I2C port descriptor */
struct arch_spi_controller {
	/**  */
	struct spi_controller;
	/**  */
	SPI_HandleTypeDef		spi;
};

/**
 * @brief		Initialize a SPI MIK32
 *
 * @param[in]	controller	Arch-specific descriptor
 *
 * @return		error code
 */
int arch_spi_init(struct arch_spi_controller *controller);

#endif /* ARCH_SPI_H_ */
