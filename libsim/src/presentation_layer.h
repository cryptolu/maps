/******************************************************************************
 *
 * GDB RSP presentation layer
 *
 ******************************************************************************/

#ifndef __PRESENTATION_LAYER_H__
#define __PRESENTATION_LAYER_H__

#include <cstdlib>
#include <string>
#include "session_layer.h"

typedef enum {P_IDLE, P_PKT, P_CKSUM1, P_CKSUM2} Parser_state;

class Presentation_layer
{
	private:
		Session_layer session;

	public:
		Presentation_layer();
		~Presentation_layer();
		void create(void);
		std::string get_packet(void);
		void send_packet(std::string buf);
		void send_ack(void);
		void send_nack(void);
};


#endif
