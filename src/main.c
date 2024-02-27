#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

// TODO: path sanitanization
// TODO: mime types

int main(void) {
  server_status_code_t status_code;
  status_code = serve_file("index.html");
  printf("status code = %d\n", status_code);

  exit(EXIT_SUCCESS);
}
