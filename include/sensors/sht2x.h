/**
 * @file
 * @brief	API
 */

#ifndef SENSORS_SHT2X_H_
#define SENSORS_SHT2X_H_

#include <stdint.h>

#include <i2c_core.h>

enum SENSORS_STH2X_MODE {
	SENSORS_SHT2X_NO_HOLD,
	SENSORS_SHT2X_HOLD,
};

struct sensors_sht2x_temp {
	int8_t	integer;
	uint8_t	decimal;
};

struct sensors_sht2x_rhum {
	uint8_t	integer;
	uint8_t	decimal;
};

struct sensors_sht2x {
	char					*name;
	enum SENSORS_STH2X_MODE	mode;
	struct i2c_client		client;
};

int sensors_sht2x_get_temp(struct sensors_sht2x *dev,
						   struct sensors_sht2x_temp *temp);

int sensors_sht2x_get_rhum(struct sensors_sht2x *dev,
						   struct sensors_sht2x_rhum *rhum);

int sensors_sht2x_init(struct sensors_sht2x *dev, enum SENSORS_STH2X_MODE mode);

#endif /* SENSORS_SHT2X_H_ */
