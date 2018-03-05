#include "kvim.h"

int cursorLeft (Doc *doc)
{
	/* only if not at the most left position
	 * will cursor move left
	 */
	if (doc->ccol > 0)
	{
		--doc->ccol;
		/* calculate whether cursor will go to the previous line
		 */
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
		/* if so, go up
		 */
		if (l1 / kvim.cols - l / kvim.cols > 0)
		{
			kvim.cx = kvim.cols;
			/* if reach the top of screen
			 */
			if (kvim.cy <= 6)
			{
				/* calculate whether reach the top of the doc
				 */
				int before = (MIN (l, doc->rows[doc->crow].rlen) - 1)
					/ kvim.cols;
				for (int i = doc->crow - 1 && before < 5; i >= 0; --i)
					before += doc->rows[i].rlen / kvim.cols + 1;
				/* if not so
				 */
				if (before > 5)
					kvim.cy = 6;
				else
					kvim.cy = before;
			}
			else
				--kvim.cy;
		}
		else
			kvim.cx = l % kvim.cols;
	}
	/* change rendered position
	 */
	doc->crcol = doc->ccol;
}

int cursorRight (Doc *doc)
{
	/* only if not at the most right position
	 * will cursor move right
	 */
	if (doc->ccol < doc->rows[doc->crow].len - 1)
	{
		/* calculate whether cursor will go to the next line
		 */
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
		/* if so, go down
		 */
		if (l / kvim.cols - l1 / kvim.cols > 0)
		{
			kvim.cx = 1;
			/* if reach the bottom of screen
			 */
			if (kvim.cols - kvim.cy <= 5)
			{
				/* calculate whether reach the bottom of the doc
				 */
				int behind = (MIN (l, doc->rows[doc->crow].rlen)
					- 1) / kvim.cols;
				for (int i = doc->crow && behind < 6; i < doc->len; ++i)
					behind += doc->rows[i].rlen / kvim.cols + 1;
				/* if not so
				 */
				if (behind > 5)
					kvim.cy = kvim.cols - 5;
				else
					kvim.cy = kvim.cols - behind;
			}
			else
				++kvim.cy;
		}
		else
			kvim.cx = l % kvim.cols;
	}
	/* change rendered position
	 */
	doc->crcol = doc->ccol;
}

int cursorUp (Doc *doc)
{
	if (doc->crow > 0)
	{
		--doc->crow;
		int l = 0;
		if (doc->rows[doc->crow].len == 0)
		{
			kvim.cx = 0;
			doc->ccol = 0;
		}
		else
		{
			for (doc->ccol = 0;
				doc->ccol < doc->rows[doc->crow].len && l < doc->crcol;
				++doc->ccol)
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
			int before = (MIN (l, doc->rows[doc->crow].rlen) - 1)
				/ kvim.cols;
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
}

int cursorDown (Doc *doc)
{
	if (doc->crow < doc->len - 1)
	{
		++doc->crow;
		int l = 0;
		if (doc->rows[doc->crow].len == 0)
		{
			kvim.cx = 1;
			doc->ccol = 0;
		}
		else
		{
			for (doc->ccol = 0;
				doc->ccol < doc->rows[doc->crow].len && l < doc->crcol;
				++doc->ccol)
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
			int behind = (MIN (l, doc->rows[doc->crow].rlen) - 1)
				/ kvim.cols;
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
}

/* handleNormal: handle key <c> in normal mode
 */
int handleNormal (char c)
{
	Doc *doc = kvim.doc;
	switch (c)
	{
		case 'h':
			cursorLeft (doc);
			break;
		case 'j':
			cursorDown (doc);
			break;
		case 'k':
			cursorUp (doc);
			break;
		case 'l':
			cursorRight (doc);
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
