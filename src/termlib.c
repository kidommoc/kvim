#include "kvim.h"

/* termInit: initialize the tty
 */
int termInit (void)
{
	struct termios term;
	tcgetattr (STDIN, &term);
	/* disable echo, canonical mode and signal of stdin
	 */
	term.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr (STDIN, TCSADRAIN, &term);
	tcgetattr (STDOUT, &term);
	/* disable canonical mode of stdout
	 */
	term.c_lflag &= ~ICANON;
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

/* cursorMove: move cursor to <x>, <y>
 */
int cursorMove (int x, int y)
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
	write (STDOUT, s, len);
	return 0;
}

/* getKey: return the key pressed
 */
int getKey (void)
{
}
