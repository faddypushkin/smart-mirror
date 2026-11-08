/**
 * @file
 * @brief	
 */

#include <errno.h>

#include <st7789/st7789.h>
#include <st7789/st7789-reg.h>

#include <mik32_hal.h>

static void st7789_write_command(struct st7789 *display, uint8_t command)
{
	
	gpio_core_pin_set(display->dc, 0);
	spi_core_master_write(&display->dev, &command, sizeof(command));
}

static void st7789_write_byte(struct st7789 *display, uint8_t byte)
{
	gpio_core_pin_set(display->dc, 1);
	spi_core_master_write(&display->dev, &byte, sizeof(byte));
}

static void st7789_write_buf(struct st7789 *display, uint8_t *buf, size_t size)
{
	uint16_t chunk_size;

	gpio_core_pin_set(display->dc, 1);
	while (size > 0) {
		chunk_size = size > 65535 ? 65535 : size;
		spi_core_master_write(&display->dev, buf, chunk_size);
		buf += chunk_size;
		size -= chunk_size;
	}

}

static int st7789_set_rotation(struct st7789 *display, uint8_t mode)
{
	uint8_t byte = 0;

	if (mode == 0)
		byte = ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB;
	else if (mode == 1)
		byte = ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
	else if (mode == 2)
		byte = ST7789_MADCTL_RGB;
	else if (mode == 3)
		byte = ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
	else
		return -EINVAL;
	st7789_write_command(display, ST7789_MADCTL);
	st7789_write_byte(display, byte);

	return 0;
}

static void st7789_set_window(struct st7789 *display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    //gpio_core_pin_set(&cs, 0);
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
	
	/* Column Address set */
	st7789_write_command(display, ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		st7789_write_buf(display, data, sizeof(data));
	}

	/* Row Address set */
	st7789_write_command(display, ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		st7789_write_buf(display, data, sizeof(data));
	}
	/* Write to RAM */
	st7789_write_command(display, ST7789_RAMWR);
	//gpio_core_pin_set(&cs, 1);
}

void st7789_draw_pixel(struct st7789 *display, int16_t x, int16_t y, uint16_t color)
{
	uint8_t data [sizeof(color)];

	if ((x < 0) ||(x >= ST7789_WIDTH) || (y < 0) || (y >= ST7789_HEIGHT))
		return;
	
	data[0] = color >> 8;
	data[1] = color & 0xFF;

	st7789_set_window(display, x, y, x, y);
	st7789_write_buf(display, data, sizeof(data));
}

void st7789_draw_char(struct st7789 *display, uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch)
{
	uint32_t i, b, j;
	uint32_t X = x, Y = y;
	uint8_t xx, yy;

	if (x + Font->FontWidth > ST7789_WIDTH ||
		y + Font->FontHeight > ST7789_HEIGHT)
		return;

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {		
		if( ch < 127 ){			
			b = Font->data[(ch - 32) * Font->FontHeight + i];
		} else if( (uint8_t) ch > 191 ){
			// +96 это так как латинские символы и знаки в шрифтах занимают 96 позиций
			// и если в шрифте который содержит сперва латиницу и спец символы и потом 
			// только кирилицу то нужно добавлять 95 если шрифт 
			// содержит только кирилицу то +96 не нужно
			b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
		} else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
			// 160 эллемент ( символ Ё ) 
			b = Font->data[( 160 ) * Font->FontHeight + i];
		} else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
			// 161 эллемент  ( символ ё ) 
			b = Font->data[( 161 ) * Font->FontHeight + i];
		}
		//-------------------------------------------------------------------
				
		//----  Украинская раскладка ----------------------------------------------------
		else if( (uint8_t) ch == 170 ){	// 168 символ по ASCII - Є
			// 162 эллемент ( символ Є )
			b = Font->data[( 162 ) * Font->FontHeight + i];
		}
		else if( (uint8_t) ch == 175 ){	// 184 символ по ASCII - Ї
			// 163 эллемент  ( символ Ї )
			b = Font->data[( 163 ) * Font->FontHeight + i];
		}
		else if( (uint8_t) ch == 178 ){	// 168 символ по ASCII - І
			// 164 эллемент ( символ І )
			b = Font->data[( 164 ) * Font->FontHeight + i];
		}
		else if( (uint8_t) ch == 179 ){	// 184 символ по ASCII - і
			// 165 эллемент  ( символ і )
			b = Font->data[( 165 ) * Font->FontHeight + i];
		}
		else if( (uint8_t) ch == 186 ){	// 184 символ по ASCII - є
			// 166 эллемент  ( символ є )
			b = Font->data[( 166 ) * Font->FontHeight + i];
		}
		else if( (uint8_t) ch == 191 ){	// 168 символ по ASCII - ї
			// 167 эллемент ( символ ї )
			b = Font->data[( 167 ) * Font->FontHeight + i];
		}
		//-----------------------------------------------------------------------------
		else
			return;

		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000)
				for (yy = 0; yy < multiplier; yy++)
					for (xx = 0; xx < multiplier; xx++)
							st7789_draw_pixel(display, X+xx, Y+yy, TextColor);
			else if( TransparentBg )
				for (yy = 0; yy < multiplier; yy++)
					for (xx = 0; xx < multiplier; xx++)
							st7789_draw_pixel(display, X+xx, Y+yy, BgColor);
			X += multiplier;
		}
		X = x;
		Y += multiplier;
	}
}

void st7789_print(struct st7789 *display, uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str)
{
	unsigned char buff_char = '\0';
	uint16_t len = strlen(str);
	
	while (len--) {
		
		//---------------------------------------------------------------------
		// проверка на кириллицу UTF-8, если латиница то пропускаем if
		// Расширенные символы ASCII Win-1251 кириллица (код символа 128-255)
		// проверяем первый байт из двух ( так как UTF-8 ето два байта )
		// если он больше либо равен 0xC0 ( первый байт в кириллеце будет равен 0xD0 либо 0xD1 именно в алфавите )
		if ( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251
			
			// проверяем какой именно байт первый 0xD0 либо 0xD1---------------------------------------------
			switch ((uint8_t)*str) {
				case 0xD0: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }	// байт символов А...Я а...п  делаем здвиг на +48
					else if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }		// байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }		// байт символа Є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }		// байт символа І ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }		// байт символа Ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
				case 0xD1: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }	// байт символов п...я	елаем здвиг на +112
					else if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }		// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }		// байт символа є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }		// байт символа і ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }		// байт символа ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
			}
			//------------------------------------------------------------------------------------------------
			// уменьшаем еще переменную так как израсходывали 2 байта для кириллицы
			len--;
			
			st7789_draw_char(display, x, y, TextColor, BgColor, TransparentBg, Font, multiplier, buff_char);
		}
		//---------------------------------------------------------------------
		else{
			st7789_draw_char(display, x, y, TextColor, BgColor, TransparentBg, Font, multiplier, *str);
		}
		
		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}

void st7789_fill_screen(struct st7789 *display, uint16_t color)
{
	uint8_t data[ST7789_HEIGHT];
    uint16_t i;

	for (i = 0; i < sizeof(data); i += 2) {
		data[i] = color >> 8;
		data[i + 1] = color & 0xFF;
	}

	st7789_set_window(display, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	gpio_core_pin_set(display->dc, 1);
	for (i = 0; i < 2 * ST7789_WIDTH; i++)
		spi_core_master_write(&display->dev, data, sizeof(data));
}

int st7789_init(struct st7789 *display)
{
	gpio_core_pin_set(display->bl, 1);

	HAL_ProgramDelayMs(10);
	gpio_core_pin_set(display->res, 0);
    HAL_ProgramDelayMs(10);
    gpio_core_pin_set(display->res, 1);
    HAL_ProgramDelayMs(150);                          // БЫЛО: delay_ms(20)

	st7789_write_command(display, ST7789_SWRESET);          // ДОБАВИТЬ (опционально, но полезно)
	HAL_ProgramDelayMs(150);

	st7789_write_command(display, ST7789_SLPOUT);           // ДОБАВИТЬ — обязательно
	HAL_ProgramDelayMs(150);

	st7789_write_command(display, ST7789_COLMOD);				//	Set color mode
	st7789_write_byte(display, ST7789_COLOR_MODE_16bit);
	st7789_write_command(display, 0xB2);						//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		st7789_write_buf(display, data, sizeof(data));
	}
	st7789_set_rotation(display, ST7789_ROTATION);	//	MADCTL (Display Rotation)
	
	/* Internal LCD Voltage generator settings */
	st7789_write_command(display, 0XB7);				//	Gate Control
	st7789_write_byte(display, 0x35);			//	Default value
	st7789_write_command(display, 0xBB);				//	VCOM setting
	st7789_write_byte(display, 0x19);			//	0.725v (default 0.75v for 0x20)
	st7789_write_command(display, 0xC0);				//	LCMCTRL	
	st7789_write_byte (display, 0x2C);			//	Default value
	st7789_write_command (display, 0xC2);				//	VDV and VRH command Enable
	st7789_write_byte (display, 0x01);			//	Default value
	st7789_write_command (display, 0xC3);				//	VRH set
	st7789_write_byte (display, 0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
	st7789_write_command (display, 0xC4);				//	VDV set
	st7789_write_byte (display, 0x20);			//	Default value
	st7789_write_command (display, 0xC6);				//	Frame rate control in normal mode
	st7789_write_byte (display, 0x0F);			//	Default value (60HZ)
	st7789_write_command (display, 0xD0);				//	Power control
	st7789_write_byte (display, 0xA4);			//	Default value
	st7789_write_byte (display, 0xA1);			//	Default value
	/**************** Division line ****************/

	st7789_write_command(display, 0xE0);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
		st7789_write_buf(display, data, sizeof(data));
	}

	st7789_write_command(display, 0xE1);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
		st7789_write_buf(display, data, sizeof(data));
	}
	st7789_write_command (display, ST7789_INVON);		//	Inversion ON
	st7789_write_command (display, ST7789_SLPOUT);	//	Out of sleep mode
	st7789_write_command (display, ST7789_NORON);		//	Normal Display on
	st7789_write_command (display, ST7789_DISPON);	//	Main screen turned on	

	HAL_ProgramDelayMs(150);

	return 0;
}
