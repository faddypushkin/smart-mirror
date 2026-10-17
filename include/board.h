/**
 * @file
 * @brief	Board API
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

/**
 * @brief	Process board events
 */
void board_run(void);

/**
 * @brief	Initialize the board
 *
 * @return	initialization status
 */
bool board_init(void);

#endif /* BOARD_H_ */
