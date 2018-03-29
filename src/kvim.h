#ifndef KVIM_H
#define KVIM_H

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <signal.h>

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
#define CTRL_D 4
#define CTRL_G 7
#define CTRL_U 21

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
#define MODE_REPLACE 2

/* Kvim: represent the editor
 * doc: the document
 * cols, rows: the size of the tty
 * cx, cy: the position of the cursor on the screen
 * mode: indicates the mode of the editor
 * termIn, termOut: the origin status of STDIN and STDOUT
 * status: status info
 * stlen: the length of status info
 */
struct Kvim
{
	int cols, rows;
	int cx, cy;
	int mode;
	struct termios termIn, termOut;
	int stlen;
	char *status;
	Doc **doc;
} kvim;

/* InputBuf
 * inputBuf: input buffer
 * iblen: the length of input buffer
 */
struct InputBuf
{
	int len;
	int inputBuf[100];
} ib;

/* SearchBuf
 * searchBuf: search buffer
 * sblen: the length of search buffer
 */
struct SearchBuf
{
	int len;
	char *searchBuf;
} sb;

/* Clipboard
 * clipbuf: clip buffer
 * len: the length of clip buffer
 * type: the type of clip buffer (CHAR or ROW)
 */
#define CT_CHAR 0
#define CT_ROW 1
struct ClipBoard
{
	int len, type;
	union ClipBuf
	{
		char *c;
		Row **r;
	} clipBuf;
} cb;

/* === Declaration === */

/* --- cursor.c --- */
int cursorUp (Doc *doc);
int cursorDown (Doc *doc);
int cursorRight (Doc *doc);
int cursorLeft (Doc *doc);

#define TABSTOP 4
/* --- doc.c --- */
int getRenderCol (const Row *row, int col);
int getContentCol (const Row *row, int rcol);
int charsInsert (Row *row, char *chars, int at, int len);
int charsDelete (Row *row, int from, int len);
Row* newRow (void);
Row *cpyRow (const Row *from);
int rowInsert (Doc *doc, Row *rows, int at);
int rowDelete (Doc *doc, int from);
int updateRender (Row *row);

/* --- fileio.c --- */
Doc* docOpen (char *filename);
int docSave (Doc *doc);
int docClose (Doc *doc);

/* --- insert.c --- */
int handleInsert (int c);

/* --- normal.c --- */
int handleNormal (int c);

/* --- replace.c --- */
int handleReplace (int c);

/* --- search.c --- */
int searchRowForward (Row *row, int start, char dist);
int searchRowBack (Row *row, int start, char dist);
int searchDocForward (Doc *doc, char *dist, int len, int *row, int *col);
int searchDocBack (Doc *doc, char *dist, int len, int *row, int *col);

/* --- shell.c --- */
int shellCommand (void);
int shellSearch (void);

/* --- termlib.c --- */
int termInit (void);
int termExit (void);
int cursorMove (int x, int y);
int getKey (void);
int printContent (Doc *doc);
int printStatus (char *buf, int len);

/* --- utils.c --- */
#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
int getNumLen (int n);
char* convertNumToStr (int n, int *len);
int convertStrToNum (int* s, int len);
int setStatus (const char *buf, int len);
int appendInputBuf (int c);
int getIbNum (void);
int addClipboardChar (Row *row, int from, int len);
int addClipboardRow (Doc *doc, int from, int len);
int compareStr (char *str1, char *str2, int len);

#endif /* KVIM_H */
