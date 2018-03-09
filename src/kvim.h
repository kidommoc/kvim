#ifndef KVIM_H
#define KVIM_H

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

#define STDIN STDIN_FILENO
#define STDOUT STDOUT_FILENO

/* --- Key --- */
#define ESC 27
#define BACKSPACE 127
#define DEL 28
#define TAB 9
#define ENTER 10
#define ARROWUP 11
#define ARROWDOWN 12
#define ARROWRIGHT 13
#define ARROWLEFT 14

/* === Struct === */

/* Row: represent a row in the doc
 * content: the content of this row
 * len: the length of content
 * render: rendered content with tabs handled
 * rlen: the length of rendered content
 */
typedef struct Row
{
	int len;
	int rlen;
	char *content;
	char *render;
} Row;

/* Doc: represent a document in the editor
 * rows: the content of this doc
 * len: the length of the rows
 * crow, ccol: the position of the cursor in this doc
 * fd: the file descriptor of this doc
 * modified: indicates whether this doc is modified but not saved
 */
typedef struct Doc
{
	int len;
	int crow, ccol, crcol;
	int fd;
	int modified;
	char *filename;
	Row *rows;
} Doc;

/* --- Editor mode --- */
#define MODE_NORMAL 0
#define MODE_INSERT 1

/* Kvim: represent the editor
 * doc: the document
 * cols, rows: the size of the tty
 * cx, cy: the position of the cursor on the screen
 * mode: indicates the mode of the editor
 */
struct Kvim
{
	int cols, rows;
	int cx, cy;
	int mode;
	struct termios termIn, termOut;
	char *status;
	Doc *doc;
} kvim;

/* === Declaration === */

/* --- display.c --- */
#define TABSTOP 4
int updateRender (Row *row);
int updateScreen (Doc *doc);

/* --- doc.c --- */
int charsInsert (Row *row, char *chars, int at, int len);
int charsDelete (Row *row, int from, int len);
Row* newRow (void);
int rowsInsert (Doc *doc, Row *rows, int at, int len);
int rowsDelete (Doc *doc, int from, int len);

/* --- fileio.c --- */
int docOpen (Doc *doc, char *filename);
int docSave (Doc *doc);
int docClose (Doc *doc);

/* --- handle.c --- */
int handleKey (void);

/* --- termlib.c --- */
int termInit (void);
int cursorMove (int x, int y);
int getKey (void);

#endif /* KVIM_H */
