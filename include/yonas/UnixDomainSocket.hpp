#pragma once
#include <cstring> // for strcpy

#include <sys/types.h>  // for compatibility
#include <sys/socket.h> // for sockaddr, socket, bind, listen, accept, AF_UNIX, SOCK_STREAM
#include <sys/un.h>     // for sockaddr_un
#include <unistd.h>     // for close, unlink

#include "panic.hpp" // for panic


namespace yonas {
struct UnixDomainSocket {
  const int fd;
  const struct sockaddr_un addr;

  void listen(int backlog) const {
    if (::listen(fd, backlog)) {
      panic("UnixDomainSocket::listen() listen()");
    }
  }

  UnixDomainSocket accept() {
    struct sockaddr_un remote;
    socklen_t len = sizeof(remote);
    int remote_fd = ::accept(fd, static_cast<struct sockaddr*>(static_cast<void*>(&remote)), &len);
    if (remote_fd < 0) {
      panic("UnixDomainSocket::accept() accept()");
    }
    return UnixDomainSocket(remote_fd, remote);
  }

  void close() const {
    if (::close(fd)) {
      panic("UnixDomainSocket::close() close()");
    }
  }

  static UnixDomainSocket bind(const char* path) {
    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
      panic("UnixDomainSocket::bind() socket()");
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    if (path[0] != '@') { // for not abstract socket address case
      std::strcpy(addr.sun_path, path);
      ::unlink(addr.sun_path); // addr.sun_path not found case, ignore
    } else {
      addr.sun_path[0] = '\0';
      std::strcpy(addr.sun_path + 1, path + 1);
    }

    if (::bind(fd, static_cast<struct sockaddr*>(static_cast<void*>(&addr)), sizeof(addr))) {
      panic("UnixDomainSocket::bind() bind()");
    }

    return UnixDomainSocket(fd, addr);
  }
 private:
  UnixDomainSocket(int fd, struct sockaddr_un addr) : fd(fd), addr(addr) {}
};
} // namespace yonas
