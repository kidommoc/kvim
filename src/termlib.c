#include "kvim.h"

/* termInit: initialize the tty
 */
int termInit (void)
{
	struct termios term;
	tcgetattr (STDIN, &term);
	memcpy (&kvim.termIn, &term, sizeof (struct termios));
	/* disable echo, canonical mode and signal of stdin
	 */
	term.c_lflag &= ~(ECHO | ICANON | ISIG);
	/* 100 ms timeout
	 * return each byte or 0 when timeout
	 */
	term.c_cc[VTIME] = 1;
	term.c_cc[VMIN] = 0;
	tcsetattr (STDIN, TCSADRAIN, &term);

	tcgetattr (STDOUT, &term);
	memcpy (&kvim.termOut, &term, sizeof (struct termios));
	/* disable canonical mode of stdout
	 */
	term.c_lflag &= ~(ECHO|ICANON);
	tcsetattr (STDOUT, TCSADRAIN, &term);

	/* clear the screen
	 * move cursor to 1,1
	 */
	write (STDOUT, "\x1b[2J\x1b[H", 7);
	kvim.cx = 1;
	kvim.cy = 1;

	/* get tty size
	 */
	struct winsize sz;
	ioctl (STDIN, TIOCGWINSZ, (char*) &sz);
	kvim.rows = sz.ws_row - 1;
	kvim.cols = sz.ws_col;

	return 0;
}

/* termExit: recover tty from stored status
 */
int termExit (void)
{
	tcsetattr (STDIN, TCSANOW, &kvim.termIn);
	tcsetattr (STDOUT, TCSANOW, &kvim.termOut);
	write (STDOUT, "\x1b[2J\x1b[H", 7);
	return 0;
}

/* cursorMove: move cursor to <x>, <y>
 */
int cursorMove (int x, int y)
{
	if (x <= 0 || x > kvim.cols || y <= 0 || y > kvim.rows + 1)
		return 1;

	int len = 2, l = 1, pow = 1;
	char *s = malloc (2);
	s[0] = '\x1b';
	s[1] = '[';
	/* count the length of y
	 */
	for (l = 1; y / (pow *= 10); ++l)
		;
	len += l;
	s = realloc (s, len);
	/* convert y to chars
	 */
	for (int i = 0; i < l; ++i, y %= pow)
		s[len - l + i] = y / (pow /= 10) + 48;
	++len;
	s = realloc (s, len);
	s[len - 1] = ';';
	/* count the length of x
	 */
	for (l = 1; x / (pow *= 10); ++l)
		;
	len += l;
	s = realloc (s, len);
	/* convert x to chars
	 */
	for (int i = 0; i < l; ++i, x %= pow)
		s[len - l + i] = x / (pow /= 10) + 48;
	++len;
	s = realloc (s, len);
	s[len - 1] = 'H';
	write (STDOUT, s, len);
	free (s);
	return 0;
}

/* getKey: return the key pressed
 */
int getKey (void)
{
	while (1)
	{
		char c, buf[3];
		while (read (STDIN, &c, 1) == 0)
			;

		switch (c)
		{
			/* ESC sequence
			 */
			case ESC:
				if (read (STDIN, &buf[0], 1) == 0)
					return ESC;
				if (read (STDIN, &buf[1], 1) == 0)
					return ESC;

				/* ESC[ sequence
				 */
				if (buf[0] == '[')
					if (buf[1] >= '0' && buf[1] <= '9')
					{
						if (read(STDIN, &buf[2], 1) == 0)
							return ESC;
						if (buf[1] == '3' && buf[2] == '~')
							return DEL;
					}
					else
						switch (buf[1])
						{
							case 'A':
								return ARROWUP;
							case 'B':
								return ARROWDOWN;
							case 'C':
								return ARROWRIGHT;
							case 'D':
								return ARROWLEFT;
						}
				break;
			default:
				return c;
		}
	}
}

int appendBuf (char *buf, int *bflen, const char *from, int len)
{
	memcpy (buf + *bflen, from, len);
	*bflen += len;
	return 0;
}

/* printContent: print the content of <doc>
 */
int printContent (Doc *doc)
{
	write (STDOUT, "\x1b[2J\x1b[H", 7);
	char *buf = malloc (kvim.rows * kvim.cols);
	int len = 0, before = 0, strow, stcol = 0, left;
	if (doc->rows[doc->crow].len == 0)
		before = 0;
	else
		before = getRenderCol (&doc->rows[doc->crow], doc->ccol) - 1;
	left = kvim.cy - 1 - before / kvim.cols;
	strow = doc->crow;
	if (left > 0)
	{
		for (--strow; left > 0 && strow >= 0; --strow)
			left -= doc->rows[strow].rlen / kvim.cols + 1;
		++strow;
	}
	stcol = -left * kvim.cols;
	left = kvim.rows * kvim.cols;
	for (int i = strow; i < doc->len && left > 0; ++i)
	{
		if (doc->rows[i].rlen - stcol +
			(doc->rows[i].rlen - doc->rows[i].rlen % kvim.cols) > left)
			appendBuf (buf, &len, doc->rows[i].render + stcol, left);
		else
		{
			appendBuf (buf, &len,
				doc->rows[i].render + stcol, doc->rows[i].rlen - stcol);
			for (int j = 0;
				j < kvim.cols - doc->rows[i].rlen % kvim.cols; ++j)
				appendBuf (buf, &len, " ", 1);
		}
		left = kvim.rows * kvim.cols - len;
		if (stcol)
			stcol = 0;
	}
	write (STDOUT, buf, len);
	free (buf);
	return 0;
}

/* printStatus: print the status info in <buf>
 */
int printStatus (const char *buf, int len)
{
	cursorMove (1, kvim.rows + 1);
	write (STDOUT, "\x1b[2K", 4);
	write (STDOUT, buf, len);
	return 0;
}
