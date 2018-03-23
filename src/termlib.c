#include "kvim.h"

static void sigwinch (int signo)
{
	struct winsize sz;
	ioctl (STDIN, TIOCGWINSZ, (char*) &sz);
	kvim.rows = sz.ws_row - 1;
	kvim.cols = sz.ws_col;
	printContent (kvim.doc[0]);
}

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
	term.c_lflag &= ~(ECHO|ICANON|ISIG);
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
	signal (SIGWINCH, sigwinch);

	return 0;
}

/* termExit: recover tty from stored status
 */
int termExit (void)
{
	kvim.termIn.c_lflag |= ECHO|ICANON|ISIG;
	kvim.termOut.c_lflag |= ECHO|ICANON|ISIG;
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

	int len = 2, l;
	char *s = malloc (2), *tmp;
	s[0] = '\x1b';
	s[1] = '[';
	/* convert y to chars
	 */
	tmp = convertNumToStr (y, &l);
	s = realloc (s, len + l + 1);
	memcpy (s + len, tmp, l);
	len += l + 1;
	s[len - 1] = ';';
	/* convert x to chars
	 */
	tmp = convertNumToStr (x, &l);
	s = realloc (s, len + l + 1);
	memcpy (s + len, tmp, l);
	len += l + 1;
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
	write (STDOUT, "\x1b[2J\x1b[3J\x1b[H", 11);
	char *buf = malloc (kvim.rows * kvim.cols);
	int len = 0, before = 0, row, col, left;
	int cols = kvim.cols - doc->lnlen - 1;

	if (doc->rows[doc->crow]->len == 0)
		before = 0;
	else
		before = getRenderCol (doc->rows[doc->crow], doc->ccol);
	left = kvim.cy - 1 - before / cols;

	row = doc->crow;
	if (left > 0)
	{
		for (--row; left > 0 && row >= 0; --row)
			left -= doc->rows[row]->rlen / cols + 1;
		++row;
	}
	col = -left * (kvim.cols - doc->lnlen - 1);

	for (int i = 0; i < kvim.rows; ++i)
	{
		if (row >= doc->len)
		{
			for (int j = 0; j < kvim.cols; ++j)
				appendBuf (buf, &len, " ", 1);
			continue;
		}
		if (i == 0 || col == 0)
		{
			int l = getNumLen (doc->rows[row]->ind + 1);
			for (int j = 0; j < doc->lnlen - l; ++j)
				appendBuf (buf, &len, " ", 1);
			char *s = convertNumToStr (doc->rows[row]->ind + 1, &l);
			appendBuf (buf, &len, s, l);
			free (s);
			appendBuf (buf, &len, " ", 1);
		}
		else
			for (int j = 0; j < doc->lnlen + 1; ++j)
				appendBuf (buf, &len, " ", 1);

		if (col + cols > doc->rows[row]->rlen)
		{
			appendBuf (buf, &len, doc->rows[row]->render + col,
				(doc->rows[row]->rlen - col) % cols);
			for (int j = 0; j < cols - (doc->rows[row]->rlen - col);
				++j)
				appendBuf (buf, &len, " ", 1);
			++row;
			col = 0;
		}
		else
		{
			appendBuf (buf, &len, doc->rows[row]->render + col, cols);
			col += cols;
		}
	}

	write (STDOUT, buf, len);
	free (buf);
	return 0;
}

/* printStatus: print the status info in <buf>
 */
int printStatus (char *buf, int len)
{
	cursorMove (1, kvim.rows + 1);
	write (STDOUT, "\x1b[2K", 4);
	int l1, l2; 
	char *buf1 = convertNumToStr (kvim.doc[0]->crow + 1, &l1),
		*buf2 = convertNumToStr (kvim.doc[0]->ccol + 1, &l2);
	if (len + l1 + l2 + 2 < kvim.cols)
	{
		buf = realloc (buf, kvim.cols - 1);
		for (int i = len; i < kvim.cols - 2 - l1 - l2; ++i)
			buf[i] = ' ';
		len = kvim.cols - 1;
		memcpy (buf + len - l1 - l2 - 1, buf1, l1);
		buf[len - l2 - 1] = ',';
		memcpy (buf + len - l2, buf2, l2);
	}
	else
	{
		buf = realloc (buf, len + l1 + l2 + 2);
		buf[len] = ' ';
		memcpy (buf + len + 1, buf1, l1);
		buf[len + l1 + 1] = ',';
		memcpy (buf + len + l1 + 2, buf2, l2);
		len += l1 + l2 + 2;
	}
	if (len < kvim.cols)
		write (STDOUT, buf, len);
	else
	{
		write (STDOUT, "<", 1);
		write (STDOUT, buf + len - kvim.cols + 2, kvim.cols - 2);
	}
	free (buf1);
	free (buf2);
	return 0;
}
