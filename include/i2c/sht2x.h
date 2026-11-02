/**
 * @file
 * @brief	API
 */

#ifndef I2C_SHT2X_H_
#define I2C_SHT2X_H_

#include <i2c/i2c_core.h>

struct sht2x_temperature {
	short	integer;
	short	decimal;
};

struct sht2x_humidity {
	short	integer;
	short	decimal;
};

struct sht2x {
	struct i2c_client;
	struct sht2x_temperature	temp;
	struct sht2x_humidity		hum;
};

struct sht2x_temperature *sht2x_get_temperature(struct sht2x *dev);

struct sht2x_humidity *sht2x_get_humidity(struct sht2x *dev);

int sht2x_init(struct sht2x *dev);

#endif /* I2C_SHT2X_H_ */
