#pragma once

#include <arpa/inet.h>

typedef struct {
  int socket_fd;
  struct sockaddr_in addr;
} HTTP_SOCKET;

HTTP_SOCKET create_server(int port);
void start_listen_loop(HTTP_SOCKET server_socket);
