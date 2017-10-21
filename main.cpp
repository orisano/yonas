#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

#include "picohttpparser.h"

static const char* HTTP_HEADER="HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/json\r\n\r\n%s\r\n";

char resp_buffer[1001000];
int response_http(int fd, const char* json) {
  int len = sprintf(resp_buffer, HTTP_HEADER, strlen(json) + 2, json);
  return write(fd, resp_buffer, len);
}

int main() {
  int listen_fd, r;
  struct sockaddr_un local, remote;

  signal(SIGPIPE, SIG_IGN);
  
  listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, "yonas.sock");
  unlink(local.sun_path);

  r = bind(listen_fd, (struct sockaddr*)&local, sizeof(local));
  if (r) {
    perror("failed to bind");
  }

  listen(listen_fd, 100);
  for (;;) {
    socklen_t len = sizeof(remote);
    int remote_fd = accept(listen_fd, (struct sockaddr*)&remote, &len);
    if (remote_fd < 0) {
      perror("failed to accept");
      return 0;
    }

    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;
    while (1) {
      while ((rret = read(remote_fd, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR);
      if (rret <= 0) {
        puts("IOError");
        goto fail;
      }
      prevbuflen = buflen;
      buflen += rret;

      num_headers = sizeof(headers) / sizeof(headers[0]);
      pret = phr_parse_request(buf, buflen, (const char**)&method, &method_len, (const char**)&path, &path_len, &minor_version, headers, &num_headers, prevbuflen);
      if (pret > 0) break;
      if (pret == -1) {
        puts("ParseError");
        goto fail;
      }
      if (buflen == sizeof(buf)) {
        puts("RequestIsTooLongError");
        goto fail;
      }
    }
    response_http(remote_fd, "{\"status\": \"ok\"}"); 
fail:;
  }
}
