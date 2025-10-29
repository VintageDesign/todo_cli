#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include "todo_list.h"
#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

class TabManager {
private:
  std::vector<std::unique_ptr<TodoList>> tabs;
  int current_tab;

public:
  TabManager();

  void addTab(const std::string &tab_name);
  void deleteCurrentTab();
  void nextTab();
  void prevTab();
  void drawTabs(WINDOW *win);
  TodoList *getCurrentList();
  size_t tabCount() const;
};

#endif
