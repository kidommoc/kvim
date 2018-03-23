#ifndef KVIM_H
#define KVIM_H

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#define STDIN STDIN_FILENO
#define STDOUT STDOUT_FILENO

/* === Key === */
#define ESC 27
#define BACKSPACE 127
#define DEL 1000
#define TAB 9
#define ENTER 10
#define ARROWUP 1001
#define ARROWDOWN 1002
#define ARROWRIGHT 1003
#define ARROWLEFT 1004

/* === Command === */
#define COMMANDNUM 3
#define FORCE 0
#define QUIT 1
#define SAVE 2
int commandList[COMMANDNUM];

/* === Struct === */

/* Row: represent a row in the doc
 * ind: the row index in the doc (based on zero)
 * content: the content of this row
 * len: the length of content
 * render: rendered content with tabs handled
 * rlen: the length of rendered content
 */
typedef struct Row
{
	int ind;
	int len;
	int rlen;
	char *content;
	char *render;
} Row;

/* Doc: represent a document in the editor
 * rows: the content of this doc
 * len: the length of the rows
 * lnlen: the length of len
 * crow, ccol: the position of the cursor in this doc
 * crcol: the column where the cursor *should* be in this rendered row
 * filename: the filename of this doc
 * fnlen: the length of filename
 * fd: the file descriptor of this doc
 * modified: indicates whether this doc is modified but not saved
 */
typedef struct Doc
{
	int len, lnlen;
	int crow, ccol, crcol;
	int fd;
	int fnlen;
	int modified;
	char *filename;
	Row **rows;
} Doc;

/* --- Editor mode --- */
#define MODE_NORMAL 0
#define MODE_INSERT 1

/* Kvim: represent the editor
 * doc: the document
 * cols, rows: the size of the tty
 * cx, cy: the position of the cursor on the screen
 * mode: indicates the mode of the editor
 * termIn, termOut: the origin status of STDIN and STDOUT
 * status: status info
 * stlen: the length of status info
 * inputBuf: input buffer
 * iblen: the length of input buffer
 */
struct Kvim
{
	int cols, rows;
	int cx, cy;
	int mode;
	struct termios termIn, termOut;
	int stlen, iblen;
	char *status;
	int inputBuf[100];
	Doc **doc;
} kvim;

/* === Declaration === */

#define TABSTOP 4
/* --- doc.c --- */
int charsInsert (Row *row, char *chars, int at, int len);
int charsDelete (Row *row, int from, int len);
Row* newRow (void);
int rowInsert (Doc *doc, Row *rows, int at);
int rowDelete (Doc *doc, int from);
int updateRender (Row *row);

/* --- fileio.c --- */
Doc* docOpen (char *filename);
int docSave (Doc *doc);
int docClose (Doc *doc);

/* --- handle.c --- */
int setStatus (const char *buf, int len);
int handleKey (void);

/* --- termlib.c --- */
int termInit (void);
int termExit (void);
int cursorMove (int x, int y);
int getKey (void);
int printContent (Doc *doc);
int printStatus (const char *buf, int len);

/* --- utils.c --- */
#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
int getNumLen (int n);
char* convertNumToStr (int n, int *len);
int convertStrToNum (int* s, int len);
int getRenderCol (const Row *row, int col);
int getContentCol (const Row *row, int rcol);

#endif /* KVIM_H */
