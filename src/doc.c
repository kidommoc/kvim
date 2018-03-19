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
	for (int i = 0; i <= col; ++i)
		if (row->content[i] == '\t')
			do
				++l;
			while (l % TABSTOP != 0 && l % kvim.cols != 0);
		else
			++l;
	return l;
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

/* rowsInsert: insert <len> rows <rows> at <at>
 */
int rowsInsert (Doc *doc, Row *rows, int at, int len)
{
	/* if out of range, error
	 */
	if (at < 0 || at > doc->len)
		return 1;

	if (doc->rows)
		doc->rows = realloc (doc->rows, (doc->len + len) * sizeof (Row));
	else
		doc->rows = malloc ((doc->len + len) * sizeof (Row));
	if (at != doc->len)
		memmove (doc->rows + at + len, doc->rows + at,
			(doc->len - at) * sizeof (Row));
	memcpy (doc->rows + at, rows, len * sizeof (Row));
	doc->len += len;

	return 0;
}

/* rowsDelete: delete <len> rows from <from>
 */
int rowsDelete (Doc *doc, int from, int len)
{
	/* if out of range, error
	 */
	if (from < 0 || from > doc->len)
		return 1;

	for (int i = from; i < doc->len && i < from + len; ++i)
	{
		if (doc->rows[i].content)
			free (doc->rows[i].content);
		if (doc->rows[i].render)
			free (doc->rows[i].render);
		/* this line may cause crash */
		free (&doc->rows[i]);
	}

	if (from + len < doc->len)
	{
		memmove (doc->rows + from, doc->rows + from + len,
			(doc->len - from - len) * sizeof (Row));
		doc->len -= len;
	}
	else
		doc->len = from;

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
