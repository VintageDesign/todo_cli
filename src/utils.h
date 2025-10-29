#pragma once

#include <pwd.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace utils {

inline std::string getHomeDir() {
  const char *home = getenv("HOME");
  if (home == nullptr) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }
  return std::string(home);
}

inline void ensureDirectoryExists() {
  std::string dir = getHomeDir() + "/.todo";
  mkdir(dir.c_str(), 0755);
}
} // namespace utils
