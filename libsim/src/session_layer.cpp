/******************************************************************************
 *
 * GDB RSP session layer
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

#include "session_layer.h"


#define PORT 50007
#define BACKLOG 1


Session_layer::Session_layer()
{
	if ((this->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		std::exit(EXIT_FAILURE);
	}

	int yes = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		std::exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		close(this->socket_fd);
		perror("bind");
		std::exit(EXIT_FAILURE);
	}

	if (listen(this->socket_fd, BACKLOG) == -1)
	{
		perror("listen");
		close(this->socket_fd);
		std::exit(EXIT_FAILURE);
	}
}

Session_layer::~Session_layer()
{
	close(this->conn_fd);
	close(this->socket_fd);
}

void Session_layer::create(void)
{
	struct sockaddr_storage conn_addr;
	socklen_t conn_size = sizeof(conn_addr);
	printf("-- Waiting for GDB connection on port %u\n", PORT);
	this->conn_fd = accept(this->socket_fd, (struct sockaddr *)&conn_addr, &conn_size);
	if (this->conn_fd == -1)
	{
		perror("accept");
		std::exit(EXIT_FAILURE);
	}
}

ssize_t Session_layer::recv(char *buf, size_t count)
{
	return read(this->conn_fd, buf, count);
}

ssize_t Session_layer::send(char *buf, size_t count)
{
	ssize_t n = write(this->conn_fd, buf, count);
	return n;
}
