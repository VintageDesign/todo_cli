#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include "config.h"
#include "todo_list.h"
#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

class TabManager {
private:
  std::vector<std::unique_ptr<TodoList>> tabs;
  int current_tab;
  Config config;

public:
  TabManager();

  void addTab(const std::string &tab_name);
  // Writes ~/.todo/<tab_name>.notion and opens it. Returns false if
  // page_ref is not a valid Notion page URL or ID.
  bool addNotionTab(const std::string &tab_name, const std::string &page_ref);
  void reloadCurrent();
  void deleteCurrentTab();
  void nextTab();
  void prevTab();
  void drawTabs(WINDOW *win);
  TodoList *getCurrentList();
  size_t tabCount() const;
};

#endif
