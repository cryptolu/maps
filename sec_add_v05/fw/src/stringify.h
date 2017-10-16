/*
 * This is a work of the U.S. Government and is not subject to copyright
 * protection in the United States. Foreign copyrights may apply.
 *
 * Written in 2015 by Jason Smith <jksmit3@tycho.ncsc.mil> and
 *                    Bryan Weeks <beweeks@tycho.ncsc.mil>
 */


#ifndef __STRINGIFY_H__
#define __STRINGIFY_H__

/* Stringification hacks to make inline ASM macros work */
#define STR_(...) #__VA_ARGS__
#define STR(...) STR_(__VA_ARGS__)

#endif
