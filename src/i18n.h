#ifndef INCLUDED_I18N_H
#define INCLUDED_I18N_H

#include "z-util.h"

#ifdef USE_LOCALE

#include <libintl.h>
#include <locale.h>

#define GETTEXT_GLUE "\004"

#define gettext_noop(msgid) (msgid)

// Use \003 to indicate translation to an empty string, not a missing translation
#define _(msgid) ({ \
    const char *t = gettext(msgid); \
    t && t[0] == '\003' && t[1] == '\0' ? "" : t; \
})

#define _C(ctx, msgid) ({ \
    const char *t = gettext(ctx GETTEXT_GLUE msgid); \
    strcmp(t, ctx GETTEXT_GLUE msgid) == 0 ? msgid : t; \
})

#define N_(msgid) gettext_noop(msgid)

#define _GAMEDATA(msgid) dgettext("gamedata", msgid)

extern char _gamedata_c_buf[1024];

#define _GAMEDATA_C(ctx, msgid) ({ \
  strcpy(_gamedata_c_buf, ctx GETTEXT_GLUE); \
  strcat(_gamedata_c_buf, (msgid)); \
  const char *t = dgettext("gamedata", _gamedata_c_buf); \
  strcmp(t, _gamedata_c_buf) == 0 ? msgid : t; \
})

#define N_GAMEDATA(msgid) gettext_noop(msgid)

#define MAX_CHAR_VISUAL_WIDTH 2

#else

#define _(string) (string)
#define _C(ctx, msgid) (msgid)
#define N_(string) (string)
#define _GAMEDATA(string) (string)
#define _GAMEDATA_C(ctx, string) (string)
#define N_GAMEDATA(string) (string)
#define MAX_CHAR_VISUAL_WIDTH 1
#define ngettext(msgid, msgid_plural, n) ((n) == 1 ? (msgid) : (msgid_plural))

#endif

bool i18n_is_doublewidth(wchar_t ch);
int i18n_wchar_visualwidth(wchar_t ch);
int i18n_text_visualwidth(const wchar_t *s);
int i18n_visualwidth(const char *buf);
bool i18n_text_split(const char *s, char *pre, char *post);

#endif /* INCLUDED_I18N_H */
