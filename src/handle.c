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
					++l;
				while (l % TABSTOP != 0 && l % kvim.cols != 0);
			else
				++l;
		int l1 = l;
		if (doc->rows[doc->crow].content[doc->ccol + 1] == '\t')
			do
				++l1;
			while (l1 % TABSTOP != 0 && l % kvim.cols != 0);
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
					++l1;
				while (l1 % TABSTOP != 0 && l1 % kvim.cols != 0);
			else
				++l1;
		++doc->ccol;
		int l = l1;
		if (doc->rows[doc->crow].content[doc->ccol] == '\t')
			do
				++l;
			while (l % TABSTOP != 0 && l % kvim.cols != 0);
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
			kvim.cx = 1;
			doc->ccol = 0;
		}
		else
		{
			for (doc->ccol = 0;
				doc->ccol < doc->rows[doc->crow].len && l < doc->crcol;
				++doc->ccol)
				if (doc->rows[doc->crow].content[doc->ccol] == '\t')
					do
						++l;
					while (l % TABSTOP != 0 && l % kvim.cols != 0);
				else
					++l;
			kvim.cx = l % kvim.cols + 1;
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
				if (doc->rows[doc->crow].content[doc->ccol] == '\t')
					do
						++l;
					while (l % TABSTOP != 0 && l % kvim.cols != 0);
				else
					++l;
			kvim.cx = l % kvim.cols + 1;
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

int setStatus (const char *buf, int len)
{
	if (kvim.status)
	{
		free (kvim.status);
		kvim.stlen = 0;
	}
	kvim.status = malloc (len);
	memcpy (kvim.status, buf, len);
	kvim.stlen = len;
	return 0;
}

/* handleCommand: handle kvim-shell command
 */
int handleCommand (void)
{
	char c, *buf = malloc (kvim.cols - 1);
	buf[0] = ':';
	int len = 1, pos = 2;
	printStatus (buf, len);
	cursorMove (kvim.rows + 1, pos);
	while ((c = getKey ()) != ENTER)
	{
		switch (c)
		{
			case ESC:
				setStatus ("MODE: NORMAL", 12);
				free (buf);
				return 0;
				break;
			case DEL:
				if (len == 1)
				{
					setStatus ("MODE: NORMAL", 12);
					free (buf);
					return 0;
				}
				if (pos > len)
					break;
				if (pos < len)
					memmove (&buf[pos - 1], &buf[pos], len - pos + 1);
				--len;
				printStatus (buf, len);
				break;
			case BACKSPACE:
				if (len == 1)
				{
					setStatus ("MODE: NORMAL", 12);
					free (buf);
					return 0;
				}
				if (pos > 2)
				{
					if (pos <= len)
						memmove (&buf[pos - 2], &buf[pos - 1], len - pos + 1);
					--len;
					--pos;
					printStatus (buf, len);
				}
				break;
			case ARROWUP:
			case ARROWDOWN:
				break;
			case ARROWRIGHT:
				if (pos < len + 1)
					++pos;
				break;
			case ARROWLEFT:
				if (pos > 2)
					--pos;
				break;
			case TAB:
				break;
			default:
				if (len >= kvim.cols - 1)
					break;
				if (pos <= len)
					memmove (&buf[pos], &buf[pos - 1], len - pos + 1);
				buf[pos - 1] = c;
				++pos;
				++len;
				printStatus (buf, len);
				break;
		}
		cursorMove (kvim.rows + 1, pos);
	}

	int i;
	/* initialize command list
	 */
	for (i = 0; i < COMMANDNUM; ++i)
		commandList[i] = 0;
	for (i = 1; i < len && buf[i] != ' '; ++i)
	{
		if (buf[i] == 'w')
			commandList[SAVE] = 1;
		if (buf[i] == 'q')
			commandList[QUIT] = 1;
		if (buf[i] == '!')
			commandList[FORCE] = 1;
	}

	if (commandList[SAVE])
	{
		/* save as ...
		 */
		if (i < len)
		{
			char *filename = malloc (kvim.cols);
			int fnlen = 0;
			while (buf[i] == ' ')
				++i;
			for ( ; i < len && buf[i] != ' '; ++i)
				filename[fnlen++] = buf[i];
			filename[fnlen] = '\0';
			docClose (kvim.doc);
			free (kvim.doc->filename);
			kvim.doc->filename = filename;
			kvim.doc->fnlen = fnlen;
			kvim.doc->fd = open (kvim.doc->filename, O_RDWR|O_CREAT);
		}

		docSave (kvim.doc);
		/* set saved status
		 */
		if (kvim.status)
			free (kvim.status);
		kvim.status = malloc (kvim.cols - 1);
		kvim.stlen = kvim.doc->fnlen + 8;
		memcpy (kvim.status, "\"", 1);
		memcpy (kvim.status + 1, kvim.doc->filename, kvim.doc->fnlen);
		memcpy (kvim.status + kvim.doc->fnlen + 1, "\" saved", 7);
	}

	if (commandList[QUIT])
	{
		if (kvim.doc->modified)
		{
			if (commandList[FORCE])
			{
				free (buf);
				return 2;
			}
			else
				setStatus ("\x1b[7;91mWarning: modified without saving. (add ! to quit anyway)\x1b[0m", 67);
		}
		else
		{
			free (buf);
			return 2;
		}
	}

	free (buf);
	return 0;
}

/* handleNormal: handle key <c> in normal mode
 */
int handleNormal (char c)
{
	Doc *doc = kvim.doc;
	switch (c)
	{
		case 'h':
		case ARROWLEFT:
			cursorLeft (doc);
			break;
		case 'j':
		case ARROWDOWN:
			cursorDown (doc);
			break;
		case 'k':
		case ARROWUP:
			cursorUp (doc);
			break;
		case 'l':
		case ARROWRIGHT:
			cursorRight (doc);
			break;
		case 'i':
			kvim.mode = MODE_INSERT;
			setStatus ("MODE: INSERT", 12);
			doc->crcol = doc->ccol;
			break;
		case 'x':
			if (doc->rows[doc->crow].content != NULL)
			{
				charsDelete (&doc->rows[doc->crow], doc->ccol, 1);
				updateRender (&doc->rows[doc->crow]);
			}
			doc->crcol = doc->ccol;
			doc->modified = 1;
			break;
		case 'o':
			rowsInsert (doc, newRow (), doc->crow + 1, 1);
			doc->modified = 1;
			cursorDown (doc);
			doc->ccol = 0;
			kvim.cx = 0;
			doc->crcol = doc->ccol;
			kvim.mode = MODE_INSERT;
			if (kvim.status)
			{
				free (kvim.status);
				kvim.stlen = 0;
			}
			setStatus ("MODE: INSERT", 12);
			break;
		case ':':
			if (handleCommand () == 2)
				return 2;
			break;
		default:
			break;
	}
}

/* handleInsert: handle key <c> in insert mode
 */
int handleInsert (char c)
{
	Doc *doc = kvim.doc;
	int l = 0, l1 = 0;
	switch (c)
	{
		case ESC:
			kvim.mode = MODE_NORMAL;
			setStatus ("MODE: NORMAL", 12);
			cursorLeft (doc);
			break;
		case BACKSPACE:
			if (doc->ccol > 0)
				cursorLeft (doc);
			else if (doc->crow > 0)
			{
				cursorUp (doc);
				--doc.crow;
				for (int i = 0; i < doc->rows[doc.crow].len - 1; ++i)
					cursorRight (doc);
			}
			handleInsert (DEL);
			doc->modified = 1;
			break;
		case DEL:
			if (doc->ccol < doc->rows[doc->crow].len)
			{
				charsDelete (&doc->rows[doc->crow], doc->ccol, 1);
				updateRender (&doc->rows[doc->crow]);
			}
			else if (doc->crow < doc->len - 1)
			{
				charsInsert (&doc->rows[doc->crow],
					doc->rows[doc->crow + 1].content,
					doc->rows[doc->crow].len,
					doc->rows[doc->crow + 1].len);
				rowsDelete (doc, doc->crow + 1, 1);
				updateRender (&doc->rows[doc->crow]);
			}
			doc->modified = 1;
			break;
		case TAB:
			charsInsert (&doc->rows[doc->crow], &c, doc->ccol, 1);
			updateRender (&doc->rows[doc->crow]);
			++doc->ccol;
			do
				++doc->crcol;
			while (doc->crcol % 4 != 0);
			for (int i = 0; i <= doc->ccol; ++i)
				if (doc->rows[doc->crow].content[i] == '\t')
					do
						++l1;
					while (l1 % TABSTOP != 0);
				else
					++l1;
			++doc->ccol;
			l = l1;
			if (doc->rows[doc->crow].content[doc->ccol] == '\t')
				do
					++l;
				while (l % TABSTOP != 0);
			else
				++l;
			if (l / kvim.cols - l1 / kvim.cols > 0)
			{
				kvim.cx = 1;
				if (kvim.cols - kvim.cy <= 5)
				{
					int behind = (MIN (l, doc->rows[doc->crow].rlen)
						- 1) / kvim.cols;
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
			doc->modified = 1;
			break;
		case ENTER:
			rowsInsert (doc, newRow (), doc->crow + 1, 1);
			charsInsert (&doc->rows[doc->crow + 1], doc->rows[doc->crow].content + doc->ccol, 0, doc->rows[doc->crow].len - doc->ccol);
			charsDelete (&doc->rows[doc->crow], doc->ccol, doc->rows[doc->crow].len - doc->ccol);
			updateRender (&doc->rows[doc->crow]);
			updateRender (&doc->rows[doc->crow + 1]);
			cursorDown (doc);
			doc->ccol = 0;
			kvim.cx = 1;
			doc->crcol = doc->ccol;
			doc->modified = 1;
			break;
		default:
			charsInsert (&doc->rows[doc->crow], &c, doc->ccol, 1);
			updateRender (&doc->rows[doc->crow]);
			doc->crcol = doc->ccol;
			l1 = 0;
			for (int i = 0; i <= doc->ccol; ++i)
				if (doc->rows[doc->crow].content[i] == '\t')
					do
						++l1;
					while (l1 % TABSTOP != 0);
				else
					++l1;
			++doc->ccol;
			l = l1;
			if (doc->rows[doc->crow].content[doc->ccol] == '\t')
				do
					++l;
				while (l % TABSTOP != 0);
			else
				++l;
			if (l / kvim.cols - l1 / kvim.cols > 0)
			{
				kvim.cx = 1;
				if (kvim.cols - kvim.cy <= 5)
				{
					int behind = (MIN (l, doc->rows[doc->crow].rlen)
						- 1) / kvim.cols;
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
			doc->modified = 1;
			break;
	}
}

/* handleKey: handle input from <fd>
 */
int handleKey (void)
{
	char c = getKey ();
	switch (kvim.mode)
	{
		case MODE_NORMAL:
			setStatus ("MODE: NORMAL", 12);
			if (handleNormal (c) == 2)
				return 2;
			break;
		case MODE_INSERT:
			setStatus ("MODE: INSERT", 12);
			handleInsert (c);
			break;
		default:
			return 0;
	}
}
