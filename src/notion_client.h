#ifndef NOTION_CLIENT_H
#define NOTION_CLIENT_H

#include "todo_item.h"
#include <string>
#include <vector>

// Minimal synchronous client for the Notion blocks API.
// Every call returns false on failure and sets lastError().
class NotionClient {
public:
  explicit NotionClient(const std::string &api_key);

  // Top-level to_do blocks of a page, in page order.
  bool fetchTodos(const std::string &page_id, std::vector<TodoItem> &out);
  bool setChecked(const std::string &block_id, bool checked);
  bool appendTodo(const std::string &page_id, const std::string &text,
                  std::string &new_id);
  bool deleteBlock(const std::string &block_id);

  const std::string &lastError() const { return last_error; }

  // Accepts a raw page ID (with or without dashes) or a notion.so URL.
  // Returns the 32-char hex ID, or "" if none can be found.
  static std::string parsePageId(const std::string &ref);

private:
  std::string api_key;
  std::string last_error;

  bool request(const std::string &method, const std::string &path,
               const std::string &body, std::string &response);
};

#endif
