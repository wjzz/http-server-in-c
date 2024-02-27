#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int get_socket(void) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockpot");
    exit(EXIT_FAILURE);
  }

  return fd;
}

struct sockaddr_in bind_socket(int fd, int port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  return address;
}

int listen_and_accept_socket(int fd, struct sockaddr_in address) {
  if (listen(fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int new_socket;
  socklen_t addrlen = sizeof(address);
  if ((new_socket = accept(fd, (struct sockaddr *)&address, &addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_socket;
}
