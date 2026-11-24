# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview
`todo` is a minimal ncurses TUI todo list written in C++17. There is no test suite or lint config.

## Build & run
Requires ncurses and libcurl dev headers (`sudo apt install libncurses5-dev libncursesw5-dev libcurl4-openssl-dev`). nlohmann/json is pulled by CMake `FetchContent` in `src/CMakeLists.txt` (first configure needs network). Run from the repo root (the top-level `CMakeLists.txt` just adds `src/`):

    cmake -B build
    cmake --build build                    # binary at build/bin/todo
    cmake --build build --target install   # installs `todo` to CMAKE_INSTALL_PREFIX/bin (default /usr/local, may need sudo)

New `.cpp` files must be added to `target_sources` in `src/CMakeLists.txt`.

Running the binary takes over the terminal and reads/writes the real `~/.todo/` directory. To avoid touching the user's lists, run with a throwaway home: `HOME=$(mktemp -d) ./build/bin/todo`.

## Architecture
- `main.cpp` — ncurses setup and the single key-dispatch loop: each iteration redraws everything (`box` → `TabManager::drawTabs` → `TodoList::draw`) then blocks on `wgetch`. All keybindings live here; the help line listing them is hard-coded at the bottom of `TodoList::draw` and must be kept in sync.
- `TabManager` — owns `std::vector<std::unique_ptr<TodoList>>` and the `Config` (`config.h`, parsed from `~/.todo/config`). On construction, every regular file in `~/.todo/` becomes a tab in directory iteration order, except `config` and dotfiles: `*.notion` files become `NotionTodoList` (tab name = stem), everything else a file-backed `TodoList` (tab name = filename). If no tabs result it creates one named `List`.
- `TodoList` — one tab: items, selection, scroll offset, rendering. `load`/`save`/`addItem`/`toggleCompleted`/`deleteItem` are virtual. In the base class every mutation re-sorts (incomplete before completed) and immediately `save()`s; there is no explicit save step. A non-empty `error_message` replaces the dashed line under the title.
- `NotionTodoList` — subclass backed by a Notion page via `NotionClient` (libcurl + nlohmann/json, synchronous, Notion-Version `2022-06-28`). Each edit calls the API first and only mutates local `items` on success; failures go to `error_message`. `save()` is a no-op. Only top-level `to_do` blocks are synced; `TodoItem::id` holds the block ID.
- `input_dialog` — `getInput()` modal text prompt (255-char buffer) drawn in a temporary window.
- `utils.h` — header-only `getHomeDir()` and `ensureDirectoryExists()` (creates `~/.todo`).

## Persistence format
Each file tab is a plain-text file `~/.todo/<tab name>`, one item per line: `[ ] text` or `[x] text`. Lines without the `[?] ` prefix load as incomplete items; blank lines are skipped.

A Notion tab is `~/.todo/<tab name>.notion` whose first line is the page ID or URL (`NotionClient::parsePageId` normalises it). `~/.todo/config` holds `key=value` lines; currently only `notion_api_key` is read.

## Known quirks
- Deleting a tab (`x`) only removes it from memory — its file in `~/.todo/` is not deleted, so it reappears on next launch.
- Tab names are used directly as filenames with no sanitization. A plain list named `config` is hidden because that name is reserved.
- `TodoList()` default constructor (file `~/.todo/list`) is unused.
