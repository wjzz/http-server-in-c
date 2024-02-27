#include "server.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <string>

const char SERVE_DIRECTORY[] = "serve";

// TODO: add more errors

typedef enum { OK = 200, INTERNAL_SERVER_ERROR = 503 } server_status_code_t;

struct payload_t {
  std::string contents_own;
  std::string content_type_own;
};

// TODO: change the pointer type
struct raw_response_t {
  server_status_code_t status_code;
  payload_t *payload;
};

struct mime_t {
  std::string extension;
  std::string mime_type;
};

mime_t mime_types[] = {{"js", "text/javascript"},
                       {"css", "text/css"},
                       {"html", "text/html"},
                       {"jpeg", "image/jpeg"}};

std::string get_mime_type_by_filename(const std::string &filename) {
  // TODO: handle errors if no extension
  size_t pos = filename.find('.');
  std::string extension = filename.substr(pos + 1);

  for (size_t i = 0; i < sizeof(mime_types) / sizeof(mime_types[0]); ++i) {
    if (extension == mime_types[i].extension) {
      return mime_types[i].mime_type;
    }
  }
  return NULL;
}

raw_response_t fetch_file(std::string requested_path) {
  std::string filename = SERVE_DIRECTORY + std::string("/") + requested_path;

  // TODO: rewrite this using C++ stdlib
  FILE *stream = fopen(filename.c_str(), "r");
  if (stream == NULL) {
    fprintf(stdout, "Couldn't open file %s\n", filename.c_str());
    raw_response_t response = {.status_code = INTERNAL_SERVER_ERROR,
                               .payload = NULL};
    return response;
  }

  std::string contents;

  char buf[1024 + 1];
  int n;
  while ((n = fread(buf, sizeof(buf[0]), 1024, stream)) > 0) {
    buf[n] = '\0';
    contents += buf;
  }

  payload_t *payload = new payload_t;
  payload->contents_own = contents;
  payload->content_type_own = get_mime_type_by_filename(filename);
  raw_response_t response = {.status_code = OK, .payload = payload};
  return response;
}

std::string status_code_to_text(server_status_code_t status_code) {
  switch (status_code) {
  case OK:
    return "OK";
  case INTERNAL_SERVER_ERROR:
    return "Internal Server Error";
  }
}

// TODO: path sanitanization

std::string render_http_response(raw_response_t response) {
  std::string result;

  auto status_code_str = std::to_string(response.status_code);
  auto status_code_text = status_code_to_text(response.status_code);
  result += "HTTP/1.0 " + status_code_str + " " + status_code_text + "\r\n";

  result += "Server: SimpleHttpInC/0.1 C23\r\n";

  if (response.payload != NULL) {
    result += "Content-type: " + response.payload->content_type_own + "\r\n";
    result += "Content-length: " +
              std::to_string(response.payload->contents_own.length()) + "\r\n";

    // TODO: last-modified

    result += "\r\n";
    result += response.payload->contents_own;
  }
  return result;
}

std::string access_file(const std::string &requested_path) {
  raw_response_t response = fetch_file(requested_path);
  return render_http_response(response);
}
