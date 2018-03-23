#include "kvim.h"

static int move (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp;
	switch (c)
	{
		case 'h':
		case ARROWLEFT:
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			break;
		case 'j':
		case ARROWDOWN:
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				cursorDown (doc);
			break;
		case 'k':
		case ARROWUP:
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				cursorUp (doc);
			break;
		case 'l':
		case ARROWRIGHT:
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				if (doc->ccol < doc->rows[doc->crow]->len - 1)
					cursorRight (doc);
			break;
		case 'g':
			if (kvim.iblen && kvim.inputBuf[kvim.iblen - 1] == 'g')
			{
				tmp = doc->crow;
				for (int i = 0; i < tmp; ++i)
					cursorUp (doc);
			}
			else
				appendInputBuf (c);
			break;
		case 'G':
			if (kvim.iblen == 0)
			{
				tmp = doc->len - doc->crow;
				for (int i = 0; i < tmp; ++i)
					cursorDown (doc);
			}
			else
			{
				tmp = getIbNum () - 1;
				if (!tmp)
					tmp = doc->len;
				if (tmp < doc->crow)
				{
					tmp = doc->crow - tmp;
					for (int i = 0; i < tmp; ++i)
						cursorUp (doc);
				}
				else
				{
					tmp -= doc->crow;
					for (int i = 0; i < tmp; ++i)
						cursorDown (doc);
				}
			}
			break;
		default:
			break;
	}
}

static int insert (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp;
	switch (c)
	{
		case 'i':
			kvim.iblen = 0;
			kvim.mode = MODE_INSERT;
			setStatus ("MODE: INSERT", 12);
			doc->crcol = getRenderCol (doc->rows[doc->crow], doc->ccol);
			break;
		case 'a':
			kvim.iblen = 0;
			cursorRight (doc);
			handleNormal ('i');
			break;
		case 'o':
			tmp = doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				rowInsert (doc, newRow (), doc->crow + 1);
				cursorDown (doc);
			kvim.mode = MODE_INSERT;
			setStatus ("MODE: INSERT", 12);
			doc->modified = 1;
			break;
		case 'O':
			tmp = doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			kvim.cx = 1;
			tmp = getIbNum ();
			for (int i = 0; i < tmp; ++i)
				rowInsert (doc, newRow (), doc->crow);
			kvim.mode = MODE_INSERT;
			setStatus ("MODE: INSERT", 12);
			doc->modified = 1;
			break;
		default:
			break;
	}
}

static int delete (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp;
	switch (c)
	{
		case 'x':
			tmp = getIbNum ();
			if (doc->rows[doc->crow]->content != NULL)
			{
				for (int i = 0; i < tmp; ++i)
					charsDelete (doc->rows[doc->crow], doc->ccol, 1);
				updateRender (doc->rows[doc->crow]);
			}
			doc->crcol = getRenderCol (doc->rows[doc->crow], doc->ccol);
			doc->modified = 1;
			break;
		case 's':
			handleNormal ('x');
			handleNormal ('i');
			break;
		case 'd':
			if (kvim.iblen && kvim.inputBuf[kvim.iblen - 1] == 'd')
			{
				--kvim.iblen;
				tmp = getIbNum ();
				for (int i = 0; i < doc->ccol; ++i)
					cursorLeft (doc);
				for (int i = 0; i < tmp; ++i)
					rowDelete (doc, doc->crow);
			}
			else
				appendInputBuf (c);
			doc->modified = 1;
			break;
		default:
			break;
	}
}

/* handleNormal: handle key <c> in normal mode
 */
int handleNormal (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp;
	switch (c)
	{
		case ESC:
			kvim.iblen = 0;
			break;
		case 'h':
		case ARROWLEFT:
		case 'j':
		case ARROWDOWN:
		case 'k':
		case ARROWUP:
		case 'l':
		case ARROWRIGHT:
		case 'g':
		case 'G':
			move (c);
			break;
		case 'i':
		case 'a':
		case 'o':
		case 'O':
			insert (c);
			break;
		case 'x':
		case 's':
		case 'd':
			delete (c);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (kvim.iblen == 0 ||
				(kvim.inputBuf[kvim.iblen - 1] >= '0' &&
				kvim.inputBuf[kvim.iblen - 1] <= '9'))
				appendInputBuf (c);
			else
				kvim.iblen = 0;
			break;
		case ':':
			kvim.iblen = 0;
			if (handleShell () == 2)
				return 2;
			break;
		default:
			break;
	}
	return 0;
}
