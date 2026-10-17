/**
 * @file
 * @brief	System clock interface
 */

#include <sysclock.h>
#include <compiler_attributes.h>

bool __weak sysclock_init(void)
{
	return false;
}
