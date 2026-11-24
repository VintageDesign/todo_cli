# Todo CLI
Just a minimalist todo list tool for the command line.

## Building
Install build deps:
```
sudo apt install libncurses5-dev libncursesw5-dev libcurl4-openssl-dev
```

Then run the build and install it. (The first configure downloads
[nlohmann/json](https://github.com/nlohmann/json), so it needs network access.)
```
cmake -B build
cmake --build build --target install
```

## Usage
To launch the tool you can simplily call `todo`. The all commands are visible on the screen to allow
you to add/remove/complete items, and add/remove tabs. 

Each list you create is saved in `~/.todo/` as a separate text file automatically. 

## Notion lists
A tab can show the checklist (to-do blocks) of a Notion page. Checking, adding and deleting items
on that tab updates the page in Notion straight away, and `r` re-fetches it.

1. Create an internal integration at https://www.notion.so/profile/integrations and copy its token.
2. On the Notion page, open the `...` menu → **Connections** and add your integration.
3. Put the token in `~/.todo/config` (and consider `chmod 600 ~/.todo/config`):
   ```
   notion_api_key=ntn_...
   ```
4. In `todo`, press `n`, enter a tab name, then paste the page URL or ID.

This saves a file `~/.todo/<name>.notion` containing the page ID. You can also create that file by
hand. Only to-do blocks directly on the page are shown; nested blocks and other content are ignored.
`~/.todo/config` and dotfiles in `~/.todo/` are never shown as tabs.
