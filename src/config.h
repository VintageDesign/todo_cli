#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Settings read from ~/.todo/config (key=value lines, '#' comments).
struct Config {
  std::string notion_api_key;
};

Config loadConfig();

#endif
