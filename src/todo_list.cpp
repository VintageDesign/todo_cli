#include "todo_list.h"
#include "todo_item.h"
#include <algorithm>
#include <fstream>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

std::string TodoList::getHomeDir() {
  const char *home = getenv("HOME");
  if (home == nullptr) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }
  return std::string(home);
}

void TodoList::ensureDirectoryExists() {
  std::string dir = getHomeDir() + "/.todo";
  mkdir(dir.c_str(), 0755);
}

TodoList::TodoList() : selected(0), scroll_offset(0) {
  filepath = getHomeDir() + "/.todo/list";
  ensureDirectoryExists();
  load();
}

void TodoList::sortList() {
  std::sort(items.begin(), items.end(),
            [](const TodoItem &a, const TodoItem &b) {
              return !a.completed && b.completed;
            });
}

void TodoList::load() {
  items.clear();
  std::ifstream file(filepath);
  if (!file.is_open())
    return;

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    TodoItem item;
    if (line[0] == '[' && line.size() > 3) {
      item.completed = (line[1] == 'x' || line[1] == 'X');
      item.text = line.substr(4);
    } else {
      item.completed = false;
      item.text = line;
    }
    items.push_back(item);
  }
  file.close();
}

void TodoList::save() {
  std::ofstream file(filepath);
  if (!file.is_open())
    return;

  for (const auto &item : items) {
    file << "[" << (item.completed ? "x" : " ") << "] " << item.text << "\n";
  }
  file.close();
}

void TodoList::addItem(const std::string &text) {
  if (!text.empty()) {
    items.push_back({text, false});
    sortList();
    save();
  }
}

void TodoList::toggleCompleted() {
  if (!items.empty() && selected >= 0 && selected < items.size()) {
    items[selected].completed = !items[selected].completed;
    sortList();
    save();
  }
}

void TodoList::deleteItem() {
  if (!items.empty() && selected >= 0 && selected < items.size()) {
    items.erase(items.begin() + selected);
    if (selected >= items.size() && selected > 0) {
      selected--;
    }
    sortList();
    save();
  }
}

void TodoList::moveUp() {
  if (selected > 0)
    selected--;
}

void TodoList::moveDown() {
  if (selected < items.size() - 1)
    selected++;
}

void TodoList::draw(WINDOW *win) {
  werase(win);
  box(win, 0, 0);

  int height, width;
  getmaxyx(win, height, width);

  mvwprintw(win, 1, 2, "Todo List");
  mvwprintw(win, 2, 2, "----------------------------------------");

  if (items.empty()) {
    mvwprintw(win, 4, 2, "No items. Press 'a' to add one.");
  } else {
    int start_line = 4;
    int visible_lines = height - 6;

    // Adjust scroll offset to keep selected item visible
    if (selected < scroll_offset) {
      scroll_offset = selected;
    } else if (selected >= scroll_offset + visible_lines) {
      scroll_offset = selected - visible_lines + 1;
    }

    // Draw visible items
    for (int i = 0; i < visible_lines && scroll_offset + i < items.size();
         i++) {
      int item_index = scroll_offset + i;

      if (item_index == selected) {
        wattron(win, A_REVERSE);
      }

      std::string status = items[item_index].completed ? "[x]" : "[ ]";
      std::string display = status + " " + items[item_index].text;

      if (display.length() > width - 4) {
        display = display.substr(0, width - 7) + "...";
      }

      mvwprintw(win, start_line + i, 2, "%s", display.c_str());

      if (item_index == selected) {
        wattroff(win, A_REVERSE);
      }
    }

    // Show scroll indicator if needed
    if (items.size() > visible_lines) {
      mvwprintw(win, height - 3, width - 15, "[%d/%d]", selected + 1,
                (int)items.size());
    }
  }

  mvwprintw(win, height - 2, 2, "a:Add d:Delete Space:Toggle q:Quit");
  wrefresh(win);
}

size_t TodoList::size() const { return items.size(); }
