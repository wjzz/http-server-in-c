#include "server.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <string>

const char SERVE_DIRECTORY[] = "serve";

// TODO: add more errors

enum server_status_code_t {
  OK = 200,
  FILE_NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 503
};

struct payload_t {
  std::string contents;
  std::string content_type;
};

struct raw_response_t {
  server_status_code_t status_code;
  std::optional<payload_t> payload;
};

struct mime_t {
  std::string extension;
  std::string mime_type;
};

std::vector<mime_t> mime_types = {{"js", "text/javascript"},
                                  {"css", "text/css"},
                                  {"html", "text/html"},
                                  {"jpeg", "image/jpeg"}};

std::string get_mime_type_by_filename(const std::string &filename) {
  size_t pos = filename.find('.');
  if (pos == std::string::npos) {
    return "UNKNOWN";
  }

  std::string extension = filename.substr(pos + 1);

  for (auto &mime : mime_types) {
    if (extension == mime.extension) {
      return mime.mime_type;
    }
  }
  return "UNKNOWN";
}

raw_response_t fetch_file(const std::string &requested_path) {
  std::string filename = SERVE_DIRECTORY + std::string("/") + requested_path;

  // TODO: rewrite this using C++ stdlib
  FILE *stream = fopen(filename.c_str(), "r");
  if (stream == NULL) {
    fprintf(stdout, "Couldn't open file %s\n", filename.c_str());
    raw_response_t response = {.status_code = FILE_NOT_FOUND,
                               .payload = std::nullopt};
    return response;
  }

  std::string contents;
  char buf[1024 + 1];
  int n;
  while ((n = fread(buf, sizeof(buf[0]), 1024, stream)) > 0) {
    contents += std::string(buf, buf + n);
  }

  payload_t payload = {.contents =
                           std::string(contents.begin(), contents.end()),
                       .content_type = get_mime_type_by_filename(filename)};
  raw_response_t response = {.status_code = OK,
                             .payload = std::optional(payload)};
  return response;
}

std::string status_code_to_text(server_status_code_t status_code) {
  switch (status_code) {
  case OK:
    return "OK";
  case FILE_NOT_FOUND:
    return "File Not Found";
  case INTERNAL_SERVER_ERROR:
    return "Internal Server Error";
  }
}

// TODO: path sanitanization

std::string render_http_response(const raw_response_t &response) {
  std::string result;

  auto status_code_str = std::to_string(response.status_code);
  auto status_code_text = status_code_to_text(response.status_code);
  result += "HTTP/1.0 " + status_code_str + " " + status_code_text + "\r\n";

  result += "Server: SimpleHttpInC/0.1 C23\r\n";

  if (response.payload) {
    result += "Content-type: " + response.payload->content_type + "\r\n";
    result += "Content-length: " +
              std::to_string(response.payload->contents.length()) + "\r\n";

    // TODO: last-modified

    result += "\r\n";
    result += response.payload->contents;
  }
  return result;
}

std::string access_file(const std::string &requested_path) {
  raw_response_t response = fetch_file(requested_path);
  if (response.payload) {
    std::cout << "response length = " << response.payload->contents.length()
              << std::endl;
  }
  return render_http_response(response);
}
