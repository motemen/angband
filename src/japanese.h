/* File: japanese.h */

/*
 * Copyright (c) 1997 KUSUNOSE Toru
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#ifndef HEADER_JAPANESE_H
#define HEADER_JAPANESE_H

/*
 * True/false
 */
#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif /* TRUE */


/*
 * Japanese verb inflection type
 */
#define JVERB_AND 1
#define JVERB_TO  2
#define JVERB_OR  3


/* Function declarations */
extern char* jstrstr(const char* s, const char* t);
extern char iskanji2(const char *s, int x);
extern const char *sindarin_to_kana(const char *sindarin);
extern void jverb(const char *in, char *out, int flag);
extern char iskinsoku(const char *str);
extern int find_break(const char *str, int witdh, char wrap);
extern void roff_to_buf(const char *str, int width, char *buf);
extern void codeconv_sjis2euc(char *str);
extern void codeconv_euc2sjis(char *str);
extern void codeconv(char *str);
extern const char *e_to_j(const char *str);


/* Macro functions */
#if defined(EUC)
# define iskanji(x) ((((int)x & 0xff) >= 0xa1 && ((int)x & 0xff) <= 0xfe) || ((int)x & 0xff) == 0x8e)
# define ishankana(x) (0)
#elif defined(SJIS)
# define iskanji(x)  ((0x81<=(unsigned char)x && (unsigned char)x<=0x9f)|| (0xe0<=(unsigned char)x && (unsigned char)x<=0xfc))
# define ishankana(x) ((0xa0 <= ((unsigned char)x)) && (((unsigned char)x) <= 0xdf))
#elif defined(MSDOS)
# include <jctype.h>
#else
# error Oops! Please define "EUC" or "SJIS" for kanji-code of your system.
#endif /* MSDOS */


#endif /* HEADER_JAPANESE_H */
