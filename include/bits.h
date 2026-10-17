/**
 * @file
 * @brief	Bit manipulation macros
 */

#ifndef BITS_H_
#define BITS_H_

#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))

#define BITS_PER_BYTE		8
#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)

#endif /* BITS_H_ */
