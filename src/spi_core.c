/**
 * @file
 * @brief	SPI core interface
 */

#include <errno.h>

#include <spi_core.h>

int spi_core_master_write(struct spi_device *dev, const void *buf, size_t size)
{
	if (!dev || !dev->controller)
		return -EINVAL;

	if (!dev->controller->ops->master_write)
		return -EOPNOTSUPP;
	return dev->controller->ops->master_write(dev, buf, size);
}

int spi_core_master_read(struct spi_device *dev, void *buf, size_t size)
{
	if (!dev || !dev->controller)
		return -EINVAL;

	if (!dev->controller->ops->master_read)
		return -EOPNOTSUPP;
	return dev->controller->ops->master_read(dev, buf, size);
}

int spi_core_init(struct spi_controller *controller)
{
	if (!controller || !controller->ops)
		return -EINVAL;

	return 0;
}
