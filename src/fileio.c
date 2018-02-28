#include "kvim.h"

/* docOpen: open <filename> and store in <doc>
 */
int docOpen (Doc* doc, char *filename)
{
	doc->fd = open (filename, O_RDWR|O_CREAT);
	char *buffer = malloc (1);
	Row *row = malloc (sizeof (Row));
	row->len = 0;
	row->content = NULL;
	while (read (doc->fd, buffer, 1) != 0)
	{
		if (*buffer == '\n')
		{
			/*
			updateRender (row);
			*/
			rowsInsert (doc, row, doc->len, 1);
			Row *row = malloc (sizeof (Row));
			row->len = 0;
			row->content = NULL;
		}
		else
			charsInsert (row, buffer, row->len, 1);
	}

	return 0;
}

/* docSave: save <doc>
 */
int docSave (Doc* doc)
{
	if (doc->modified == 0)
		return 0;

	lseek (doc->fd, 0, SEEK_SET);
	for (int i = 0; i < doc->len; ++i)
	{
		write (doc->fd, doc->rows[i].content, doc->rows[i].len);
		write (doc->fd, "\n", 1);
	}
	doc->modified = 0;

	return 0;
}
