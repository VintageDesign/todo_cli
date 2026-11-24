add_executable(todo)

target_sources(todo PRIVATE main.cpp todo_list.cpp input_dialog.cpp
                            tab_manager.cpp config.cpp notion_client.cpp
                            notion_todo_list.cpp)

include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Curses REQUIRED)
find_package(CURL REQUIRED)
target_link_libraries(todo PRIVATE ${CURSES_LIBRARIES} CURL::libcurl
                                   nlohmann_json::nlohmann_json)
target_include_directories(todo PRIVATE ${CURSES_INCLUDE_DIR})

install(TARGETS todo DESTINATION bin)
