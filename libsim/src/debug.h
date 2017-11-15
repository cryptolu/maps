/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu>
 *
 * This simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/******************************************************************************
 *
 * Cpu
 *
 ******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef CPU_DEBUG_TRACE
#define CPU_LOG_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define CPU_LOG_TRACE(...)
#endif

#ifdef RSP_DEBUG_TRACE
#define RSP_LOG_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define RSP_LOG_TRACE(...)
#endif

#ifdef REG_TRACE
#define REG_LOG_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define REG_LOG_TRACE(...)
#endif

#endif
