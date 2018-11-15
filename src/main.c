#include "kvim.h"

const char *USAGE = "Usage: kvim <file>\n";

static int init (char *filename)
{
	termInit ();
	kvim.status = NULL;
	kvim.stlen = 0;
	kvim.mode = MODE_NORMAL;
	setStatus ("MODE: NORMAL", 12);
	kvim.doc = malloc (sizeof (Doc*));
	kvim.doc[0] = docOpen (filename);
	kvim.cx = kvim.doc[0]->lnlen + 2;
	kvim.ib.len = 0;
	kvim.sb.searchBuf = NULL;
	kvim.sb.len = 0;
	kvim.cb.type = CT_CHAR;
	kvim.cb.clipBuf.c = NULL;
	kvim.cb.clipBuf.r = NULL;
	return 0;
}

static int quit (void)
{
	if (kvim.status)
		free (kvim.status);
	if (kvim.sb.searchBuf)
		free (kvim.sb.searchBuf);
	docClose (kvim.doc[0]);
	free (kvim.doc);
	if (kvim.cb.type == CT_ROW && kvim.cb.clipBuf.r)
	{
		for (int i = 0; i < kvim.cb.len; ++i)
		{
			if (kvim.cb.clipBuf.r[i]->content)
				free (kvim.cb.clipBuf.r[i]->content);
			if (kvim.cb.clipBuf.r[i]->render)
				free (kvim.cb.clipBuf.r[i]->render);
		}
		free (kvim.cb.clipBuf.r);
	}
	else if (kvim.cb.type == CT_CHAR && kvim.cb.clipBuf.c)
		free (kvim.cb.clipBuf.c);
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
			case MODE_REPLACE:
				setStatus ("MODE: REPLACE", 13);
				st = handleReplace (c);
				break;
			default:
				break;
		}
	}
	
	quit ();
	return 0;
}
