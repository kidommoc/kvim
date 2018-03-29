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
 - `gg`: jump to the first line of the doc
 - `G`: jump to the last line of the doc
 - `<num>G`: jump to the `<num>` line of the doc
 - `0`: jump to the beginning of the row
 - `^`: jump to the first non-empty char of the row
 - `$`: jump to the end of the row
 - `f<char>`: jump at next \<char\> of the row
 - `t<char>`: jump at previous \<char\> of the row
 - `F<char>`: jump to next \<char\> of the row
 - `T<char>`: jump to previous \<char\> of the row
 - `n`: jump to next search result of the doc
 - `N`: jump to previous search result of the doc
 - `r<char>`: replace this char by \<char\>
 - `R`: switch to Replace mode
 - `^u`: go down half a page
 - `^d`: go up half a page
 - `^G`: display doc property
 - `:`: start kvim-shell (command mode)
 - `/`: start kvim-shell (search mode)

- Insert mode:
 - `ESC`: quit (to Normal mode)

- Replace mode:
 - `ESC`: quit (to Normal mode)

Note that it's supported to add number before `h`, `j`, `k`, `l`, `x`, `s`, `dd`, `f`, `t`, `F`, and `T` to indicate the length to move/delete. For instance, enter `12x` will cause a 12-char deletion.

## Kvim-shell

It's just like a common shell, except the input buffer can't be longer than the width of the screen.

Here follows the supported command in command mode:

- `w`: save the doc
- `w filename`: save the doc as `filename`
- `q`: quit kvim (wouldn't work with not saved doc)
- `!`: make other command mandatory

Command can be conbined like `wq` or `wq filename`.

In search mode, just type the string to search and press ENTER to search. After that, `n` and `N` in normal mode can help jump to the next/previous result. SPACE is also supported.
