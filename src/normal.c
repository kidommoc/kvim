#include "kvim.h"

static int move (int c)
{
	int cols = kvim.cols - kvim.doc[0]->lnlen - 1;
	Doc *doc = kvim.doc[0];
	int tmp, tmp1, tmp2;
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
				kvim.iblen = 0;
				tmp = doc->ccol;
				for (int i = 0; i < tmp; ++i)
					cursorLeft (doc);
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
				tmp = doc->ccol;
				for (int i = 0; i < tmp; ++i)
					cursorLeft (doc);
				tmp = doc->len - doc->crow;
				for (int i = 0; i < tmp; ++i)
					cursorDown (doc);
			}
			else
			{
				kvim.iblen = 0;
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
		case '0':
			tmp = doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			break;
		case '^':
			tmp = doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			while (doc->rows[doc->crow]->content[doc->ccol] == ' '
				|| doc->rows[doc->crow]->content[doc->ccol] == TAB)
				cursorRight (doc);
			break;
		case '$':
			tmp = doc->rows[doc->crow]->len - doc->ccol - 1;
			for (int i = 0; i < tmp; ++i)
				cursorRight (doc);
			break;
		case CTRL_U:
			tmp2 = kvim.cy;
			for (tmp = 0; tmp < kvim.rows / 2 && doc->crow > 0; )
			{
				tmp += (getRenderCol (doc->rows[doc->crow], doc->ccol) + 1)
					/ cols + 1;
				cursorUp (doc);
				tmp += (doc->rows[doc->crow]->rlen
					- getRenderCol (doc->rows[doc->crow], doc->ccol) + 1)
					/ cols;
			}
			tmp = 0;
			tmp1 = doc->ccol;
			for (int i = doc->crow; i >= 0; --i)
			{
				if (tmp1)
					tmp += getRenderCol (doc->rows[i], tmp1) / kvim.cols + 1;
				else
					tmp += doc->rows[i]->rlen / kvim.cols + 1;
				tmp1 = 0;
			}
			if (tmp > tmp2)
				kvim.cy = tmp2;
			break;
		case CTRL_D:
			tmp2 = kvim.cy;
			for (tmp = 0; tmp < kvim.rows / 2 && doc->crow < doc->len - 1; )
			{
				tmp += (doc->rows[doc->crow]->rlen
					- getRenderCol (doc->rows[doc->crow], doc->ccol) + 1)
					/ cols + 1;
				cursorDown (doc);
				tmp += (getRenderCol (doc->rows[doc->crow], doc->ccol) + 1)
					/ cols;
			}
			tmp = 0;
			tmp1 = doc->ccol;
			for (int i = doc->crow; i < doc->len; ++i)
			{
				if (tmp1)
					tmp +=  (doc->rows[i]->rlen - 1
					- getRenderCol (doc->rows[i], tmp1)) / kvim.cols;
				else
					tmp += doc->rows[i]->rlen / kvim.cols + 1;
				tmp1 = 0;
			}
			if (tmp > kvim.rows - tmp2)
				kvim.cy = tmp2;
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
				doc->modified = 1;
			}
			else
				appendInputBuf (c);
			break;
		default:
			break;
	}
}

static int search (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp, tmp1, tmp2, dist;
	switch (c)
	{
		case 'f':
			tmp1 = getIbNum ();
			dist = getKey ();
			tmp = doc->ccol;
			for (int i = 0; i < tmp1; ++i)
			{
				tmp2 = tmp;
				tmp = searchRowForward (doc->rows[doc->crow], tmp, dist);
				if (tmp == tmp2)
				{
					tmp = doc->ccol;
					break;
				}
			}
			tmp -= doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorRight (doc);
			break;
		case 't':
			tmp1 = getIbNum ();
			dist = getKey ();
			tmp = doc->ccol;
			for (int i = 0; i < tmp1; ++i)
			{
				tmp2 = tmp;
				tmp = searchRowForward (doc->rows[doc->crow], tmp, dist);
				if (tmp == tmp2)
				{
					tmp = doc->ccol;
					break;
				}
			}
			tmp -= doc->ccol - 1;
			for (int i = 0; i < tmp; ++i)
				cursorRight (doc);
			break;
		case 'F':
			tmp1 = getIbNum ();
			dist = getKey ();
			tmp = doc->ccol;
			for (int i = 0; i < tmp1; ++i)
			{
				tmp2 = tmp;
				tmp = searchRowBack (doc->rows[doc->crow], tmp, dist);
				if (tmp == tmp2)
				{
					tmp = doc->ccol;
					break;
				}
			}
			tmp = doc->ccol - tmp;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			break;
		case 'T':
			tmp1 = getIbNum ();
			dist = getKey ();
			tmp = doc->ccol;
			for (int i = 0; i < tmp1; ++i)
			{
				tmp2 = tmp;
				tmp = searchRowBack (doc->rows[doc->crow], tmp, dist);
				if (tmp == tmp2)
				{
					tmp = doc->ccol;
					break;
				}
			}
			tmp = doc->ccol - tmp - 1;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			break;
		case 'n':
			if (kvim.sblen)
			{
				tmp1 = doc->crow;
				tmp2 = doc->ccol + 1;
				if (searchDocForward (doc, kvim.searchBuf, kvim.sblen,
					&tmp1, &tmp2))
				{
					tmp1 = 0;
					tmp2 = 0;
					if (searchDocForward (doc, kvim.searchBuf, kvim.sblen,
						&tmp1, &tmp2))
						setStatus ("No result!", 10);
					else
					{
						//setStatus ("Hit the bottom. Start from the top.", 35);
						tmp = doc->ccol;
						for (int i = 0; i < tmp; ++i)
							cursorLeft (doc);
						tmp = doc->crow;
						for (int i = tmp1; i < tmp; ++i)
							cursorUp (doc);
						for (int i = doc->ccol; i < tmp2; ++i)
							cursorRight (doc);
						tmp = 0;
					}
				}
				else
				{
					tmp = doc->ccol;
					for (int i = 0; i < tmp; ++i)
						cursorLeft (doc);
					for (int i = doc->crow; i < tmp1; ++i)
						cursorDown (doc);
					for (int i = doc->ccol; i < tmp2; ++i)
						cursorRight (doc);
				}
			}
			else
				setStatus ("Nothing to search!", 18);
				;
			break;
		case 'N':
			if (kvim.sblen)
			{
				tmp1 = doc->crow;
				tmp2 = doc->ccol;
				if (searchDocBack (doc, kvim.searchBuf, kvim.sblen,
					&tmp1, &tmp2))
				{
					tmp1 = doc->len - 1;
					tmp2 = doc->rows[doc->len - 1]->len - 1;
					if (searchDocBack (doc, kvim.searchBuf, kvim.sblen,
						&tmp1, &tmp2))
						setStatus ("No result!", 10);
					else
					{
						//setStatus ("Hit the top. Start from the bottom.", 35);
						tmp = doc->ccol;
						for (int i = 0; i < tmp; ++i)
							cursorLeft (doc);
						tmp = doc->crow;
						for (int i = tmp; i < tmp1; ++i)
							cursorDown (doc);
						for (int i = doc->ccol; i < tmp2; ++i)
							cursorRight (doc);
					}
				}
				else
				{
					tmp = doc->ccol;
					for (int i = 0; i < tmp; ++i)
						cursorLeft (doc);
					tmp = doc->crow;
					for (int i = tmp1; i < tmp; ++i)
						cursorUp (doc);
					for (int i = doc->ccol; i < tmp2; ++i)
						cursorRight (doc);
				}
			}
			else
				setStatus ("Nothing to search!", 18);
				;
			break;
		case '/':
			kvim.iblen = 0;
			shellSearch ();
			break;
		default:
			break;
	}
}

static int displayInfo (void)
{
	int l, len = kvim.doc[0]->fnlen + 3;	
	char *buf = malloc (len), *tmp;
	buf[0] = '\"';
	memcpy (buf + 1, kvim.doc[0]->filename, kvim.doc[0]->fnlen);
	memcpy (buf + len - 2, "\" ", 2);
	tmp = convertNumToStr (kvim.doc[0]->len, &l);
	buf = realloc (buf, len + l + 4);
	memcpy (buf + len, tmp, l);
	free (tmp);
	memcpy (buf + len + l, "L --", 4);
	len += l + 4;
	tmp = convertNumToStr (
		MIN (100, kvim.doc[0]->crow * 100 / kvim.doc[0]->len + 1), &l);
	buf = realloc (buf, len + l + 3);
	memcpy (buf + len, tmp, l);
	free (tmp);
	memcpy (buf + len + l, "%--", 3);
	len += l + 3;
	setStatus (buf, len);
	free (buf);
	return 0;
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
		case '^':
		case '$':
		case CTRL_U:
		case CTRL_D:
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
		case 'f':
		case 't':
		case 'F':
		case 'T':
		case 'n':
		case 'N':
		case '/':
			search (c);
			break;
		case '0':
			if (kvim.iblen == 0)
			{
				move (c);
				break;
			}
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
		case CTRL_G:
			displayInfo ();
			break;
		case ':':
			kvim.iblen = 0;
			if (shellCommand () == 2)
				return 2;
			break;
		default:
			break;
	}
	return 0;
}
