#include <cstdio>  // for sprintf, puts
#include <cstring> // for strlen
#include <csignal> // for signal, SIGPIPE, SIG_IGN

#include <errno.h>  // for errno, EINTR
#include <unistd.h> // for read, write

#include "picohttpparser.h" // for phr_header, phr_parse_request

#include "UnixDomainSocket.hpp" // for UnixDomainSocket

static const char* HTTP_HEADER="HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/json\r\n\r\n%s\r\n";

char resp_buffer[1001000];
int response_http(int fd, const char* json) {
  int len = std::sprintf(resp_buffer, HTTP_HEADER, std::strlen(json) + 2, json);
  return write(fd, resp_buffer, len);
}

int main() {
  std::signal(SIGPIPE, SIG_IGN);

  auto local = yonas::UnixDomainSocket::bind("yonas.sock");
  local.listen(100);
  for (;;) {
    auto remote = local.accept();

    char buf[4096];
    const char *method = nullptr;
    const char *path = nullptr;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    while (1) {
      ssize_t rret;
      while ((rret = read(remote.fd, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR);
      if (rret <= 0) {
        std::puts("IOError");
        goto fail;
      }
      prevbuflen = buflen;
      buflen += rret;

      num_headers = sizeof(headers) / sizeof(headers[0]);
      pret = phr_parse_request(buf, buflen, &method, &method_len, &path, &path_len, &minor_version, headers, &num_headers, prevbuflen);
      if (pret > 0) break;
      if (pret == -1) {
        std::puts("ParseError");
        goto fail;
      }
      if (buflen == sizeof(buf)) {
        std::puts("RequestIsTooLongError");
        goto fail;
      }
    }
    response_http(remote.fd, "{\"status\": \"ok\"}");
fail:;
  }
}
