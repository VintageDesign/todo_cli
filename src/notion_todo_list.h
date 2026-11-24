#ifndef NOTION_TODO_LIST_H
#define NOTION_TODO_LIST_H

#include "notion_client.h"
#include "todo_list.h"

// A tab backed by the to_do blocks of a Notion page. Every edit is sent to
// Notion first; local state only changes if the API call succeeds.
class NotionTodoList : public TodoList {
public:
  NotionTodoList(const std::string &tab_name, const std::string &page_id,
                 const std::string &api_key);

  void load() override;
  void save() override {}
  void addItem(const std::string &text) override;
  void toggleCompleted() override;
  void deleteItem() override;
  bool isRemote() const override { return true; }

private:
  std::string page_id;
  bool has_key;
  NotionClient client;

  bool validSelection() const;
};

#endif
