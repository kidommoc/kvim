#include "kvim.h"

Row* newRow (void)
{
	Row *row = malloc (sizeof (Row));
	row->content = NULL;
	row->len = 0;
	row->render = NULL;
	row->rlen = 0;
	return row;
}

/* handleNormal: handle key <c> in normal mode
 */
int handleNormal (char c)
{
	Doc *doc = kvim.doc;
	switch (c)
	{
		case 'h':
			if (doc->ccol > 0)
			{
				--doc->ccol;
				int l = 0;
				for (int i = 0; i <= doc->ccol; ++i)
					if (doc->rows[doc->crow].content[i] == '\t')
						do
						{
							++l;
						} while (l % TABSTOP != 0);
					else
						++l;
				int l1 = l;
				if (doc->rows[doc->crow].content[doc->ccol + 1] == '\t')
					do
					{
						++l1;
					} while (l1 % TABSTOP != 0);
				else
					++l1;
				if (l1 / kvim.cols - l / kvim.cols > 0)
				{
					kvim.cx = kvim.cols;
					if (kvim.cy <= 6)
					{
						int before;
						if (l < doc->rows[doc->crow].rlen)
							before = l / kvim.cols;
						else
							before = doc->rows[doc->crow].rlen / kvim.cols
								- 1;
						for (int i = doc->crow - 1 && before < 5; i >= 0; --i)
							before += doc->rows[i].rlen / kvim.cols + 1;
						if (before > 5)
							kvim.cy = 6;
						else
							kvim.cy = before;
					}
					else
						--kvim.cy;
				}
				else
				{
					kvim.cx = l % kvim.cols;
				}
				doc->crcol = doc->ccol;
			}
			break;
		case 'j':
			if (doc->crow < doc->len - 1)
			{
				++doc->crow;
				int l = 0;
				if (doc->rows[doc->crow].len == 0)
					doc->ccol = 0;
				else
				{
					for (doc->ccol = 0; doc->ccol < doc->rows[doc->crow].len && l < doc->crcol; ++doc->ccol)
					{
						if (doc->rows[doc->crow].content[doc->ccol] == '\t')
						{
							do
							{
								++l;
							} while (l % TABSTOP != 0);
						}
						else
							++l;
					}
					kvim.cx = l % kvim.cols;
				}
				if (kvim.rows - kvim.cy <= 5)
				{
					int behind;
					if (l < doc->rows[doc->crow].rlen)
						behind = (doc->rows[doc->crow].rlen - 1) / kvim.cols - l / kvim.cols;
					else
						behind = 0;
					for (int i = doc->crow + 1; i < doc->len && behind < 6; ++i)
						behind += (doc->rows[i].rlen -1 ) / kvim.cols + 1;
					if (behind > 5)
						kvim.cy = kvim.rows - 5;
					else
						kvim.cy = kvim.rows - behind;
				}
				else
					++kvim.cy;
			}
			break;
		case 'k':
			if (doc->crow > 0)
			{
				--doc->crow;
				int l = 0;
				if (doc->rows[doc->crow].len == 0)
					doc->ccol = 0;
				else
				{
					for (doc->ccol = 0; doc->ccol < doc->rows[doc->crow].len && l < doc->crcol; ++doc->ccol)
					{
						if (doc->rows[doc->crow].content[doc->ccol] == '\t')
						{
							do
							{
								++l;
							} while (l % TABSTOP != 0);
						}
						else
							++l;
					}
					kvim.cx = l % kvim.cols;
				}
				if (kvim.cy <= 6)
				{
					int before;
					if (l < doc->rows[doc->crow].rlen)
						before = l / kvim.cols;
					else
						before = (doc->rows[doc->crow].rlen - 1) / kvim.cols - 1;
					for (int i = doc->crow - 1; i >= 0 && before < 5; --i)
						before += (doc->rows[i].rlen  - 1) / kvim.cols + 1;
					if (before > 5)
						kvim.cy = 6;
					else
						kvim.cy = before;
				}
				else
					--kvim.cy;
			}
			break;
		case 'l':
			if (doc->ccol < doc->rows[doc->crow].len - 1)
			{
				int l1 = 0;
				for (int i = 0; i <= doc->ccol; ++i)
					if (doc->rows[doc->crow].content[i] == '\t')
						do
						{
							++l1;
						} while (l1 % TABSTOP != 0);
					else
						++l1;
				++doc->ccol;
				int l = l1;
				if (doc->rows[doc->crow].content[doc->ccol] == '\t')
					do
					{
						++l;
					} while (l % TABSTOP != 0);
				else
					++l;
				if (l / kvim.cols - l1 / kvim.cols > 0)
				{
					kvim.cx = 1;
					if (kvim.cols - kvim.cy <= 5)
					{
						int behind;
						if (l < doc->rows[doc->crow].rlen)
							behind = doc->rows[doc->crow].rlen / kvim.cols
								- l / kvim.cols;
						else
							behind = 0;
						for (int i = doc->crow && behind < 6; i < doc->len; ++i)
							behind += doc->rows[i].rlen / kvim.cols + 1;
						if (behind > 5)
							kvim.cy = kvim.cols - 5;
						else
							kvim.cy = kvim.cols - behind;
					}
					else
						++kvim.cy;
				}
				else
				{
					kvim.cx = l % kvim.cols;
				}
				doc->crcol = doc->ccol;
			}
			break;
		default:
			break;
	}
}

/* handleInsert: handle key <c> in insert mode
 */
int handleInsert (char c)
{
}

/* handleKey: handle input from <fd>
 */
int handleKey (int fd)
{
	char c = getKey (fd);
	switch (kvim.mode)
	{
		case MODE_NORMAL:
			handleNormal (c);
			break;
		case MODE_INSERT:
			handleInsert (c);
			break;
		default:
			return 1;
	}
}
