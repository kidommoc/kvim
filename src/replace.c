#include "kvim.h"

int handleReplace (int c)
{
	Doc *doc = kvim.doc[0];
	int tmp;
	switch (c)
	{
		case ESC:
			kvim.mode = MODE_NORMAL;
			setStatus ("MODE: NORMAL", 12);
			cursorLeft (doc);
			break;
		case ARROWUP:
			cursorUp (doc);
			break;
		case ARROWDOWN:
			cursorDown (doc);
			break;
		case ARROWRIGHT:
			cursorRight (doc);
			break;
		case ARROWLEFT:
			cursorLeft (doc);
			break;
		case BACKSPACE:
			if (doc->ccol > 0)
				cursorLeft (doc);
			else
			{
				cursorUp (doc);
				for (int i = 0; i < doc->rows[doc->crow]->len; ++i)
					cursorRight (doc);
			}
			break;
		case ENTER:
			for (int i = doc->ccol; i >= 0; --i)
				cursorLeft (doc);
			cursorDown (doc);
			break;
		default:
			if (c < 32)
				break;
			if (doc->ccol < doc->rows[doc->crow]->len)
				charsDelete (doc->rows[doc->crow], doc->ccol, 1);
			charsInsert (doc->rows[doc->crow], (char*) &c, doc->ccol, 1);
			updateRender (doc->rows[doc->crow]);
			cursorRight (doc);
			doc->modified = 1;
			break;
	}
	return 0;
}
