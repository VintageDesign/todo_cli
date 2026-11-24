#include "notion_client.h"
#include <cctype>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
const char *kApiBase = "https://api.notion.com/v1";
const char *kNotionVersion = "2022-06-28";

size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  static_cast<std::string *>(userdata)->append(ptr, size * nmemb);
  return size * nmemb;
}
} // namespace

NotionClient::NotionClient(const std::string &api_key) : api_key(api_key) {}

bool NotionClient::request(const std::string &method, const std::string &path,
                           const std::string &body, std::string &response) {
  response.clear();
  CURL *curl = curl_easy_init();
  if (!curl) {
    last_error = "Failed to initialise curl";
    return false;
  }

  std::string url = std::string(kApiBase) + path;
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers,
                              ("Authorization: Bearer " + api_key).c_str());
  headers = curl_slist_append(
      headers, (std::string("Notion-Version: ") + kNotionVersion).c_str());
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
  if (!body.empty())
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

  CURLcode res = curl_easy_perform(curl);
  long status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    last_error = std::string("Network error: ") + curl_easy_strerror(res);
    return false;
  }
  if (status < 200 || status >= 300) {
    last_error = "Notion API error " + std::to_string(status);
    json err = json::parse(response, nullptr, false);
    if (!err.is_discarded() && err.contains("message") &&
        err["message"].is_string())
      last_error += ": " + err["message"].get<std::string>();
    return false;
  }
  last_error.clear();
  return true;
}

bool NotionClient::fetchTodos(const std::string &page_id,
                              std::vector<TodoItem> &out) {
  std::vector<TodoItem> result;
  std::string cursor;
  do {
    std::string path = "/blocks/" + page_id + "/children?page_size=100";
    if (!cursor.empty())
      path += "&start_cursor=" + cursor;

    std::string response;
    if (!request("GET", path, "", response))
      return false;

    json data = json::parse(response, nullptr, false);
    if (data.is_discarded() || !data.contains("results")) {
      last_error = "Unexpected response from Notion";
      return false;
    }

    for (const auto &block : data["results"]) {
      if (block.value("type", "") != "to_do")
        continue;
      const json &todo = block["to_do"];
      TodoItem item;
      item.id = block.value("id", "");
      item.completed = todo.value("checked", false);
      for (const auto &rt : todo.value("rich_text", json::array()))
        item.text += rt.value("plain_text", "");
      result.push_back(item);
    }

    cursor = (data.value("has_more", false) && data["next_cursor"].is_string())
                 ? data["next_cursor"].get<std::string>()
                 : "";
  } while (!cursor.empty());

  out = std::move(result);
  return true;
}

bool NotionClient::setChecked(const std::string &block_id, bool checked) {
  json body = {{"to_do", {{"checked", checked}}}};
  std::string response;
  return request("PATCH", "/blocks/" + block_id, body.dump(), response);
}

bool NotionClient::appendTodo(const std::string &page_id,
                              const std::string &text, std::string &new_id) {
  json body = {
      {"children",
       {{{"object", "block"},
         {"type", "to_do"},
         {"to_do",
          {{"rich_text", {{{"type", "text"}, {"text", {{"content", text}}}}}},
           {"checked", false}}}}}}};
  std::string response;
  if (!request("PATCH", "/blocks/" + page_id + "/children", body.dump(),
               response))
    return false;

  json data = json::parse(response, nullptr, false);
  if (data.is_discarded() || !data.contains("results") ||
      data["results"].empty()) {
    last_error = "Unexpected response from Notion";
    return false;
  }
  // Notion returns the page's children; the appended block is last.
  new_id = data["results"].back().value("id", "");
  return true;
}

bool NotionClient::deleteBlock(const std::string &block_id) {
  std::string response;
  return request("DELETE", "/blocks/" + block_id, "", response);
}

std::string NotionClient::parsePageId(const std::string &ref) {
  std::string s = ref.substr(0, ref.find_first_of("?#"));
  std::string compact;
  for (char c : s)
    if (c != '-' && !std::isspace(static_cast<unsigned char>(c)))
      compact += c;
  if (compact.size() < 32)
    return "";
  std::string id = compact.substr(compact.size() - 32);
  for (char c : id)
    if (!std::isxdigit(static_cast<unsigned char>(c)))
      return "";
  return id;
}
