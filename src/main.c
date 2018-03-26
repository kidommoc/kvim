#include "kvim.h"

const char *USAGE = "Usage: kvim <file>\n";

static int init (char *filename)
{
	termInit ();
	kvim.status = NULL;
	kvim.stlen = 0;
	kvim.iblen = 0;
	kvim.mode = MODE_NORMAL;
	kvim.doc = malloc (sizeof (Doc*));
	kvim.doc[0] = docOpen (filename);
	kvim.cx = kvim.doc[0]->lnlen + 2;
	kvim.searchBuf = NULL;
	kvim.sblen = 0;
	setStatus ("MODE: NORMAL", 12);
	return 0;
}

static int quit (void)
{
	if (kvim.status)
		free (kvim.status);
	if (kvim.searchBuf)
		free (kvim.searchBuf);
	docClose (kvim.doc[0]);
	free (kvim.doc);
	termExit ();
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
		printContent (kvim.doc[0]);
		printStatus (kvim.status, kvim.stlen);
		cursorMove (kvim.cx, kvim.cy);

		int c = getKey ();
		switch (kvim.mode)
		{
			case MODE_NORMAL:
				setStatus ("MODE: NORMAL", 12);
				st = handleNormal (c);
				break;
			case MODE_INSERT:
				setStatus ("MODE: INSERT", 12);
				st = handleInsert (c);
				break;
			default:
				break;
		}
	}
	
	quit ();
	return 0;
}
