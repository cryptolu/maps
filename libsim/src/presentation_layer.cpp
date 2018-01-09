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
 * GDB RSP presentation layer
 *
 ******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "presentation_layer.h"
#include "session_layer.h"

Presentation_layer::Presentation_layer()
{
	/* intentionally empty */
}

Presentation_layer::~Presentation_layer()
{
	/* intentionally empty */
}

void Presentation_layer::create(void)
{
	this->session.create();
}

std::string Presentation_layer::get_packet(void)
{
	char current_cksum = 0x00;
	char received_cksum[2];
	char cksum;
	std::string buf = "";
	Parser_state state = P_IDLE;
	bool done = false;
	char c;
	while (done == false)
	{
		ssize_t n = this->session.recv(&c, 1);
		if (n == 0)
		{
			fprintf(stderr, "-- ERROR: session.recv() returned 0 in Presentation_layer::get_packet()\n");
			std::exit(EXIT_FAILURE);
		}
		switch (state)
		{
			case P_IDLE:
				if (c == '$')
				{
					state = P_PKT;
				}
				break;
			case P_PKT:
				if (c == '#')
				{
					state = P_CKSUM1;
				}
				else if (c == '$')
				{
					/* restart new packet */
					buf = "";
					current_cksum = 0x00;
				}
				else
				{
					buf += c;
					current_cksum += c;
				}
				break;
			case P_CKSUM1:
				received_cksum[0] = c;
				state = P_CKSUM2;
				break;
			case P_CKSUM2:
				received_cksum[1] = c;
				errno = 0;
				cksum = strtol(received_cksum, NULL, 16);
				if (errno != 0)
				{
					this->send_nack();
				}
				else if (cksum != current_cksum)
				{
					this->send_nack();
				}
				else
				{
					done = true;
				}
				break;
		}
	}
	return buf;
}

void Presentation_layer::send_packet(std::string buf)
{
	uint8_t cksum = 0x00;
	for (auto it = buf.cbegin(); it != buf.cend(); ++it)
	{
		cksum += (uint8_t)*it;
	}
	while (1)
	{
		char cksum_str[3];
		snprintf(cksum_str, 3, "%02x", cksum);
		std::string pkt = "$" + buf + "#" + cksum_str[0] + cksum_str[1];
		this->session.send((char *)pkt.c_str(), pkt.size());
		char c;
		this->session.recv(&c, 1);
		if (c == '+')
		{
			break;
		}
		else if (c != '-')
		{
			fprintf(stderr, "-- ERROR while waiting for ACK in Presentation_layer::send_packet()\n");
			std::exit(EXIT_FAILURE);
		}
	}
}

void Presentation_layer::send_ack(void)
{
	char c = '+';
	this->session.send(&c, 1);
}

void Presentation_layer::send_nack(void)
{
	char c = '-';
	this->session.send(&c, 1);
}
