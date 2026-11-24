#include "input_dialog.h"
#include "tab_manager.h"
#include "todo_list.h"
#include <curl/curl.h>
#include <ncurses.h>

int main() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  int height, width;
  getmaxyx(stdscr, height, width);

  WINDOW *main_win = newwin(height, width, 0, 0);
  keypad(main_win, TRUE);

  TabManager tabManager;

  bool running = true;
  while (running) {
    TodoList *currentList = tabManager.getCurrentList();
    if (!currentList)
      break;

    werase(main_win);
    box(main_win, 0, 0);
    tabManager.drawTabs(main_win);
    currentList->draw(main_win);

    int ch = wgetch(main_win);
    switch (ch) {
    case 'q':
    case 'Q':
      running = false;
      break;

    case 'a':
    case 'A': {
      std::string text = getInput(main_win, "Enter new item:");
      currentList->addItem(text);
      break;
    }

    case 'd':
    case 'D':
      if (currentList->size() > 0) {
        currentList->deleteItem();
      }
      break;

    case ' ':
      currentList->toggleCompleted();
      break;

    case KEY_UP:
    case 'k':
      currentList->moveUp();
      break;

    case KEY_DOWN:
    case 'j':
      currentList->moveDown();
      break;

    case 't':
    case 'T': {
      std::string tab_name = getInput(main_win, "Enter new tab name:");
      if (!tab_name.empty()) {
        tabManager.addTab(tab_name);
      }
      break;
    }

    case 'n':
    case 'N': {
      std::string tab_name = getInput(main_win, "Enter Notion tab name:");
      if (tab_name.empty())
        break;
      std::string page_ref = getInput(main_win, "Notion page URL or ID:");
      tabManager.addNotionTab(tab_name, page_ref);
      break;
    }

    case 'r':
    case 'R':
      tabManager.reloadCurrent();
      break;

    case 'x':
    case 'X':
      if (tabManager.tabCount() > 1) {
        tabManager.deleteCurrentTab();
      }
      break;

    case KEY_LEFT:
    case 'h':
      tabManager.prevTab();
      break;

    case KEY_RIGHT:
    case 'l':
      tabManager.nextTab();
      break;
    }
  }

  delwin(main_win);
  endwin();
  curl_global_cleanup();

  return 0;
}
