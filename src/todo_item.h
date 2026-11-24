#ifndef TODO_ITEM_H
#define TODO_ITEM_H

#include <string>

struct TodoItem {
  std::string text;
  bool completed;
  std::string id; // Notion block ID; empty for file-backed lists
};

#endif
