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
