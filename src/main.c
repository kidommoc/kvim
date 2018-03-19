#include "kvim.h"

const char *USAGE = "Usage: kvim <file>\n";

int init (char *filename)
{
	termInit ();
	kvim.status = NULL;
	kvim.stlen = 0;
	kvim.mode = MODE_NORMAL;
	kvim.doc = docOpen (filename);
	setStatus ("MODE: NORMAL", 12);
	return 0;
}

int quit (void)
{
	docClose (kvim.doc);
	if (kvim.status)
		free (kvim.status);
	termExit ();
	write (0, "1\n", 2);
	return 0;
}

int main (int argc, char **argv)
{
	/* now only support one file
	 */
	if (argc != 2)
	{
		write (STDERR_FILENO, USAGE, 19);
		return 1;
	}

	/* initialize
	 */
	init (argv[1]);

	/* indicating the status of kvim
	 * 0: normal
	 * 1: error
	 * 2: quit
	 */
	int st = 0;
	/* main loop
	 */
	while (!st)
	{
		printContent (kvim.doc);
		printStatus (kvim.status, kvim.stlen);
		cursorMove (kvim.cx, kvim.cy);
		st = handleKey ();
	}
	
	quit ();
	return 0;
}
