#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"
#include "sockets.h"

const int PORT = 4444;
const int BUFSIZE = 2048;

struct request_t {
  std::string method;
  std::string path;
};

request_t parse_request(const std::string &input) {
  size_t pos = input.find_first_of('\n');
  std::string first_line = input.substr(0, pos);
  std::cout << first_line << std::endl;

  size_t space_pos = first_line.find_first_of(' ');
  std::string method = first_line.substr(0, space_pos);
  std::string remaining = first_line.substr(1 + space_pos);
  size_t second_space_pos = remaining.find_first_of(' ');
  std::string path = remaining.substr(
      1, second_space_pos - 1); // remove the / at the beginning

  if (path == "") {
    path = "index.html";
  }

  request_t request = {.method = method, .path = path};
  return request;
}

void handle_connection(int socket) {
  // TODO: maybe we don't read everything here
  char buffer[BUFSIZE] = {0};
  read(socket, buffer, BUFSIZE - 1);

  //   printf("Got message %s\n", buffer);

  request_t request = parse_request(buffer);
  printf("Got path = %s\n", request.path.c_str());

  std::string payload = access_file(request.path);

  send(socket, payload.c_str(), payload.length(), 0);
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
