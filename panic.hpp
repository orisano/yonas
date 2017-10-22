#pragma once
#include <cstdio>  // for stderr, fprintf, perror
#include <cstdlib> // for exit, EXIT_FAILURE

namespace yonas {
[[noreturn]]
void panic(const char* msg) {
  std::fprintf(stderr, "yonas::");
  std::perror(msg);
  std::exit(EXIT_FAILURE);
}
} // namespace yonas
