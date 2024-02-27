#include <stdio.h>
#include <stdlib.h>

#include "server.h"

int main(void) {
  access_file(stdout, "index.html");
  access_file(stdout, "script.js");
  access_file(stdout, "style.css");

  exit(EXIT_SUCCESS);
}
