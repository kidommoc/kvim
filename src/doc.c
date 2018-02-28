#include "kvim.h"

/* charsInsert: insert <len> chars <chars> at <at>
 */
int charsInsert (Row *row, char *chars, int at, int len)
{
	if (at < 0 || at > row->len)
		return 1;

	row->content = realloc (row->content, row->len + len);
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

/* rowsInsert: insert <len> rows <rows> at <at>
 */
int rowsInsert (Doc *doc, Row *rows, int at, int len)
{
	if (at < 0 || at > doc->len)
		return 1;

	doc->rows = realloc (doc->rows, (doc->len + len) * sizeof (Row));
	if (at != doc->len)
		memmove (doc->rows + at + len, doc->rows + at, doc->len - at);
	memcpy (doc->rows + at, rows, len * sizeof (Row));
	doc->len += len;

	return 0;
}

/* rowsDelete: delete <len> rows from <from>
 */
int rowsDelete (Doc *doc, int from, int len)
{
	if (from < 0 || from > doc->len)
		return 1;

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
