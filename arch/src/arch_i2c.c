/**
 * @file
 * @brief	Arch-specific i2C interface
 */

#include <errno.h>

#include <arch_i2c.h>

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
static int mik32_i2c_master_write(struct i2c_client *client, const void *buf, size_t size)
{
	I2C_HandleTypeDef *i2c = client->adap->private_data;
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Master_Transmit(i2c, client->addr, buf, size, I2C_TIMEOUT_DEFAULT);
	if (ret != HAL_OK)
		HAL_I2C_Reset(i2c);
	if (i2c->Init.AutoEnd == I2C_AUTOEND_DISABLE)
		i2c->Instance->CR2 |= I2C_CR2_STOP_M;

	return ret == HAL_OK ? size : -EIO;
}

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
static int mik32_i2c_master_read(struct i2c_client *client, void *buf, size_t size)
{
	I2C_HandleTypeDef *i2c = client->adap->private_data;
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Master_Receive(i2c, client->addr, buf, size, I2C_TIMEOUT_DEFAULT);
	if (ret != HAL_OK)
		HAL_I2C_Reset(i2c);
	if (i2c->Init.AutoEnd == I2C_AUTOEND_DISABLE)
		i2c->Instance->CR2 |= I2C_CR2_STOP_M;

	return ret == HAL_OK ? size : -EIO;
}

static const struct i2c_adapter_ops mik32_i2c_adapter_ops = {
	.master_write		= mik32_i2c_master_write,
	.master_read		= mik32_i2c_master_read,
};

int arch_i2c_init(struct arch_i2c_adapter *adap)
{
	if (!adap)
		return -EINVAL;

	if (adap->nr > 1)
		return -ENODEV;

	adap->i2c.Instance = (adap->nr == 0) ? I2C_0 : I2C_1;

	adap->i2c.Init.Mode = HAL_I2C_MODE_MASTER;

	adap->i2c.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
	adap->i2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
	adap->i2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;

	adap->i2c.Clock.PRESC = 1;
	adap->i2c.Clock.SCLDEL = 1;
	adap->i2c.Clock.SDADEL = 1;
	adap->i2c.Clock.SCLH = 1;
	adap->i2c.Clock.SCLL = 1;

	if (HAL_I2C_Init(&adap->i2c) != HAL_OK)
		return -EIO;

	adap->ops = &mik32_i2c_adapter_ops;
	adap->private_data = &adap->i2c;

	return i2c_core_init(adap);
}
