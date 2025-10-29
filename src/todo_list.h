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
  int scroll_offset;
  std::string filepath;
  std::string name;

  void sortList();

public:
  TodoList();
  TodoList(const std::string &tab_name);

  void load();
  void save();
  void addItem(const std::string &text);
  void toggleCompleted();
  void deleteItem();
  void moveUp();
  void moveDown();
  void draw(WINDOW *win);
  size_t size() const;
  std::string getName() const;
};

#endif
