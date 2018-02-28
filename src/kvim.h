#ifndef KVIM_H
#define KVIM_H

/* === Struct === */

/* Row: represent a row in the doc
 * content: the content of this row
 * len: the length of content
 * render: rendered content with tabs handled
 * rlen: the length of rendered content
 */
typedef struct Row
{
	char *content;
	int len;
	char *render;
	int rlen;
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
	Row *rows;
	int len;
	int crow, ccol;
	int fd;
	int modified;
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
	Doc *doc;
	int cols, rows;
	int cx, cy;
	int mode;
} kvim;

#endif /* KVIM_H */
