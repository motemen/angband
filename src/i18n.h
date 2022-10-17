#ifndef INCLUDED_I18N_H
#define INCLUDED_I18N_H

#include "z-util.h"

#ifdef USE_LOCALE
#include <libintl.h>
#include <locale.h>
// NOTE[locale] temporary fix for gettext not handling empty msgstr
#define _(string) ({ const char *t = gettext(string); t && t[0] == '\003' && t[1] == '\0' ? "" : t; })
#define _C(ctx, msgid) ({ const char *t = gettext(ctx "\004" msgid); strcmp(t, ctx "\004" msgid) == 0 ? msgid : t; })
#define gettext_noop(string) string
#define N_(string) gettext_noop(string)
#define GAMEDATA_(string) dgettext("gamedata", string)
#define GAMEDATA_N_(string) gettext_noop(string)
#define GAMEDATA_C_(ctx, msgid) ({ \
  char buf[256]; \
  strcpy(buf, ctx "\004"); \
  strcat(buf, (msgid)); \
  const char *t = dgettext("gamedata", buf); \
  strcmp(t, buf) == 0 ? (msgid) : t; \
})
#define STATIC_CONST_ const
#define MAX_CHAR_VISUAL_WIDTH 2
#else
#define _(string) (string)
#define _C(ctx, msgid) (msgid)
#define N_(string) (string)
#define GAMEDATA_(string) (string)
#define GAMEDATA_N_(string) (string)
#define GAMEDATA_C_(ctx, string) (string)
#define STATIC_CONST_ static const
#define MAX_CHAR_VISUAL_WIDTH 1
#endif

bool i18n_is_doublewidth(wchar_t ch);
int i18n_wchar_visualwidth(wchar_t ch);
int i18n_text_visualwidth(wchar_t *s);
bool i18n_text_split(const char *s, char *pre, char *post);

#endif /* INCLUDED_I18N_H */