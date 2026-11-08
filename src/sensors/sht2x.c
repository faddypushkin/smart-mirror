/**
 * @file
 * @brief	interface
 */

#include <errno.h>
#include <stdint.h>

#include <sensors/sht2x.h>

#define SHT2x_I2C_ADDR			0x40
#define SHT2x_READ_TEMP_HOLD	0xe3
#define	SHT2x_READ_RH_HOLD		0xe5
#define SHT2x_READ_TEMP_NOHOLD	0xf3
#define SHT2x_READ_RH_NOHOLD	0xf5
#define	SHT2x_WRITE_REG			0xe6
#define SHT2x_READ_REG			0xe7
#define SHT2x_SOFT_RESET		0xfe

static uint16_t sensors_sht2x_get_raw_data(struct sensors_sht2x *dev,
										   uint8_t cmd)
{
	uint8_t val[3] = { 0 };

	i2c_core_master_write(&dev->client, &cmd, sizeof(cmd));
	i2c_core_master_read(&dev->client, val, sizeof(val));

	return val[0] << 8 | val[1];
}

int sensors_sht2x_get_temp(struct sensors_sht2x *dev,
						   struct sensors_sht2x_temp *temp)
{
	uint8_t cmd = dev->mode ? SHT2x_READ_TEMP_HOLD : SHT2x_READ_TEMP_NOHOLD;
	int raw_data = ((sensors_sht2x_get_raw_data(dev, cmd) * 17572) >> 16) - 4685;

	temp->integer = raw_data / 100;
	temp->decimal = raw_data % 100;

	return 0;
}

int sensors_sht2x_get_rhum(struct sensors_sht2x *dev,
						   struct sensors_sht2x_rhum *rhum)
{
	uint8_t cmd = dev->mode ? SHT2x_READ_RH_HOLD : SHT2x_READ_RH_NOHOLD;
	int raw_data = ((sensors_sht2x_get_raw_data(dev, cmd) * 12500) >> 16) - 600;

	rhum->integer = raw_data / 100;
	rhum->decimal = raw_data % 100;

	return 0;
}

int sensors_sht2x_init(struct sensors_sht2x *dev, enum SENSORS_STH2X_MODE mode)
{
	if (!dev)
		return -EINVAL;

	if (!dev->client.adap)
		return -ENODEV;

	dev->client.addr	= SHT2x_I2C_ADDR;
	dev->mode			= mode;

	return 0;
}
