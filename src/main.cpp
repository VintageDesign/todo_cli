#include "input_dialog.h"
#include "todo_list.h"
#include <ncurses.h>

int main() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  int height, width;
  getmaxyx(stdscr, height, width);

  WINDOW *main_win = newwin(height, width, 0, 0);
  keypad(main_win, TRUE);

  TodoList todoList;

  bool running = true;
  while (running) {
    todoList.draw(main_win);

    int ch = wgetch(main_win);
    switch (ch) {
    case 'q':
    case 'Q':
      running = false;
      break;

    case 'a':
    case 'A': {
      std::string text = getInput(main_win, "Enter new item:");
      todoList.addItem(text);
      break;
    }

    case 'd':
    case 'D':
      if (todoList.size() > 0) {
        todoList.deleteItem();
      }
      break;

    case ' ':
      todoList.toggleCompleted();
      break;

    case KEY_UP:
    case 'k':
      todoList.moveUp();
      break;

    case KEY_DOWN:
    case 'j':
      todoList.moveDown();
      break;
    }
  }

  delwin(main_win);
  endwin();

  return 0;
}
