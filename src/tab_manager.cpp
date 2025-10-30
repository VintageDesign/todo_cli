#include "tab_manager.h"
#include "utils.h"
#include <filesystem>

namespace fs = std::filesystem;

TabManager::TabManager() : current_tab(0) {

  utils::ensureDirectoryExists();
  if (fs::is_empty(utils::getHomeDir() + "/.todo/")) {
    tabs.push_back(std::make_unique<TodoList>("List"));
  } else {
    for (const auto &entry :
         fs::directory_iterator(utils::getHomeDir() + "/.todo/")) {
      // Check if the entry is a regular file
      if (fs::is_regular_file(entry.status())) {
        tabs.push_back(std::make_unique<TodoList>(entry.path().filename()));
      }
    }
  }
}

void TabManager::addTab(const std::string &tab_name) {
  tabs.push_back(std::unique_ptr<TodoList>(new TodoList(tab_name)));
  current_tab = tabs.size() - 1;
}

void TabManager::deleteCurrentTab() {
  if (tabs.size() <= 1) {
    return;
  }

  tabs.erase(tabs.begin() + current_tab);

  if (current_tab >= tabs.size()) {
    current_tab = tabs.size() - 1;
  }
}

void TabManager::nextTab() {
  if (tabs.size() > 0) {
    current_tab = (current_tab + 1) % tabs.size();
  }
}

void TabManager::prevTab() {
  if (tabs.size() > 0) {
    current_tab = (current_tab - 1 + tabs.size()) % tabs.size();
  }
}

void TabManager::drawTabs(WINDOW *win) {
  int height, width;
  getmaxyx(win, height, width);

  wmove(win, 0, 2);
  for (size_t i = 0; i < tabs.size(); i++) {
    if (i == current_tab) {
      wattron(win, A_REVERSE);
    }

    std::string tab_label = " " + tabs[i]->getName() + " ";
    wprintw(win, "%s", tab_label.c_str());

    if (i == current_tab) {
      wattroff(win, A_REVERSE);
    }

    wprintw(win, " ");
  }
}

TodoList *TabManager::getCurrentList() {
  if (tabs.empty())
    return nullptr;
  return tabs[current_tab].get();
}

size_t TabManager::tabCount() const { return tabs.size(); }
