/**
 * @file
 * @brief	
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

void display_print(uint16_t x, uint16_t y, const char *text);

void display_init(void);

#endif /* DISPLAY_H_ */
