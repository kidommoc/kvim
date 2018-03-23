# Kvim Text Editor

Kvim is a *vim-like text editor*. It only takes those function used most (by me) in vim and therefore light-weight.

The codes are usually on dev branch.

## Installation

Just run `install.sh` to install.

	./install.sh

After that, a executable file called `kvim` will be created as the program.

## Feature

- Two Mode: Normal, Insert
- Embedded kvim-shell to save and quit
- Line numbers displaying supported

The following operations are implemented in kvim v1.1:

- Normal mode:
 - `h`/`j`/`k`/`l`: cursor moving (left, down, up, right)
 - `i`: insert (to Insert mode)
 - `a`: append (to Insert mode)
 - `x`: delete
 - `s`: delete (to Insert mode)
 - `o`: new next line (to Insert mode)
 - `O`: new previous line (to Insert mode)
 - `dd`: delete the whole line
 - `:`: start kvim-shell

- Insert mode:
 - `ESC`: quit (to Normal mode)

Note that it's supported to add number before `h`, `j`, `k`, `l`, `x`, `s` and `dd` to indicate the length to move/delete. For instance, enter `12x` will cause a 12-char deletion.

## Kvim-shell

It's just like a common shell, except the input buffer can't be longer than the width of the screen. Here follows the supported command:

- `w`: save the doc
- `w filename`: save the doc as `filename`
- `q`: quit kvim (wouldn't work with not saved doc)
- `!`: make other command mandatory

Command can be conbined like `wq` or `wq filename`.
