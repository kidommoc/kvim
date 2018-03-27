#include "kvim.h"

/* handleInsert: handle key <c> in insert mode
 */
int handleInsert (int c)
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
			else if (doc->crow > 0)
			{
				cursorUp (doc);
				for (int i = 0; i < doc->rows[doc->crow]->len; ++i)
					cursorRight (doc);
			}
			handleInsert (DEL);
			doc->modified = 1;
			break;
		case DEL:
			if (doc->ccol < doc->rows[doc->crow]->len)
			{
				charsDelete (doc->rows[doc->crow], doc->ccol, 1);
				updateRender (doc->rows[doc->crow]);
			}
			else if (doc->crow < doc->len - 1)
			{
				charsInsert (doc->rows[doc->crow],
					doc->rows[doc->crow + 1]->content,
					doc->rows[doc->crow]->len,
					doc->rows[doc->crow + 1]->len);
				rowDelete (doc, doc->crow + 1);
				updateRender (doc->rows[doc->crow]);
			}
			doc->crcol = getRenderCol (doc->rows[doc->crow], doc->ccol);
			doc->modified = 1;
			break;
		case ENTER:
			rowInsert (doc, newRow (), doc->crow + 1);
			charsInsert (doc->rows[doc->crow + 1],
				doc->rows[doc->crow]->content + doc->ccol,
				0, doc->rows[doc->crow]->len - doc->ccol);
			charsDelete (doc->rows[doc->crow], doc->ccol,
				doc->rows[doc->crow]->len - doc->ccol);
			updateRender (doc->rows[doc->crow]);
			updateRender (doc->rows[doc->crow + 1]);
			tmp = doc->ccol;
			for (int i = 0; i < tmp; ++i)
				cursorLeft (doc);
			cursorDown (doc);
			doc->modified = 1;
			break;
		default:
			charsInsert (doc->rows[doc->crow], (char*)&c, doc->ccol, 1);
			updateRender (doc->rows[doc->crow]);
			cursorRight (doc);
			doc->modified = 1;
			break;
	}
	return 0;
}
