/**
 * @file
 * @brief	
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <st7789/st7789.h>
#include <sensors/sht2x.h>

void platform_display_run(struct st7789 *dev, struct sensors_sht2x *sht2x);

void platform_display_init(struct st7789 *dev);

#endif /* DISPLAY_H_ */
