#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

const char SERVE_DIRECTORY[] = "serve";

// TODO: add more errors

typedef enum { OK = 200, INTERNAL_SERVER_ERROR = 503 } server_status_code_t;

typedef struct {
  int length;
  char *contents_own;
  char *content_type_own;
} payload_t;

typedef struct {
  server_status_code_t status_code;
  payload_t *payload;
} raw_response_t;

typedef struct {
  char *extension;
  char *mime_type;
} mime_t;

mime_t mime_types[] = {{"js", "text/javascript"},
                       {"css", "text/css"},
                       {"html", "text/html"},
                       {"jpeg", "image/jpeg"}};

char *get_mime_type_by_filename(char *filename) {
  char *token = strsep(&filename, ".");
  token = strsep(&filename, ".");

  for (size_t i = 0; i < sizeof(mime_types) / sizeof(mime_types[0]); ++i) {
    if (strcmp(mime_types[i].extension, token) == 0) {
      return mime_types[i].mime_type;
    }
  }
  return NULL;
}

raw_response_t fetch_file(char *requested_path) {
  char filename[256];
  snprintf(filename, sizeof(filename), "%s/%s", SERVE_DIRECTORY,
           requested_path);

  FILE *stream = fopen(filename, "r");
  if (stream == NULL) {
    fprintf(stdout, "Couldn't open file %s\n", filename);
    raw_response_t response = {.status_code = INTERNAL_SERVER_ERROR,
                               .payload = NULL};
    return response;
  }

  // TODO: make sure we can handle big files by resizing this buffer if needed
  char *contents = calloc(1 + 1024 * 10, sizeof(char));
  int contents_size = 0;

  char buf[1024];
  int n;
  while ((n = fread(buf, sizeof(buf[0]), 1024, stream)) > 0) {
    memcpy(contents + contents_size, buf, n);
    contents_size += n;
  }

  char *mime_buf = get_mime_type_by_filename(filename);
  char *content_type = calloc(100, sizeof(char));
  strncpy(content_type, mime_buf, strlen(mime_buf));

  payload_t *payload = malloc(sizeof(payload_t));
  payload->length = contents_size;
  payload->contents_own = contents;
  payload->content_type_own = content_type;
  raw_response_t response = {.status_code = OK, .payload = payload};
  return response;
}

char *status_code_to_text(server_status_code_t status_code, int n,
                          char buf[n]) {
  switch (n) {
  case 200:
    snprintf(buf, n, "OK");
  }
  return buf;
}

// TODO: path sanitanization

void send_http_response(FILE *fp, raw_response_t response) {
  char status_code_msg[256] = {0};
  fprintf(fp, "HTTP/1.0 %d %s\n", response.status_code,
          status_code_to_text(response.status_code, 256, status_code_msg));

  fprintf(fp, "Server: SimpleHttpInC/0.1 C23\n");

  if (response.payload != NULL) {
    fprintf(fp, "Content-type: %s\n", response.payload->content_type_own);
    fprintf(fp, "Content-length: %d\n", response.payload->length);

    // TODO: last-modified

    fprintf(fp, "\n");
    fprintf(fp, "%s\n", response.payload->contents_own);

    free(response.payload->content_type_own);
    free(response.payload->contents_own);
    free(response.payload);
  }
}

void access_file(FILE *fp, char *requested_path) {
  raw_response_t response = fetch_file(requested_path);
  send_http_response(fp, response);
}
