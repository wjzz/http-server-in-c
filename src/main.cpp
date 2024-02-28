#include <iostream>
#include <string>
#include <unistd.h>

#include "server.h"
#include "sockets.h"

const int PORT = 4444;
const int BUFSIZE = 2048;

struct request_t {
  std::string method;
  std::string path;
};

std::tuple<std::string, std::string> extract_path(const std::string &input) {
  size_t pos = input.find_first_of('\n');
  std::string first_line = input.substr(0, pos);
  std::cout << first_line << std::endl;

  size_t space_pos = first_line.find_first_of(' ');
  std::string method = first_line.substr(0, space_pos);

  std::string remaining = first_line.substr(1 + space_pos);
  size_t second_space_pos = remaining.find_first_of(' ');

  std::string path = remaining.substr(
      1, second_space_pos - 1); // remove the / at the beginning
  return std::tuple(method, path);
}

request_t parse_request(const std::string &input) {
  auto [method, path] = extract_path(input);

  if (path == "") {
    path = "index.html";
  }

  request_t request = {.method = method, .path = path};
  return request;
}

std::string read_all(int socket) {
  std::string request_buffer;
  char buffer[BUFSIZE] = {0};
  // int n;
  // while ((n = read(socket, buffer, BUFSIZE - 1)) > 0) {
  //   request_buffer += std::string(buffer, buffer + n);
  // }
  read(socket, buffer, BUFSIZE - 1);
  request_buffer = buffer;
  return request_buffer;
}

void handle_connection(int socket) {
  auto request_buffer = read_all(socket);

  request_t request = parse_request(request_buffer);
  std::cout << "Got path = " << request.path << std::endl;

  std::string payload = access_file(request.path);

  send(socket, payload.c_str(), payload.length(), 0);
  std::cout << "Reply OK" << std::endl;

  close(socket);
}

int main(void) {
  int server_fd = get_socket();
  struct sockaddr_in address = bind_socket(server_fd, PORT);
  std::cout << "bind OK" << std::endl;

  while (true) {
    int new_socket = listen_and_accept_socket(server_fd, address);
    std::cout << "Got connection" << std::endl;

    handle_connection(new_socket);
  }

  close(server_fd);
  exit(EXIT_SUCCESS);
}
