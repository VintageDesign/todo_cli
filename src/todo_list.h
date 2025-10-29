#ifndef TODO_LIST_H
#define TODO_LIST_H

#include "todo_item.h"
#include <ncurses.h>
#include <string>
#include <vector>

class TodoList {
private:
  std::vector<TodoItem> items;
  int selected;
  std::string filepath;

  std::string getHomeDir();
  void ensureDirectoryExists();
  void sortList();

public:
  TodoList();

  void load();
  void save();
  void addItem(const std::string &text);
  void toggleCompleted();
  void deleteItem();
  void moveUp();
  void moveDown();
  void draw(WINDOW *win);
  size_t size() const;
};

#endif
