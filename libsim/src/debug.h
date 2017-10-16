/******************************************************************************
 *
 * Cpu
 *
 ******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG_TRACE
#define LOG_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#endif
