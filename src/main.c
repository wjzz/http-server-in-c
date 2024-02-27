#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

// TODO: path sanitanization
// TODO: mime types

typedef struct {
  int length;
  char *contents_own;
  char *content_type_own;
} payload_t;

void send_http_response(server_status_code_t status_code,
                        payload_t *payload_own) {
  FILE *fp = stdout;

  char status_code_msg[256] = {0};
  fprintf(fp, "HTTP/1.0 GET %d %s\n", status_code,
          status_code_text(status_code, 256, status_code_msg));

  fprintf(fp, "Server: SimpleHttpInC/0.1 C23\n");

  if (payload_own != NULL) {
    fprintf(fp, "Content-type: %s\n", payload_own->content_type_own);
    fprintf(fp, "Content-length: %d\n", payload_own->length);

    // TODO: last-modified

    fprintf(fp, "\n");
    fprintf(fp, "%s\n", payload_own->contents_own);

    free(payload_own->content_type_own);
    free(payload_own->contents_own);
    free(payload_own);
  }
}

int main(void) {
  server_status_code_t status_code;
  status_code = serve_file("index.html");
  printf("status code = %d\n", status_code);

  exit(EXIT_SUCCESS);
}
