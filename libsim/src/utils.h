/******************************************************************************
 *
 * utilities
 *
 ******************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#define GET_BIT(x, n) (((x) >> (n)) & 1)
#define GET_FIELD(x, start, len) (((x) >> (start)) & ((1 << (len)) - 1))

unsigned int bit_count(uint32_t x);

#endif
