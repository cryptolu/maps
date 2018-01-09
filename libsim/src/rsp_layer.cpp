/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * arm_v7m_leakage simulator
 *
 * Copyright (C) 2017 University of Luxembourg
 *
 * Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu> and
 * Daniel Dinu <daniel.dinu@uni.lu>
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
 * GDB RSP layer
 *
 ******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>

#include "cpu.h"
#include "presentation_layer.h"
#include "rsp_layer.h"

//#define RSP_DEBUG_TRACE
#include "debug.h"


#define SWAP32(value) \
	((((value) >> 24) & 0xff) | \
	 (((value) << 8) & 0xff0000) | \
	 (((value) >> 8) & 0xff00) | \
	 (((value) << 24) & 0xff000000));


Rsp_layer::Rsp_layer()
{
	/* intentionally empty */
}

Rsp_layer::~Rsp_layer()
{
	/* intentionally empty */
}

void Rsp_layer::byte_to_str(char *str, uint8_t b)
{
	snprintf(str, 3, "%02x", b);
}

void Rsp_layer::report_error(const char *msg, const char *location)
{
	fprintf(stderr, "-- ERROR: %s in %s\n", msg, location);
	std::exit(EXIT_FAILURE);
}

void Rsp_layer::push_uint8(std::string &str, uint8_t x)
{
	char buf[3];

	this->byte_to_str(buf, (x >>  0) & 0xff);
	str += buf;
}

void Rsp_layer::push_uint32(std::string &str, uint32_t x)
{
	push_uint8(str, (x >>  0) & 0xff);
	push_uint8(str, (x >>  8) & 0xff);
	push_uint8(str, (x >> 16) & 0xff);
	push_uint8(str, (x >> 24) & 0xff);
}

void Rsp_layer::run(Cpu *cpu)
{
	this->presentation.create();
	bool done = false;
	while (done == false)
	{
		std::string request = this->presentation.get_packet();
		std::string reply;
		RSP_LOG_TRACE("request = <%s>\n", request.c_str());
		switch (request[0])
		{
			case 'H': /* set thread operation */
				this->presentation.send_ack();
				reply = "";
				break;
			case '?': /* status? */
				this->presentation.send_ack();
				/* TODO: reply with 'T' packets with reason, once figured out which one should be used */
				reply = "S05"; /* breakpoint trap */
				break;
			case 'g': /* read registers */
				this->presentation.send_ack();
				for (unsigned int i = 0; i < 16; ++i)
				{
					push_uint32(reply, cpu->read_register(i));
				}
				break;
			case 'm': /* read memory */
				this->presentation.send_ack();
				{
					char *sptr = (char *)request.c_str() + 1;
					char *eptr;
					unsigned int addr = strtoul(sptr, &eptr, 16);
					unsigned int len = strtoul(eptr + 1, NULL, 16);
					for (unsigned int i = 0; i < len; ++i)
					{
						push_uint8(reply, cpu->read8_ram(addr + i));
					}
				}
				break;
			case 'M': /* read memory */
				this->presentation.send_ack();
				{
					char *ptr1 = (char *)request.c_str() + 1;
					char *ptr2;
					unsigned int addr = strtoul(ptr1, &ptr2, 16); /* addr */
					ptr2++; /* , */
					unsigned int len = strtoul(ptr2, &ptr1, 16); /* length */
					ptr1++; /* : */
					char buf[3];
					buf[2] = '\0';
					for (unsigned int i = 0; i < len; ++i)
					{
						buf[0] = *ptr1;
						buf[1] = *(ptr1 + 1);
						ptr1 += 2;
						unsigned int value = strtoul(buf, NULL, 16);
						cpu->write8_ram(addr + i, value);
					}
					reply = "OK";
				}
				break;
			case 'p': /* read register */
				this->presentation.send_ack();
				{
					char *sptr = (char *)request.c_str() + 1;
					unsigned int reg_idx = strtoul(sptr, NULL, 16);
					/* Note that register #0x19 is PSR reg */
					if (reg_idx == 25)
					{ /* APSR */
						push_uint32(reply, cpu->read_apsr());
					}
					else
					{
						push_uint32(reply, cpu->read_register(reg_idx));
					}
				}
				break;
			case 'P': /* write register */
				this->presentation.send_ack();
				{
					char *sptr = (char *)request.c_str() + 1;
					char *eptr;
					unsigned int reg_idx = strtoul(sptr, &eptr, 16);
					uint32_t value = strtoul(eptr + 1, NULL, 16);
					/* value is sent low bytes first, so we need to reorder bytes */
					value = SWAP32(value);
					cpu->write_register(reg_idx, value);
					reply = "OK";
				}
				break;
			case 's': /* step */
				if (request.size() == 1)
				{
					cpu->step();
					this->presentation.send_ack();
					reply = "S05";
				}
				break;
			case 'c': /* continue */
				if (request.size() > '1')
				{
					char *sptr = (char *)request.c_str() + 1;
					unsigned int addr = strtoul(sptr, NULL, 16);
					cpu->write_register(PC, addr);
				}
				while (1)
				{
					if (cpu->step() == -1)
					{
						break;
					}
				}
				this->presentation.send_ack();
				reply = "S05";
				break;
			case 'k': /* kill */
				this->presentation.send_ack();
				/* don't send any reply. It's safer to play dead in case the murderer would still be around ;-) */
				break;
			case 'q': /* query */
				this->presentation.send_ack();
				if (request[1] == 'S' && request[2] == 'u')
				{
					reply = "swbreak+";
				}
				break;
			default:
				this->presentation.send_ack();
				RSP_LOG_TRACE("unsupported packet '%c'\n", request[0]);
				reply = "";
				break;
		}
		this->presentation.send_packet(reply);
	}
}
