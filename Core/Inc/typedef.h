
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

//typedef unsigned long		uint32_t;
//typedef unsigned int		uint16_t;
//typedef unsigned char		uint8_t;
typedef signed long			SDWORD;
typedef signed int			SWORD;
typedef signed char			SBYTE;

#define bit_select(B, i)	((B >> i) & 1)
#define bit_set(B, i)		(B |= (1 << i))
#define bit_reset(B, i)		(B &= ~(1 << i))

#endif

