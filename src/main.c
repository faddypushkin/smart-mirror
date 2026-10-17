/**
 * @file
 * @brief	Main program
 */

#include <stdbool.h>

#include <board.h>
#include <sysclock.h>

int main(void)
{
	sysclock_init();
	board_init();

	while (true)
		board_run();

	return 0;
}
