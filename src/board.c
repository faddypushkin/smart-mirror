/**
 * @file
 * @brief	Board interface
 */

#include <board.h>

void __attribute__((weak)) board_run(void)
{
}

bool __attribute__((weak)) board_init(void)
{
	return false;
}
