#include "kvim.h"

/* termInit: initialize the tty
 */
int termInit (int fdin, int fdout)
{
	struct termios term;
	tcgetattr (fdin, &term);
	/* disable echo and canonical mode of stdin
	 */
	term.c_lflag &= ~(ECHO|ICANON);
	/* disable ^c
	 */
	term.c_lflag |= ISIG;
	term.c_cc[VINTR] = _POSIX_VDISABLE;
	tcsetattr (fdin, TCSADRAIN, &term);
	tcgetattr (fdout, &term);
	/* disable canonical mode of stdout
	 */
	term.c_lflag &= ~ICANON;
	tcsetattr (fdout, TCSADRAIN, &term);

	/* clear the screen
	 * move cursor to 1,1
	 */
	write (fdout, "\x1b[2J\x1b[H", 7);
	kvim.cx = 1;
	kvim.cy = 1;

	/* get tty size
	 */
	struct winsize sz;
	ioctl (fdin, TIOCGWINSZ, (char*) &sz);
	kvim.rows = sz.ws_row - 1;
	kvim.cols = sz.ws_col;

	return 0;
}

/* cursorMove: move cursor to <x>, <y>
 */
int cursorMove (int fd, int x, int y)
{
	if (x <= 0 || x > kvim.cols || y <= 0 || y > kvim.rows)
		return 1;

	int len = 2, l = 1, pow = 1;
	char *s = malloc (2);
	s[0] = '\x1b';
	s[1] = '[';
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
	s[len - 1] = ';';
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
	s[len - 1] = 'H';
	write (fd, s, len);
	return 0;
}

int getKey (int fd)
{
}