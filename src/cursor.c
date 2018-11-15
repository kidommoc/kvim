#include "kvim.h"

int cursorUp (Doc *doc)
{
	/* the width of screen excluding the width of line number
	 */
	int cols = kvim.cols - doc->lnlen - 2;
	/* if not at the first line
	 */
	if (doc->crow > 0)
	{
		kvim.cy -= getRenderCol (doc->rows[doc->crow], doc->ccol)
			/ cols + 1;
		--doc->crow;
		if (doc->rows[doc->crow]->len == 0)
		{
			kvim.cx = doc->lnlen + 3;
			doc->ccol = 0;
		}
		else if (doc->crcol < doc->rows[doc->crow]->rlen)
		{
			doc->ccol = getContentCol (doc->rows[doc->crow], doc->crcol);
			kvim.cx = getRenderCol (doc->rows[doc->crow], doc->ccol) % cols
				+ 3 + doc->lnlen;
		}
		else
		{
			doc->ccol = doc->rows[doc->crow]->len - 1;
			kvim.cx = (doc->rows[doc->crow]->rlen - 1) % cols + 3
				+ doc->lnlen;
		}
		kvim.cy -= doc->rows[doc->crow]->rlen / cols
			- getRenderCol (doc->rows[doc->crow], doc->ccol) / cols;
		if (kvim.cy <= 5)
		{
			int before = getRenderCol (doc->rows[doc->crow], doc->ccol)
				 / cols;
			for (int i = doc->crow - 1; i >= 0 && before < 5; --i)
				before += (doc->rows[i]->rlen - 1) / cols + 1;
			if (before >= 5)
				kvim.cy = 6;
			else
				kvim.cy = before + 1;
		}
	}
}

int cursorDown (Doc *doc)
{
	/* the width of screen excluding the width of line number
	 */
	int cols = kvim.cols - doc->lnlen - 2;
	/* if not at the last line
	 */
	if (doc->crow < doc->len - 1)
	{
		kvim.cy += doc->rows[doc->crow]->rlen / cols
			- getRenderCol (doc->rows[doc->crow], doc->ccol) / cols;
		++doc->crow;
		if (doc->rows[doc->crow]->len == 0)
		{
			kvim.cx = doc->lnlen + 3;
			doc->ccol = 0;
		}
		else if (doc->crcol < doc->rows[doc->crow]->rlen)
		{
			doc->ccol = getContentCol (doc->rows[doc->crow], doc->crcol);
			kvim.cx = getRenderCol (doc->rows[doc->crow], doc->ccol) % cols
				+ 3 + doc->lnlen;
		}
		else
		{
			doc->ccol = doc->rows[doc->crow]->len - 1;
			kvim.cx = (doc->rows[doc->crow]->rlen - 1) % cols + 3
				+ doc->lnlen;
		}
		kvim.cy += getRenderCol (doc->rows[doc->crow], doc->ccol) / cols
			+ 1;
		if (kvim.rows - kvim.cy < 5)
		{
			int behind = doc->rows[doc->crow]->rlen / cols
				- getRenderCol (doc->rows[doc->crow], doc->ccol) / cols;
			for (int i = doc->crow + 1; i < doc->len && behind < 5; ++i)
				behind += (doc->rows[i]->rlen - 1) / cols + 1;
			if (behind >= 5)
				kvim.cy = kvim.rows - 5;
			else
				kvim.cy = kvim.rows - behind;
		}
	}
}

int cursorRight (Doc *doc)
{
	int cols = kvim.cols - doc->lnlen - 2;
	/* only if not at the most right position
	 * will cursor move right
	 */
	if (doc->ccol < doc->rows[doc->crow]->len)
	{
		/* calculate whether cursor will go to the next line
		 */
		++doc->ccol;
		int l = getRenderCol (doc->rows[doc->crow], doc->ccol),
			l1 = getRenderCol (doc->rows[doc->crow], doc->ccol - 1);
		/* if so, go down
		 */
		if (l / cols - l1 / cols > 0)
		{
			kvim.cx = doc->lnlen + 3;
			/* if reach the bottom of screen
			 */
			if (kvim.rows - kvim.cy <= 5)
			{
				/* calculate whether reach the bottom of the doc
				 */
				int behind = doc->rows[doc->crow]->rlen / cols - l / cols;
				for (int i = doc->crow + 1; behind < 5 && i < doc->len; ++i)
					behind += (doc->rows[i]->rlen - 1) / cols + 1;
				/* if not so
				 */
				if (behind >= 5)
					kvim.cy = kvim.rows - 5;
				else
					kvim.cy = kvim.rows - behind;
			}
			else
				++kvim.cy;
		}
		else
			kvim.cx = l % cols + 3 + doc->lnlen;
	}
	/* change rendered position
	 */
	doc->crcol = getRenderCol (doc->rows[doc->crow], doc->ccol);
}

int cursorLeft (Doc *doc)
{
	int cols = kvim.cols - doc->lnlen - 2;
	/* only if not at the most left position
	 * will cursor move left
	 */
	if (doc->ccol > 0)
	{
		--doc->ccol;
		/* calculate whether cursor will go to the previous line
		 */
		int l = getRenderCol (doc->rows[doc->crow], doc->ccol),
			l1 = getRenderCol (doc->rows[doc->crow], doc->ccol + 1);
		/* if so, go up
		 */
		if (l1 / cols - l / cols > 0)
		{
			kvim.cx = kvim.cols;
			/* if reach the top of screen
			 */
			if (kvim.cy <= 6)
			{
				/* calculate whether reach the top of the doc
				 */
				int before = MIN (l, doc->rows[doc->crow]->rlen) / cols;
				for (int i = doc->crow - 1 && before < 5; i >= 0; --i)
					before += (doc->rows[i]->rlen - 1) / cols + 1;
				/* if not so
				 */
				if (before >= 5)
					kvim.cy = 6;
				else
					kvim.cy = before + 1;
			}
			else
				--kvim.cy;
		}
		else
			kvim.cx = l % cols + 3 + doc->lnlen;
	}
	/* change rendered position
	 */
	doc->crcol = getRenderCol (doc->rows[doc->crow], doc->ccol);
}
