/**
 * @file
 * @brief	Arch-specific I2C API
 */

#ifndef ARCH_I2C_H_
#define ARCH_I2C_H_

#include <mik32_hal_i2c.h>

#include <i2c_core.h>

/** Arch-specific I2C port descriptor */
struct arch_i2c_adapter {
	/**  */
	struct i2c_adapter;
	/**  */
	I2C_HandleTypeDef		i2c;
};

/**
 * @brief		Initialize a MIK32
 *
 * @param[in]	chip	Arch-specific descriptor
 *
 * @return		error code
 */
int arch_i2c_init(struct arch_i2c_adapter *adap);

#endif /* ARCH_I2C_H_ */
