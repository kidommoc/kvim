#include "kvim.h"

/* getContentCol: change rendered column <rcol> to its content column
 */
int getContentCol (const Row *row, int rcol)
{
	int l = 0, col = 0;
	for ( ; col < row->len && l < rcol; ++col)
		if (row->content[col] == '\t')
			do
				++l;
			while (l % TABSTOP != 0 && l % kvim.cols != 0);
		else
			++l;
	return col;
}

/* getRenderCol: change content column <rcol> to its render column
 */
int getRenderCol (const Row *row, int col)
{
	if (row->len == 0)
		return 0;
	int l = 0;
	for (int i = 0; i <= col && i < row->len; ++i)
		if (row->content[i] == '\t')
			do
				++l;
			while (l % TABSTOP != 0 && l % kvim.cols != 0);
		else
			++l;
	return l - 1;
}

/* charsInsert: insert <len> chars <chars> at <at>
 */
int charsInsert (Row *row, char *chars, int at, int len)
{
	/* if out of range, error
	 */
	if (at < 0 || at > row->len)
		return 1;

	if (row->content)
		row->content = realloc (row->content, row->len + len);
	else
		row->content = malloc (row->len + len);
	if (at != row->len)
		memmove (row->content + at + len, row->content + at, row->len - at);
	memcpy (row->content + at, chars, len);
	row->len += len;

	return 0;
}

/* charsDelete: delete <len> chars from <from>
 */
int charsDelete (Row *row, int from, int len)
{
	/* if out of range, error
	 */
	if (from < 0 || from > row->len)
		return 1;

	if (from + len < row->len)
	{
		memmove (row->content + from, row->content + from + len,
			row->len - from - len);
		row->len -= len;
	}
	else
		row->len = from;

	return 0;
}

/* newRow: return a pointer to a new row
 */
Row* newRow (void)
{
	Row *row = malloc (sizeof (Row));
	row->content = NULL;
	row->len = 0;
	row->render = NULL;
	row->rlen = 0;
	return row;
}

/* rowInsert: insert one row <row> at <at>
 */
int rowInsert (Doc *doc, Row *row, int at)
{
	/* if out of range, error
	 */
	if (at < 0 || at > doc->len)
		return 1;

	if (doc->rows)
		doc->rows = realloc (doc->rows, (doc->len + 1) * sizeof (Row*));
	else
		doc->rows = malloc ((doc->len + 1) * sizeof (Row*));
	if (at != doc->len)
		memmove (doc->rows + at + 1, doc->rows + at,
			(doc->len - at) * sizeof (Row*));
	doc->rows[at] = row;
	++doc->len;

	return 0;
}

/* rowDelete: delete <len> rows from <from>
 */
int rowDelete (Doc *doc, int from)
{
	/* if out of range, error
	 */
	if (from < 0 || from > doc->len)
		return 1;

	if (doc->rows)
	{
		if (doc->rows[from]->content)
			free (doc->rows[from]->content);
		if (doc->rows[from]->render)
			free (doc->rows[from]->render);
		free (doc->rows[from]);

		Row **tmp = NULL;
		if (from + 1 < doc->len)
		{
			tmp = malloc ((doc->len - 1) * sizeof (Row*));
			memcpy (tmp, doc->rows, from * sizeof (Row*));
			memcpy (tmp + from, doc->rows + from + 1,
				(doc->len - from - 1) * sizeof (Row*));
			--doc->len;
		}
		else
		{
			tmp = malloc (from * sizeof (Row*));
			memcpy (tmp, doc->rows, from * sizeof (Row*));
			doc->len = from;
		}
		free (doc->rows);
		doc->rows = tmp;
	}

	return 0;
}

/* updateRender: turn content to render of <row>
 */
int updateRender (Row *row)
{
	row->rlen = 0;
	if (row->render)
		free (row->render);
	for (int i = 0; i < row->len; ++i)
		if (row->content[i] == '\t')
			do
				++row->rlen;
			while (row->rlen % TABSTOP != 0 && row->len % kvim.cols != 0);
		else
			++row->rlen;
	row->render = malloc (row->rlen);
	int ind = 0;
	for (int i = 0; i < row->len; ++i)
	{
		if (row->content[i] == '\t')
			do
			{
				row->render[ind] = ' ';
				++ind;
			} while (ind % TABSTOP != 0);
		else
		{
			row->render[ind] = row->content[i];
			++ind;
		}
	}
	return 0;
}
