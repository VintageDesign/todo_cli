# Todo CLI
Just a minimalist todo list tool for the command line.

## Building
Install build deps:
```
sudo apt install libncurses5-dev libncursesw5-dev
```

Then run the build and install it.
```
cmake -B build
cmake --build build --target install
```

## Usage
To launch the tool you can simplily call `todo`. The all commands are visible on the screen to allow
you to add/remove/complete items, and add/remove tabs. 

Each list you create is saved in `~/.todo/` as a separate text file automatically. 
