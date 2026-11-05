#include <display.h>
#include <spi_core.h>
#include <arch_gpio.h>
#include <gpio_core.h>
#include <uart_core.h>

#include <st7789/fronts.h>

#include <mik32_hal.h>
#include <mik32_memory_map.h>

/* Choose a display rotation you want to use: (0-3) */
//#define ST7789_ROTATION 0
#define ST7789_ROTATION 1
//#define ST7789_ROTATION 2				//  use Normally on 240x240
//#define ST7789_ROTATION 3

#if ST7789_ROTATION == 0
    #define ST7789_WIDTH  240
    #define ST7789_HEIGHT 320
    #define X_SHIFT 0
    #define Y_SHIFT 0
#elif ST7789_ROTATION == 1
    #define ST7789_WIDTH  320
    #define ST7789_HEIGHT 240
    #define X_SHIFT 0
    #define Y_SHIFT 0
#elif ST7789_ROTATION == 2
    #define ST7789_WIDTH  240
    #define ST7789_HEIGHT 320
    #define X_SHIFT 0
    #define Y_SHIFT 0
#elif ST7789_ROTATION == 3
    #define ST7789_WIDTH  320
    #define ST7789_HEIGHT 240
    #define X_SHIFT 0
    #define Y_SHIFT 0
#endif


/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458

#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

/* Control Registers and constant codes */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

/**
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

/* Advanced options */
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

static struct arch_gpio_chip gpio1 = { .gpio = GPIO_1 };
static struct gpio_pin res = { .chip = &gpio1 };
static struct gpio_pin dc = { .chip = &gpio1 };
static struct gpio_pin cs = { .chip = &gpio1 };
static struct gpio_pin bl = { .chip = &gpio1 };

static void write_command(uint8_t command)
{
	gpio_core_pin_set(&cs, 0);
	gpio_core_pin_set(&dc, 0);
	spi_write_buf(&command, sizeof(command));
	gpio_core_pin_set(&cs, 1);
}

static void write_byte(uint8_t byte)
{
	gpio_core_pin_set(&cs, 0);
	gpio_core_pin_set(&dc, 1);
	spi_write_buf(&byte, sizeof(byte));
	gpio_core_pin_set(&cs, 1);
}

static void write_data(uint8_t *buff, size_t buff_size)
{
	gpio_core_pin_set(&cs, 0);
	gpio_core_pin_set(&dc, 1);

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
		spi_write_buf(buff, chunk_size);
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	gpio_core_pin_set(&cs, 1);
}

static void set_rotation(uint8_t m)
{
	write_command(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		write_byte(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		write_byte(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		write_byte(ST7789_MADCTL_RGB);
		break;
	case 3:
		write_byte(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    //gpio_core_pin_set(&cs, 0);
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
	
	/* Column Address set */
	write_command(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		write_data(data, sizeof(data));
	}

	/* Row Address set */
	write_command(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		write_data(data, sizeof(data));
	}
	/* Write to RAM */
	write_command(ST7789_RAMWR);
	//gpio_core_pin_set(&cs, 1);
}

static void st7789_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
	uint8_t data [sizeof(color)];

	if ((x < 0) ||(x >= ST7789_WIDTH) || (y < 0) || (y >= ST7789_HEIGHT))
		return;
	
	data[0] = color >> 8;
	data[1] = color & 0xFF;

	set_window(x, y, x, y);
	write_data(data, sizeof(data));
}

static void st7789_draw_char(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch){
	
	uint32_t i, b, j;
	
	uint32_t X = x, Y = y;
	
	uint8_t xx, yy;
	
	if( multiplier < 1 ){
		multiplier = 1;
	}

	/* Check available space in LCD */
	if (ST7789_WIDTH >= ( x + Font->FontWidth) || ST7789_HEIGHT >= ( y + Font->FontHeight)){

	
			/* Go through font */
			for (i = 0; i < Font->FontHeight; i++) {		
				
				if( ch < 127 ){			
					b = Font->data[(ch - 32) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch > 191 ){
					// +96 это так как латинские символы и знаки в шрифтах занимают 96 позиций
					// и если в шрифте который содержит сперва латиницу и спец символы и потом 
					// только кирилицу то нужно добавлять 95 если шрифт 
					// содержит только кирилицу то +96 не нужно
					b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
					// 160 эллемент ( символ Ё ) 
					b = Font->data[( 160 ) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
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
			
				for (j = 0; j < Font->FontWidth; j++) {
					
					if ((b << j) & 0x8000) {
						
						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
									st7789_draw_pixel(X+xx, Y+yy, TextColor);
							}
						}
						
					} 
					else if( TransparentBg ){
						
						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
									st7789_draw_pixel(X+xx, Y+yy, BgColor);
							}
						}
						
					}
					X = X + multiplier;
				}
				X = x;
				Y = Y + multiplier;
			}
	}
}

static void st7789_print(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str)
{	
	
	if( multiplier < 1 ){
		multiplier = 1;
	}
	
	unsigned char buff_char;
	
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
			
			st7789_draw_char(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, buff_char);
		}
		//---------------------------------------------------------------------
		else{
			st7789_draw_char(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, *str);
		}
		
		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}

void display_print(uint16_t x, uint16_t y, const char *text)
{
	(void) x;
	(void) y;
	(void) text;
}

static void display_fill_screen(uint16_t color)
{
    uint16_t i;
	set_window(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	uint8_t data [ST7789_HEIGHT];

	for (uint16_t j = 0; j < sizeof(data); j += 2) {
		data[j] = color >> 8;
		data[j + 1] = color & 0xFF;
	}

	gpio_core_pin_set(&cs, 0);
	gpio_core_pin_set(&dc, 1);

	for (i = 0; i < 2 * ST7789_WIDTH; i++)
		spi_write_buf(data, sizeof(data));

	gpio_core_pin_set(&cs, 1);
}

void display_init(void)
{
	arch_gpio_init(&gpio1);
	gpio_core_pin_config(&res, 10, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&dc, 9, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&cs, 8, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_config(&bl, 7, GPIO_CORE_FLAG_DIR_OUT);
	gpio_core_pin_set(&bl, 1);

	HAL_ProgramDelayMs(10);
	gpio_core_pin_set(&res, 0);
    HAL_ProgramDelayMs(10);
    gpio_core_pin_set(&res, 1);
    HAL_ProgramDelayMs(150);                          // БЫЛО: delay_ms(20)

	write_command(ST7789_SWRESET);          // ДОБАВИТЬ (опционально, но полезно)
	HAL_ProgramDelayMs(150);

	write_command(ST7789_SLPOUT);           // ДОБАВИТЬ — обязательно
	HAL_ProgramDelayMs(150);

    write_command(ST7789_COLMOD);				//	Set color mode
    write_byte(ST7789_COLOR_MODE_16bit);
  	write_command(0xB2);						//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		write_data(data, sizeof(data));
	}
	set_rotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
	
	/* Internal LCD Voltage generator settings */
    write_command(0XB7);				//	Gate Control
    write_byte(0x35);			//	Default value
    write_command(0xBB);				//	VCOM setting
    write_byte(0x19);			//	0.725v (default 0.75v for 0x20)
    write_command(0xC0);				//	LCMCTRL	
    write_byte (0x2C);			//	Default value
    write_command (0xC2);				//	VDV and VRH command Enable
    write_byte (0x01);			//	Default value
    write_command (0xC3);				//	VRH set
    write_byte (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
    write_command (0xC4);				//	VDV set
    write_byte (0x20);			//	Default value
    write_command (0xC6);				//	Frame rate control in normal mode
    write_byte (0x0F);			//	Default value (60HZ)
    write_command (0xD0);				//	Power control
    write_byte (0xA4);			//	Default value
    write_byte (0xA1);			//	Default value
	/**************** Division line ****************/

	write_command(0xE0);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
		write_data(data, sizeof(data));
	}

    write_command(0xE1);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
		write_data(data, sizeof(data));
	}
    write_command (ST7789_INVON);		//	Inversion ON
	write_command (ST7789_SLPOUT);	//	Out of sleep mode
  	write_command (ST7789_NORON);		//	Normal Display on
  	write_command (ST7789_DISPON);	//	Main screen turned on	

	HAL_ProgramDelayMs(150);
	//display_fill_screen(RED);
	//display_fill_screen(GREEN);
	//display_fill_screen(BLUE);

	//display_fill_screen(BROWN);
	//for (int i = 0; i < 10; ++i)
	//	st7789_draw_pixel(10, 10 + i, BLACK);

	display_fill_screen(BLACK);
	//st7789_draw_char( 0, 0, RED , BLACK , 0, &Font_16x26, 3, 'П' );

	//st7789_print( 50, 20, RED , BLACK , 0, &Font_16x26, 1, "STM32 TFT" );
	st7789_print(0, 0, RED , BLACK , 0, &Font_16x26, 1, "Я в рот ебал это..." );
}
