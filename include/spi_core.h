/**
 * @file
 * @brief	
 */

#ifndef SPI_CORE_H_
#define SPI_CORE_H_

#include <stddef.h>
#include <stdint.h>

#include <gpio_core.h>

struct spi_device;

/**  */
struct spi_controller_ops {
	/**  */
	int (*master_write)(struct spi_device *dev, const void *buf, size_t size);
	/**  */
	int (*master_read)(struct spi_device *dev, void *buf, size_t size);
};

/**  */
struct spi_controller {
	/**  */
	uint32_t						nr;
	/**  */
	const struct spi_controller_ops	*ops;
	/**  */
	void							*private_data;
};

/**  */
struct spi_device {
	/**  */
	struct spi_controller	*controller;
	/**  */
	struct gpio_pin			*cs;
	/**  */
	void					*private_data;
};

int spi_core_master_write(struct spi_device *dev, const void *buf, size_t size);

int spi_core_master_read(struct spi_device *dev, void *buf, size_t size);

int spi_core_init(struct spi_controller *controller);

#endif /* SPI_CORE_H_ */
