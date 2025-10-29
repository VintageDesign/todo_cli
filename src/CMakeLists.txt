add_executable(todo)

target_sources(todo PRIVATE main.cpp todo_list.cpp input_dialog.cpp)

find_package(Curses REQUIRED)
target_link_libraries(todo PRIVATE ${CURSES_LIBRARIES})
target_include_directories(todo PRIVATE ${CURSES_INCLUDE_DIR})

install(TARGETS todo DESTINATION bin)
