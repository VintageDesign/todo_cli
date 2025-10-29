#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H

#include <ncurses.h>
#include <string>

std::string getInput(WINDOW *win, const std::string &prompt);

#endif
