#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint-gcc.h>

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#ifndef __cplusplus
// Standard types
typedef uint32_t bool;

// Boolean types
#define FALSE ((bool) 0)
#define TRUE  ((bool) 1)

#define GET_NEXT_BUFF_U16_IDX(u16BufferIdx, BUFFER_SIZE) \
{ \
	uint16_t u16Temp = u16BufferIdx; \
	u16Temp++; \
	/* See if buffer size is a power of two */ \
	if((BUFFER_SIZE&(BUFFER_SIZE-1)) == 0) \
	{ \
		/* Use masking to optimize pointer wrapping to index 0 */ \
		u16Temp &= (BUFFER_SIZE-1); \
	} \
	else \
	{ \
		/* Wrap index to 0 if it has exceeded buffer boundary */ \
		if(u16Temp == (uint16_t)BUFFER_SIZE) u16Temp = 0; \
	} \
	u16BufferIdx = u16Temp; \
}

#if !defined(__cplusplus) && !defined(__true_false_are_keywords)
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

#endif

#define ABS(x)	(x >= 0) ? x : -x

#endif /* GLOBAL_H_ */
