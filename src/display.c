#include "kvim.h"
#include <stdio.h>

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
			{
				++row->rlen;
			} while (row->rlen % TABSTOP != 0);
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

int updateScreen (Doc *doc)
{
}
