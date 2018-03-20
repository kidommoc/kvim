# Kvim Text Editor

Kvim is a *vim-like text editor*. It only takes those function used most (by me) in vim and therefore light-weight.

The codes are usually on dev branch.

## Installation

Just run `install.sh` to install.

	./install.sh

After that, a executable file called `kvim` will be created as the program.

## Feature

The following operations/features are implemented in kvim v1.0:

- Mode: Normal and Insert
- Operation: h, j, k, l in Normal mode as moving cursor left, down, up, right (in fact, ARROW UP/DOWN/LEFT/RIGHT are also supported)
- Operation: x, o in Normal mode indicating delete this char and create a new row respectively
- Operation: i in Normal mode to switch mode to Insert
- Operation: ESC in Insert mode to switch mode to Normal
- Operation: ARROW UP/DOWN/LEFT/RIGHT in Insert mode to move cursor
- Operation: `:` in Normal mode to get into kvim-shell
- Operation: ESC in kvim-shell to exit shell and return to Normal mode
- Command: `w`, `q`, `!` indicating saving, quiting, and mandatory operation
