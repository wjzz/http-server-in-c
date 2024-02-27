#ifndef SERVER_H
#define SERVER_H

typedef enum { OK = 200, INTERNAL_SERVER_ERROR = 503 } server_status_code_t;

server_status_code_t serve_file(char *requested_path);
char *status_code_text(server_status_code_t status_code, int n, char buf[n]);

#endif
