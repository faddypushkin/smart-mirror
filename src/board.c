/**
 * @file
 * @brief	Board interface
 */

#include <board.h>
#include <compiler_attributes.h>

void __weak board_run(void)
{
}

bool __weak board_init(void)
{
	return false;
}
