#include "input_dialog.h"

std::string getInput(WINDOW *win, const std::string &prompt) {
  int height, width;
  getmaxyx(win, height, width);

  WINDOW *input_win = newwin(5, width - 4, height / 2 - 2, 2);
  box(input_win, 0, 0);
  mvwprintw(input_win, 1, 2, "%s", prompt.c_str());
  mvwprintw(input_win, 3, 2, "> ");
  wrefresh(input_win);

  echo();
  curs_set(1);

  char buffer[256];
  mvwgetnstr(input_win, 3, 4, buffer, 255);

  noecho();
  curs_set(0);

  delwin(input_win);
  return std::string(buffer);
}
