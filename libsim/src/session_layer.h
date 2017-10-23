/******************************************************************************
 *
 * GDB RSP session layer
 *
 ******************************************************************************/

#ifndef __SESSION_LAYER_H__
#define __SESSION_LAYER_H__

#include <cstdlib>
#include <cstdint>

class Session_layer
{
	private:
		int socket_fd;
		int conn_fd;

	public:
		Session_layer();
		~Session_layer();

		void create(void);
		ssize_t recv(char *buf, size_t count);
		ssize_t send(char *buf, size_t count);
};

#endif
