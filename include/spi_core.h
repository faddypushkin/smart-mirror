/**
 * @file
 * @brief	
 */

#ifndef SPI_CORE_H_
#define SPI_CORE_H_

#include <stddef.h>
#include <stdint.h>

void spi_write_buf(uint8_t *buf, size_t size);

void spi_init(void);

#endif /* SPI_CORE_H_ */
