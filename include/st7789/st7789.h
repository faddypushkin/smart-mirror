/**
 * @file
 * @brief	
 */

#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>

#include <st7789/fronts.h>
#include <spi_core.h>
#include <gpio_core.h>

struct st7789 {
	struct spi_device	dev;
	struct gpio_pin		*res,
						*dc,
						*bl;
};

void st7789_draw_pixel(struct st7789 *display, int16_t x, int16_t y, uint16_t color);

void st7789_draw_char(struct st7789 *display, uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch);

void st7789_print(struct st7789 *display, uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str);

void st7789_fill_screen(struct st7789 *display, uint16_t color);

int st7789_init(struct st7789 *display);

#endif /* ST7789_H_ */
