#include <stdio.h>

#include <platform_display.h>

#include <st7789/st7789-reg.h>
#include <st7789/fronts.h>

void platform_display_run(struct st7789 *dev, struct sensors_sht2x *sht2x)
{
	struct sensors_sht2x_temp temp;
	struct sensors_sht2x_rhum rhum;
	char str[10];

	sensors_sht2x_get_temp(sht2x, &temp);
	sensors_sht2x_get_rhum(sht2x, &rhum);

    snprintf(str, sizeof(str), "%2d.%02uC", temp.integer % 100, temp.decimal % 100);
	st7789_print(dev, 200, 50, RED , BLACK , 1, &Font_16x26, 1, str);
	snprintf(str, sizeof(str), "%2u.%02u%%", rhum.integer % 100, rhum.decimal % 100);
	st7789_print(dev, 200, 80, RED , BLACK , 1, &Font_16x26, 1, str);	
}

void platform_display_init(struct st7789 *dev)
{
	st7789_init(dev);

	st7789_fill_screen(dev, BLACK);

	st7789_print(dev, 0, 0, RED , BLACK , 0, &Font_16x26, 2, "MIK32 Амур");
	st7789_print(dev, 0, 50, RED , BLACK , 0, &Font_16x26, 1, "Температура:");
	st7789_print(dev, 0, 80, RED , BLACK , 0, &Font_16x26, 1, "Отн. вл-сть:" );
	st7789_print(dev, 0, 110, RED , BLACK , 0, &Font_16x26, 1, "Давление:" );
	st7789_print(dev, 0, 140, RED , BLACK , 0, &Font_16x26, 1, "Высота:" );
}
