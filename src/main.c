#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"
#include "sockets.h"

const int PORT = 4444;
const int BUFSIZE = 2048;

typedef struct {
  char *path;
} request_t;

request_t parse_request(char *buf) {
  char *token = strsep(&buf, "\n");
  //   printf("DEBUG %s\n", token);

  strsep(&token, " ");
  char *path = strsep(&token, " ");
  //   char *protocol = token;
  //   printf("method %s\n", method);
  //   printf("path %s\n", path);
  //   printf("protocol %s\n", protocol);

  // remove the / at the beginning
  path++;
  if (strcmp(path, "") == 0) {
    char buf[] = "index.html";
    path = calloc(strlen(buf), sizeof(char));
    strncpy(path, buf, strlen(buf));
  }
  request_t request = {.path = path};
  return request;
}

void handle_connection(int socket) {
  char buffer[BUFSIZE] = {0};
  read(socket, buffer, BUFSIZE - 1);

  //   printf("Got message %s\n", buffer);

  request_t request = parse_request(buffer);
  printf("Got path = %s\n", request.path);

  char *msg;
  size_t size;
  FILE *stream = open_memstream(&msg, &size);

  access_file(stream, request.path);
  fflush(stream);

  send(socket, msg, size, 0);
  printf("Reply OK.\n");

  close(socket);
}

int main(void) {
  int server_fd = get_socket();
  struct sockaddr_in address = bind_socket(server_fd, PORT);
  printf("bind OK!\n");

  while (true) {
    int new_socket = listen_and_accept_socket(server_fd, address);
    printf("Got connection!\n");

    handle_connection(new_socket);
  }

  close(server_fd);
  exit(EXIT_SUCCESS);
}
