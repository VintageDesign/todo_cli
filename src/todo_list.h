#ifndef TODO_LIST_H
#define TODO_LIST_H

#include "todo_item.h"
#include <ncurses.h>
#include <string>
#include <vector>

class TodoList {
protected:
  std::vector<TodoItem> items;
  int selected;
  int scroll_offset;
  std::string filepath;
  std::string name;
  std::string error_message; // shown under the title when non-empty

  void sortList();
  void clampSelection();

  // For subclasses that are not backed by a ~/.todo/ file.
  TodoList(const std::string &tab_name, bool);

public:
  TodoList();
  TodoList(const std::string &tab_name);
  virtual ~TodoList() = default;

  virtual void load();
  virtual void save();
  virtual void addItem(const std::string &text);
  virtual void toggleCompleted();
  virtual void deleteItem();
  virtual bool isRemote() const { return false; }
  void moveUp();
  void moveDown();
  void draw(WINDOW *win);
  size_t size() const;
  std::string getName() const;
};

#endif
