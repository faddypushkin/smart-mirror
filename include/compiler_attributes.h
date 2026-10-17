/**
 * @file
 * @brief	Compiler attribute definitions
 */

#ifndef COMPILER_ATTRIBUTES_H_
#define COMPILER_ATTRIBUTES_H_

#ifndef __packed
#define __packed	__attribute__((__packed__))
#endif

#ifndef __used
#define __used		__attribute__((__used__))
#endif

#ifndef __weak
#define __weak		__attribute__((__weak__))
#endif

#endif /* COMPILER_ATTRIBUTES_H_ */
