/**
 * @file
 * @brief	I2C core API
 */

#ifndef I2C_CORE_H_
#define I2C_CORE_H_

#include <stddef.h>
#include <stdint.h>

struct i2c_adapter;
struct i2c_client;

/**  */
struct i2c_adapter_ops {
	/**  */
	int (*master_write)(struct i2c_client *client, const void *buf, size_t size);
	/**  */
	int (*master_read)(struct i2c_client *client, void *buf, size_t size);
};

/**  */
struct i2c_adapter {
	/**  */
	uint32_t						nr;
	/**  */
	uint32_t						speed;
	/**  */
	const struct i2c_adapter_ops	*ops;
	/**  */
	void							*private_data;
};

/**  */
struct i2c_client {
	/**  */
	struct i2c_adapter	*adap;
	/**  */
	uint16_t			addr;
	/**  */
	void				*private_data;
};

/**
 * @brief		
 *
 * @param[in]	
 * @param[in]	
 *
 * @return		error code
 */
int i2c_core_master_write(struct i2c_client *client, const void *buf, size_t size);

/**
 * @brief		
 *
 * @param[in]	
 * @param[in]	
 * @param[in]	
 *
 * @retval		
 * @retval		
 */
int i2c_core_master_read(struct i2c_client *client, void *buf, size_t size);

/**
 * @brief
 *
 * @param[in]
 *
 * @return
 */
int i2c_core_init(struct i2c_adapter *adap);

#endif /* I2C_CORE_H_ */
