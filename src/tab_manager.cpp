#include "tab_manager.h"
#include "notion_client.h"
#include "notion_todo_list.h"
#include "utils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TabManager::TabManager() : current_tab(0), config(loadConfig()) {
  utils::ensureDirectoryExists();
  for (const auto &entry :
       fs::directory_iterator(utils::getHomeDir() + "/.todo/")) {
    if (!fs::is_regular_file(entry.status()))
      continue;
    std::string filename = entry.path().filename();
    // The config file and dotfiles are not lists.
    if (filename == "config" || filename[0] == '.')
      continue;

    if (entry.path().extension() == ".notion") {
      std::ifstream file(entry.path());
      std::string page_ref;
      std::getline(file, page_ref);
      tabs.push_back(std::make_unique<NotionTodoList>(
          entry.path().stem(), NotionClient::parsePageId(page_ref),
          config.notion_api_key));
    } else {
      tabs.push_back(std::make_unique<TodoList>(filename));
    }
  }
  if (tabs.empty()) {
    tabs.push_back(std::make_unique<TodoList>("List"));
  }
}

bool TabManager::addNotionTab(const std::string &tab_name,
                              const std::string &page_ref) {
  std::string page_id = NotionClient::parsePageId(page_ref);
  if (tab_name.empty() || page_id.empty())
    return false;

  std::ofstream file(utils::getHomeDir() + "/.todo/" + tab_name + ".notion");
  if (!file.is_open())
    return false;
  file << page_id << "\n";
  file.close();

  tabs.push_back(std::make_unique<NotionTodoList>(tab_name, page_id,
                                                  config.notion_api_key));
  current_tab = tabs.size() - 1;
  return true;
}

void TabManager::reloadCurrent() {
  if (TodoList *list = getCurrentList())
    list->load();
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
