#include "config.h"
#include "utils.h"
#include <fstream>

static std::string trim(const std::string &s) {
  const char *ws = " \t\r\n";
  size_t start = s.find_first_not_of(ws);
  if (start == std::string::npos)
    return "";
  size_t end = s.find_last_not_of(ws);
  return s.substr(start, end - start + 1);
}

Config loadConfig() {
  Config config;
  std::ifstream file(utils::getHomeDir() + "/.todo/config");
  if (!file.is_open())
    return config;

  std::string line;
  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#')
      continue;
    size_t eq = line.find('=');
    if (eq == std::string::npos)
      continue;
    std::string key = trim(line.substr(0, eq));
    std::string value = trim(line.substr(eq + 1));
    if (key == "notion_api_key")
      config.notion_api_key = value;
  }
  return config;
}
