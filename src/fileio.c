#include "kvim.h"
#include <stdio.h>

/* docOpen: open <filename> and store in <doc>
 */
Doc* docOpen (char *filename)
{
	Doc *doc = malloc (sizeof (Doc));
	doc->rows = NULL;
	doc->len = 0;
	doc->crow = 0;
	doc->ccol = 0;
	doc->crcol = doc->ccol;
	doc->modified = 0;
	int fnlen = 0;
	for (int i = 0; filename[i] != '\0'; ++i, ++fnlen)
		;
	doc->fnlen = fnlen;
	doc->filename = malloc (fnlen);
	memcpy (doc->filename, filename, fnlen);
	doc->fd = open (filename, O_RDWR|O_CREAT);
	syscall (SYS_chmod, filename, 0644);
	char buffer;
	Row *row = newRow ();
	int new = 1;
	while (read (doc->fd, &buffer, 1) != 0)
	{
		if (buffer == '\n')
		{
			updateRender (row);
			rowInsert (doc, row, doc->len);
			row = newRow ();
			new = 0;
		}
		else
		{
			charsInsert (row, &buffer, row->len, 1);
			new = 1;
		}
	}
	if (new)
	{
		updateRender (row);
		rowInsert (doc, row, doc->len);
	}

	return doc;
}

/* docSave: save <doc>
 */
int docSave (Doc* doc)
{
	if (!doc->modified)
		return 0;

	lseek (doc->fd, 0, SEEK_SET);
	for (int i = 0; i < doc->len; ++i)
	{
		write (doc->fd, doc->rows[i]->content, doc->rows[i]->len);
		write (doc->fd, "\n", 1);
	}
	doc->modified = 0;

	return 0;
}

/* docClose: close <doc>
 */
int docClose (Doc *doc)
{
	close (doc->fd);
	for (int i = 0; i < doc->len; ++i)
	{
		Row *row = doc->rows[i];
		if (row->content)
			free (row->content);
		if (row->render)
			free (row->render);
		free (row);
	}
	free (doc->rows);
	free (doc);
	return 0;
}
