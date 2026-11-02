/**
 * @file
 * @brief	interface
 */

#include <errno.h>
#include <stdint.h>

#include <i2c/sht2x.h>

#define SHT2x_I2C_ADDR			0x40
#define SHT2x_HOLD_MASTER		1
#define SHT2x_READ_TEMP_HOLD	0xe3
#define	SHT2x_READ_RH_HOLD		0xe5
#define SHT2x_READ_TEMP_NOHOLD	0xf3
#define SHT2x_READ_RH_NOHOLD	0xf5
#define	SHT2x_WRITE_REG			0xe6
#define SHT2x_READ_REG			0xe7
#define SHT2x_SOFT_RESET		0xfe
#define SHT2x_TIMEOUT			1000

static uint16_t sht2x_get_raw_data(struct sht2x *dev, uint8_t cmd)
{
	uint8_t val[3] = { 0 };

	i2c_core_master_write(dev, &cmd, sizeof(cmd));
	i2c_core_master_read(dev, val, sizeof(val));

	return val[0] << 8 | val[1];
}

struct sht2x_temperature *sht2x_get_temperature(struct sht2x *dev)
{
	int raw_data = ((sht2x_get_raw_data(dev, SHT2x_READ_TEMP_HOLD) * 17572) >> 16) - 4685;

	dev->temp.integer = raw_data / 100;
	dev->temp.decimal = raw_data % 100;

	return &dev->temp;
}

struct sht2x_humidity *sht2x_get_humidity(struct sht2x *dev)
{
	int raw_data = ((sht2x_get_raw_data(dev, SHT2x_READ_RH_HOLD) * 12500) >> 16) - 600;

	dev->hum.integer = raw_data / 100;
	dev->hum.decimal = raw_data % 100;

	return &dev->hum;
}

int sht2x_init(struct sht2x *dev)
{
	if (!dev)
		return -EINVAL;

	if (!dev->adap)
		return -ENODEV;

	dev->addr = SHT2x_I2C_ADDR;
	dev->temp.integer = 0;
	dev->temp.decimal = 0;

	return 0;
}
