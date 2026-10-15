/**
 * @file
 * @brief	System clock interface
 */

#include <sysclock.h>

bool __attribute__((weak)) sysclock_init(void)
{
	return true;
}
