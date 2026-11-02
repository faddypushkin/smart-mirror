/**
 * @file
 * @brief	I2C core interface
 */

#include <errno.h>

#include <i2c_core.h>

int i2c_core_master_write(struct i2c_client *client, const void *buf, size_t size)
{
	if (!client || !client->adap)
		return -EINVAL;

	if (!client->adap->ops->master_write)
		return -EOPNOTSUPP;
	return client->adap->ops->master_write(client, buf, size);
}

int i2c_core_master_read(struct i2c_client *client, void *buf, size_t size)
{
	if (!client || !client->adap)
		return -EINVAL;

	if (!client->adap->ops->master_read)
		return -EOPNOTSUPP;
	return client->adap->ops->master_read(client, buf, size);
}

int i2c_core_init(struct i2c_adapter *adap)
{
	if (!adap || !adap->ops)
		return -EINVAL;

	return 0;
}
