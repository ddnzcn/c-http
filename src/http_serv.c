#include "http_serv.h"
#include "request/HttpRequest.h"
#include "response/HttpResponse.h"
#include "router/Router.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

HTTP_SOCKET create_server(int port) {
  HTTP_SOCKET new_socket;

  new_socket.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (new_socket.socket_fd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  printf("Created socket!\n");

  // Allow reuse of port
  int opt = 1;
  if (setsockopt(new_socket.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) < 0) {
    perror("setsockopt failed");
    close(new_socket.socket_fd);
    exit(EXIT_FAILURE);
  }

  new_socket.addr.sin_family = AF_INET;
  new_socket.addr.sin_addr.s_addr = INADDR_ANY;
  new_socket.addr.sin_port = htons(port);

  if (bind(new_socket.socket_fd, (struct sockaddr *)&new_socket.addr,
           sizeof(new_socket.addr)) < 0) {
    perror("Bind failed");
    close(new_socket.socket_fd);
    exit(EXIT_FAILURE);
  }
  printf("Socket bound to port %d.\n", port);

  if (listen(new_socket.socket_fd, 5) < 0) {
    perror("Listen failed");
    close(new_socket.socket_fd);
    exit(EXIT_FAILURE);
  }
  printf("Server listening...\n");

  return new_socket;
}

void start_listen_loop(HTTP_SOCKET server_socket) {
  // poll setup
  struct pollfd pfds[16];

  pfds[0].fd = server_socket.socket_fd;
  pfds[0].events = POLLIN;
  int nfds = 1;

  while (1) {
    int ready = poll(pfds, nfds, -1);
    if (ready == -1) {
      perror("poll error");
      // cleanup?
      close(server_socket.socket_fd);
      exit(-1);
    }

    if (pfds[0].revents & POLLIN) {
      int new_client_fd = accept(server_socket.socket_fd, NULL, NULL);
      if (new_client_fd != -1) {
        if (nfds < 16) {
          pfds[nfds].fd = new_client_fd;
          pfds[nfds].events = POLLIN;
          nfds++;
          printf("New client! fd=%d\n", new_client_fd);
        } else {
          printf("Max client reached, closing fd!");
          close(new_client_fd);
        }
      }
    }

    for (int i = 1; i < nfds; i++) {
      if (pfds[i].revents & POLLIN) {
        HttpRequest *req = parse_request(pfds[i].fd);

        handle_request(req, pfds[i].fd);

        close(pfds[i].fd);
        free_req(req);

        pfds[i] = pfds[nfds - 1];
        nfds--;
        i--;
      }
    }
  }
}
