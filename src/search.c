#include "kvim.h"

int searchRowForward (Row *row, int start, char dist)
{
	for (int i = start + 1; i < row->len; ++i)
		if (row->content[i] == dist)
			return i;
	return start;
}

int searchRowBack (Row *row, int start, char dist)
{
	for (int i = start - 1; i >= 0; --i)
		if (row->content[i] == dist)
			return i;
	return start;
}

int searchDocForward (Doc *doc, char *dist, int len, int *row, int *col)
{
	int stcol = *col, st = 1;
	for ( ; *row < doc->len && st; ++*row)
	{
		for (*col = stcol; *col <= doc->rows[*row]->len - len && st; ++*col)
			if (doc->rows[*row]->content[*col] == *dist
				&& compareStr (doc->rows[*row]->content + *col, dist, len))
			{
				st = 0;
				goto RETURN;
			}
		stcol = 0;
	}
RETURN:
	return st;
}

int searchDocBack (Doc *doc, char *dist, int len, int *row, int *col)
{
	int stcol = *col, st = 1;
	for ( ; *row >= 0 && st; --*row)
	{
		for (*col = stcol; *col > len - 2 && st; --*col)
			if (doc->rows[*row]->content[*col] == dist[len - 1]
				&& compareStr (doc->rows[*row]->content + *col - len + 1,
				dist, len))
			{
				st = 0;
				*col -= len - 1;
				goto RETURN;
			}
		if (*row)
			stcol = doc->rows[*row - 1]->len - 1;
	}
RETURN:
	return st;
}
