#include "kvim.h"

int getNumLen (int n)
{
	int l = 1, pow = 1;
	for ( ; n / (pow *= 10); ++l)
		;
	return l;
}

char* convertNumToStr (int n, int *len)
{
	*len = getNumLen (n);
	int pow = 1;
	for (int i = 0; i < *len; ++i)
		pow *= 10;
	char *s = malloc (*len);
	for (int i = 0; i < *len; ++i, n %= pow)
		s[i] = n / (pow /= 10) + 48;
	return s;
}

int convertStrToNum (int* s, int len)
{
	int a = 0;
	for (int i = 0; i < len; ++i)
		a = a * 10 + s[i] - 48;
	return a > 0 ? a : 1;
}

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
	if (col == row->len)
		return row->rlen;
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
