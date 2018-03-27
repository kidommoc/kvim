#include "kvim.h"

static int shell (char *buf, int len, int pos)
{
	int c;
	printStatus (buf, len);
	cursorMove (pos, kvim.rows + 1);
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
		cursorMove (pos, kvim.rows + 1);
	}
	return len;
}

/* shellCommand: handle kvim-shell command
 */
int shellCommand (void)
{
	char *buf = malloc (kvim.cols - 1);
	buf[0] = ':';
	int len = 1, pos = 2;

	if (!(len = shell (buf, len, pos)))
		return 0;

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
			int fnlen = 0;
			while (buf[i] == ' ')
				++i;
			char *filename = malloc (len - i);
			for ( ; i < len && buf[i] != ' '; ++i)
				filename[fnlen++] = buf[i];
			filename[fnlen] = '\0';
			close (kvim.doc[0]->fd);
			free (kvim.doc[0]->filename);
			kvim.doc[0]->filename = filename;
			kvim.doc[0]->fnlen = fnlen;
			kvim.doc[0]->fd = open (kvim.doc[0]->filename, O_RDWR|O_CREAT);
			syscall (SYS_chmod, kvim.doc[0]->filename, 0644);
			kvim.doc[0]->modified = 1;
		}

		docSave (kvim.doc[0]);
		/* set saved status
		 */
		char *buf = malloc (kvim.doc[0]->fnlen + 8);
		buf[0] = '\"';
		memcpy (buf + 1, kvim.doc[0]->filename, kvim.doc[0]->fnlen);
		memcpy (buf + 1 + kvim.doc[0]->fnlen,"\" saved", 7);
		setStatus (buf, kvim.doc[0]->fnlen + 8);
		free (buf);
	}

	if (commandList[QUIT])
	{
		if (kvim.doc[0]->modified)
		{
			if (commandList[FORCE])
			{
				free (buf);
				return 2;
			}
			else
				setStatus ("Warning: modified without saving. (add ! to quit anyway)", 56);
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

int shellSearch (void)
{
	char *buf = malloc (kvim.cols - 1);
	buf[0] = '/';
	int len = 1, pos = 2;

	if (!(len = shell (buf, len, pos)))
		return 0;

	if (kvim.searchBuf)
		free (kvim.searchBuf);
	kvim.sblen = len - 1;
	kvim.searchBuf = malloc (kvim.sblen);
	memcpy (kvim.searchBuf, buf + 1, kvim.sblen);
	free (buf);
	handleNormal ('n');

	return 0;
}
