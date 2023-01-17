#include <stdlib.h>
#include <string.h>

#include "i18n.h"
#include "z-util.h"

bool i18n_is_doublewidth(wchar_t ch)
{
	// FIXME: Very dumb implementation
	return ch > 0xff;
}

int i18n_wchar_visualwidth(wchar_t ch) {
	return i18n_is_doublewidth(ch) ? 2 : 1;
}

int i18n_text_visualwidth(wchar_t *s)
{
	int w = 0;
	for (wchar_t c = *s; c; c = *++s)
	{
		w += i18n_wchar_visualwidth(c);
	}
	return w;
}

bool i18n_text_split(const char *s, char *pre, char *post) {
	static const char *sep = "{}";
	char *p = strstr(s, sep);
	if (!p) return false;

	strncpy(pre, s, p - s);
	*(pre + (p - s)) = '\0';
	strcpy(post, p + strlen(sep));

	return true;
}
