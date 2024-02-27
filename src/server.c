#include <stdio.h>

#include "server.h"

const char SERVE_DIRECTORY[] = "serve";

server_status_code_t serve_file(char *requested_path) {
  char filename[256];
  snprintf(filename, sizeof(filename), "%s/%s", SERVE_DIRECTORY,
           requested_path);

  FILE *stream = fopen(filename, "r");
  if (stream == NULL) {
    fprintf(stdout, "Couldn't open file %s\n", filename);
    return INTERNAL_SERVER_ERROR;
  }

  char buf[1024 + 1];
  int n;
  while ((n = fread(buf, sizeof(buf[0]), 1024, stream)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }

  return OK;
}
