#include "notion_todo_list.h"

NotionTodoList::NotionTodoList(const std::string &tab_name,
                               const std::string &page_id,
                               const std::string &api_key)
    : TodoList(tab_name, true), page_id(page_id), has_key(!api_key.empty()),
      client(api_key) {
  load();
}

bool NotionTodoList::validSelection() const {
  return !items.empty() && selected >= 0 && selected < (int)items.size();
}

void NotionTodoList::load() {
  if (!has_key) {
    error_message = "Set notion_api_key in ~/.todo/config";
    return;
  }
  if (page_id.empty()) {
    error_message = "Invalid Notion page ID in " + name + ".notion";
    return;
  }

  std::vector<TodoItem> fetched;
  if (!client.fetchTodos(page_id, fetched)) {
    error_message = client.lastError();
    return;
  }
  items = std::move(fetched);
  error_message.clear();
  sortList();
  clampSelection();
}

void NotionTodoList::addItem(const std::string &text) {
  if (text.empty() || !has_key)
    return;
  std::string id;
  if (!client.appendTodo(page_id, text, id)) {
    error_message = client.lastError();
    return;
  }
  error_message.clear();
  items.push_back({text, false, id});
  sortList();
}

void NotionTodoList::toggleCompleted() {
  if (!validSelection() || !has_key)
    return;
  TodoItem &item = items[selected];
  if (!client.setChecked(item.id, !item.completed)) {
    error_message = client.lastError();
    return;
  }
  error_message.clear();
  item.completed = !item.completed;
  sortList();
}

void NotionTodoList::deleteItem() {
  if (!validSelection() || !has_key)
    return;
  if (!client.deleteBlock(items[selected].id)) {
    error_message = client.lastError();
    return;
  }
  error_message.clear();
  items.erase(items.begin() + selected);
  clampSelection();
}
