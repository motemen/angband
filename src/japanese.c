/* File: japanese.c */


/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#ifdef JP

#include "japanese.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * 漢字を考慮した strstr
 */
char* jstrstr(const char *s, const char *t)
{
	int i, l1, l2;

	l1 = strlen(s);
	l2 = strlen(t);
	if (l1 >= l2)
	{
		for (i = 0; i <= l1 - l2; i++)
		{
			if (!strncmp(s + i, t, l2))
				return (char*)(s + i);
			if (iskanji(*(s + i)))
				i++;
		}
	}
	return NULL;
}

/* Returns lower-cased string */
void jstrlower(char *str)
{
	int i = 0;

	while (*str)
	{
#ifdef JP
		if (iskanji(*str)) str++;
		else
#endif
		*str = tolower(*str);

		str++;
	}
}


/* 文字列sのxバイト目が漢字の1バイト目かどうか判定する */
char iskanji2(const char *s, int x)
{
	int i;

	for (i = 0; i < x; i++)
	{
		if (iskanji(s[i])) i++;
	}
	if ((x == i) && iskanji(s[x])) return TRUE;

	return FALSE;
}

typedef struct convert_key convert_key;

struct convert_key
{
	const char *key1;
	const char *key2;
};

static const convert_key s2j_table[]= {
/* {"^","'"}, */
{"-","="}, /* '-'は内部で長音として使っているので、別の文字に変える。 */
{"dd","ッd"},{"kk","ッk"},{"ss","ッs"},{"tt","ッt"},
{"mb","nb"},{"mp","np"},{"mv","nv"},{"mm","nm"},
{"x","ks"},{"f$","v$"},{"ph","f"},
/* iant:ヤント */
{"^ia","ya"},{"^ii","yi"},{"^iu","yu"},{"^ie","ye"},{"^io","yo"},
/* sindar:シンダール  parantir:パランティア  feanor:フェアノール */
{"ar$","a-ru$"},{"ir$","ia$"},{"or$","o-ru$"},
{"rha","ra"},{"rhi","ri"},{"rhu","ru"},{"rhe","re"},{"rho","ro"},
{"ra","ラ"},{"ri","リ"},{"ru","ル"},{"re","レ"},{"ro","ロ"},
{"ir","ia"},{"ur","ua"},{"er","ea"},
{"i'r","i-a"},{"u'r","u-a"},{"e'r","e-a"},
/* paurlhach:パウアラハ */
{"ach$","aハ$"},{"uch$","uフ$"},{"och$","oホ$"},
{"ar","aル"},
{"'","-"},
/* quenya:クウェンヤ */
{"qu","kw"},
{"tya","チャ"},{"tyi","チィ"},{"tyu","チュ"},{"tye","チェ"},{"tyo","チョ"},
{"sha","シャ"},{"shi","シ"},{"shu","シュ"},{"she","シェ"},{"sho","ショ"},
{"tha","サ"},{"thi","シ"},{"thu","ス"},{"the","セ"},{"tho","ソ"},{"th","ス"},
{"cha","ハ"},{"chi","ヒ"},{"chu","フ"},{"che","ヘ"},{"cho","ホ"},{"ch","ヒ"},
{"kha","ハ"},{"khi","ヒ"},{"khu","フ"},{"khe","ヘ"},{"kho","ホ"},{"kh","ヒ"},
{"dha","ザ"},{"dhi","ジ"},{"dhu","ズ"},{"dhe","ゼ"},{"dho","ゾ"},{"dh","ズ"},
{"lha","ラ"},{"lhi","リ"},{"lhu","ル"},{"lhe","レ"},{"lho","ロ"},
{"hwa","ホワ"},{"hwi","フイ"},{"hwu","フウ"},{"hwe","フエ"},{"hwo","フオ"},
{"hya","ヒャ"},{"hyi","ヒィ"},{"hyu","ヒュ"},{"hye","ヒェ"},{"hyo","ヒョ"},
{"ya","ヤ"},{"yu","ユ"},{"yo","ヨ"},
{"my","ミュ"},
{"ba","バ"},{"bi","ビ"},{"bu","ブ"},{"be","ベ"},{"bo","ボ"},
{"ca","カ"},{"ci","キ"},{"cu","ク"},{"ce","ケ"},{"co","コ"},
{"da","ダ"},{"di","ディ"},{"du","ドゥ"},{"de","デ"},{"do","ド"},
{"fa","ファ"},{"fi","フィ"},{"fu","フ"},{"fe","フェ"},{"fo","フォ"},
{"ga","ガ"},{"gi","ギ"},{"gu","グ"},{"ge","ゲ"},{"go","ゴ"},
{"ha","ハ"},{"hi","ヒ"},{"hu","フ"},{"he","ヘ"},{"ho","ホ"},
{"ja","ジャ"},{"ji","ジ"},{"ju","ジュ"},{"je","ジェ"},{"jo","ジョ"},
{"ka","カ"},{"ki","キ"},{"ku","ク"},{"ke","ケ"},{"ko","コ"},
{"la","ラ"},{"li","リ"},{"lu","ル"},{"le","レ"},{"lo","ロ"},
{"ma","マ"},{"mi","ミ"},{"mu","ム"},{"me","メ"},{"mo","モ"},
{"na","ナ"},{"ni","ニ"},{"nu","ヌ"},{"ne","ネ"},{"no","ノ"},
{"pa","パ"},{"pi","ピ"},{"pu","プ"},{"pe","ペ"},{"po","ポ"},
{"qu","ク"},
{"sa","サ"},{"si","シ"},{"su","ス"},{"se","セ"},{"so","ソ"},
{"ta","タ"},{"ti","ティ"},{"tu","トゥ"},{"te","テ"},{"to","ト"},
{"va","ヴァ"},{"vi","ヴィ"},{"vu","ヴ"},{"ve","ヴェ"},{"vo","ヴォ"},
{"wa","ワ"},{"wi","ウィ"},{"wu","ウ"},{"we","ウェ"},{"wo","ウォ"},
{"za","ザ"},{"zi","ジ"},{"zu","ズ"},{"ze","ゼ"},{"zo","ゾ"},
{"b","ブ"},{"c","ク"},{"d","ド"},{"f","フ"},{"g","グ"},
{"h","フ"},{"j","ジュ"},{"k","ク"},{"l","ル"},{"m","ム"},
{"n","ン"},{"p","プ"},{"q","ク"},{"r","ル"},{"s","ス"},
{"t","ト"},{"v","ヴ"},{"w","ウ"},{"y","イ"},{"z","ズ"},
{"a","ア"},{"i","イ"},{"u","ウ"},{"e","エ"},{"o","オ"},
{"-","ー"},
{"^",""},{"$",""},
{NULL,NULL}
};

const char *sindarin_to_kana(const char *sindarin)
{
	static char buf1[256], buf2[256];
	int idx;

	sprintf(buf1,"^%s$",sindarin);
	jstrlower(buf1);

	for (idx = 0; s2j_table[idx].key1 != NULL; idx++)
	{
		const char *pat1 = s2j_table[idx].key1;
		const char *pat2 = s2j_table[idx].key2;
		int len = strlen(pat1);
		char *dest = buf2;
		char *src = buf1;

		while (*src)
		{
			if (strncmp(src, pat1, len) == 0)
			{
				strcpy(dest, pat2);
				src  += len;
				dest += strlen(pat2);
			}
			else if (iskanji(*src))
			{
				*dest = *src;
				dest++; src++;
				*dest = *src;
				dest++; src++;
			}
			else
			{
				*dest = *src;
				dest++; src++;
			}
		}
		*dest = 0;
		strcpy(buf1,buf2);
	}

	return (buf1);
}

/*日本語動詞活用 (打つ＞打って,打ち etc) */

#define CMPTAIL(y) strncmp(&in[l-strlen(y)],y,strlen(y))

/* 殴る,蹴る＞殴り,蹴る */
static void jverb1( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("できる")==0) sprintf(&out[l-6],"でき");

	else if (CMPTAIL("する")==0) sprintf(&out[l-4],"し");
	else if (CMPTAIL("いる")==0) sprintf(&out[l-4],"いて");
	else if (CMPTAIL("える")==0) sprintf(&out[l-4],"え");
	else if (CMPTAIL("ける")==0) sprintf(&out[l-4],"け");
	else if (CMPTAIL("げる")==0) sprintf(&out[l-4],"げ");
	else if (CMPTAIL("せる")==0) sprintf(&out[l-4],"せ");
	else if (CMPTAIL("ぜる")==0) sprintf(&out[l-4],"ぜ");
	else if (CMPTAIL("てる")==0) sprintf(&out[l-4],"て");
	else if (CMPTAIL("でる")==0) sprintf(&out[l-4],"で");
	else if (CMPTAIL("ねる")==0) sprintf(&out[l-4],"ね");
	else if (CMPTAIL("へる")==0) sprintf(&out[l-4],"へ");
	else if (CMPTAIL("べる")==0) sprintf(&out[l-4],"べ");
	else if (CMPTAIL("める")==0) sprintf(&out[l-4],"め");
	else if (CMPTAIL("れる")==0) sprintf(&out[l-4],"れ");

	else if (CMPTAIL("う")==0) sprintf(&out[l-2],"い");
	else if (CMPTAIL("く")==0) sprintf(&out[l-2],"き");
	else if (CMPTAIL("ぐ")==0) sprintf(&out[l-2],"ぎ");
	else if (CMPTAIL("す")==0) sprintf(&out[l-2],"し");
	else if (CMPTAIL("ず")==0) sprintf(&out[l-2],"じ");
	else if (CMPTAIL("つ")==0) sprintf(&out[l-2],"ち");
	else if (CMPTAIL("づ")==0) sprintf(&out[l-2],"ぢ");
	else if (CMPTAIL("ぬ")==0) sprintf(&out[l-2],"に");
	else if (CMPTAIL("ふ")==0) sprintf(&out[l-2],"ひ");
	else if (CMPTAIL("ぶ")==0) sprintf(&out[l-2],"び");
	else if (CMPTAIL("む")==0) sprintf(&out[l-2],"み");
	else if (CMPTAIL("る")==0) sprintf(&out[l-2],"り");

	else                       sprintf(&out[l],"そして");
}

/* 殴る,蹴る> 殴って蹴る */
static void jverb2( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("できる")==0) sprintf(&out[l-6],"できて");

	else if (CMPTAIL("する")==0) sprintf(&out[l-4],"して");
	else if (CMPTAIL("いる")==0) sprintf(&out[l-4],"いて");

	else if (CMPTAIL("える")==0) sprintf(&out[l-4],"えて");
	else if (CMPTAIL("ける")==0) sprintf(&out[l-4],"けて");
	else if (CMPTAIL("げる")==0) sprintf(&out[l-4],"げて");
	else if (CMPTAIL("せる")==0) sprintf(&out[l-4],"せて");
	else if (CMPTAIL("ぜる")==0) sprintf(&out[l-4],"ぜて");
	else if (CMPTAIL("てる")==0) sprintf(&out[l-4],"てって");
	else if (CMPTAIL("でる")==0) sprintf(&out[l-4],"でて");
	else if (CMPTAIL("ねる")==0) sprintf(&out[l-4],"ねて");
	else if (CMPTAIL("へる")==0) sprintf(&out[l-4],"へて");
	else if (CMPTAIL("べる")==0) sprintf(&out[l-4],"べて");
	else if (CMPTAIL("める")==0) sprintf(&out[l-4],"めて");
	else if (CMPTAIL("れる")==0) sprintf(&out[l-4],"れて");

	else if (CMPTAIL("う")==0) sprintf(&out[l-2],"って");
	else if (CMPTAIL("く")==0) sprintf(&out[l-2],"いて");
	else if (CMPTAIL("ぐ")==0) sprintf(&out[l-2],"いで");
	else if (CMPTAIL("す")==0) sprintf(&out[l-2],"して");
	else if (CMPTAIL("ず")==0) sprintf(&out[l-2],"じて");
	else if (CMPTAIL("つ")==0) sprintf(&out[l-2],"って");
	else if (CMPTAIL("づ")==0) sprintf(&out[l-2],"って");
	else if (CMPTAIL("ぬ")==0) sprintf(&out[l-2],"ねて");
	else if (CMPTAIL("ふ")==0) sprintf(&out[l-2],"へて");
	else if (CMPTAIL("ぶ")==0) sprintf(&out[l-2],"んで");
	else if (CMPTAIL("む")==0) sprintf(&out[l-2],"んで");
	else if (CMPTAIL("る")==0) sprintf(&out[l-2],"って");

	else                       sprintf(&out[l],"ことにより");
}

/* 殴る,蹴る > 殴ったり蹴ったり */
static void jverb3( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("できる")==0) sprintf(&out[l-6],"できた");

	else if (CMPTAIL("する")==0) sprintf(&out[l-4],"した");
	else if (CMPTAIL("いる")==0) sprintf(&out[l-4],"いた");

	else if (CMPTAIL("える")==0) sprintf(&out[l-4],"えた");
	else if (CMPTAIL("ける")==0) sprintf(&out[l-4],"けた");
	else if (CMPTAIL("げる")==0) sprintf(&out[l-4],"げた");
	else if (CMPTAIL("せる")==0) sprintf(&out[l-4],"せた");
	else if (CMPTAIL("ぜる")==0) sprintf(&out[l-4],"ぜた");
	else if (CMPTAIL("てる")==0) sprintf(&out[l-4],"てった");
	else if (CMPTAIL("でる")==0) sprintf(&out[l-4],"でた");
	else if (CMPTAIL("ねる")==0) sprintf(&out[l-4],"ねた");
	else if (CMPTAIL("へる")==0) sprintf(&out[l-4],"へた");
	else if (CMPTAIL("べる")==0) sprintf(&out[l-4],"べた");
	else if (CMPTAIL("める")==0) sprintf(&out[l-4],"めた");
	else if (CMPTAIL("れる")==0) sprintf(&out[l-4],"れた");

	else if (CMPTAIL("う")==0) sprintf(&out[l-2],"った");
	else if (CMPTAIL("く")==0) sprintf(&out[l-2],"いた");
	else if (CMPTAIL("ぐ")==0) sprintf(&out[l-2],"いだ");
	else if (CMPTAIL("す")==0) sprintf(&out[l-2],"した");
	else if (CMPTAIL("ず")==0) sprintf(&out[l-2],"じた");
	else if (CMPTAIL("つ")==0) sprintf(&out[l-2],"った");
	else if (CMPTAIL("づ")==0) sprintf(&out[l-2],"った");
	else if (CMPTAIL("ぬ")==0) sprintf(&out[l-2],"ねた");
	else if (CMPTAIL("ふ")==0) sprintf(&out[l-2],"へた");
	else if (CMPTAIL("ぶ")==0) sprintf(&out[l-2],"んだ");
	else if (CMPTAIL("む")==0) sprintf(&out[l-2],"んだ");
	else if (CMPTAIL("る")==0) sprintf(&out[l-2],"った");

	else                       sprintf(&out[l],"ことや");
}


void jverb( const char *in , char *out , int flag)
{
	switch (flag)
	{
		case JVERB_AND:	jverb1(in, out);	break;
		case JVERB_TO:	jverb2(in, out);	break;
		case JVERB_OR:	jverb3(in, out);	break;
	}
}

char iskinsoku(const char *str)
{
	const char *kinsoku[] =
	{
		"、", "。", "，", "．", "・", "？", "！",
		"ヽ", "ヾ", "ゝ", "ゞ", "々", "ー", "～", 
		"）", "］", "｝", "」", "』", 
		"ぁ", "ぃ", "ぅ", "ぇ", "ぉ", "っ", "ゃ", "ゅ", "ょ",
		"ァ", "ィ", "ゥ", "ェ", "ォ", "ッ", "ャ", "ュ", "ョ",
		NULL
	};
	int i;

	for (i = 0; kinsoku[i]; i++)
	{
		if (strncmp(str, kinsoku[i], 2) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* Find a reasonable break-point */
int find_break(const char *str, int witdh, char wrap)
{
	int i = 0;
	int bpos = -1;
	while (str[i] != 0 && i < ((iskanji(str[i])) ? witdh - 1 : witdh))
	{
		char k_flg = iskanji(str[i]);

		i += k_flg ? 2 : 1;

		if (wrap)
		{
			if (str[i] == '\0') bpos = i;

			else if (iskanji(str[i]))
			{
				if (!iskinsoku(&str[i])) bpos = i;
			}

			else if (k_flg || strchr(" \n(", str[i]))
			{
				bpos = i;
			}

			if (str[i] == '\n') break;
		}
	}

	if (bpos == -1) bpos = i;

	return bpos;
}

/*
 *   日本語を含む長い文字列を適当な位置で改行してメモリに書き込む
 *   例: str = "あかさたな abc dddd eeeeeeeee", width = 6
 *   のとき buf="あかさ\0たな\0abc\0dddd\0eeeeee\0eee\0\0"
 */
void roff_to_buf(const char *str, int width, char *buf)
{
	int n;
	const char *s;
	char *d;

	s = str;
	d = buf;

	while (*s)
	{
		/* Skip white spaces */
		while (!isgraph(*s) && !iskanji(*s)) s++;

		/* Find a reasonable break-point */
		n = find_break(s, width, TRUE);

		/* Copy one line of history */
		strncpy(d, s, n);
		d[n] = '\0';
		d += n + 1;
		s += n;
	}

	/* Add a blank line */
	d[0] = '\0';
}

#define KCODE_ASCII 0
#define KCODE_SJIS  1
#define KCODE_EUC   2

/* 漢字が存在し、その漢字コードがEUCかどうか調べる。*/
static int check_kcode(const char *str)
{
	int i;
	int kanji, iseuc;
	unsigned char c1;

	/* 漢字が存在し、その漢字コードがEUCかどうか調べる。*/
	kanji = 0;
	iseuc = 1;
	for (i = 0; str[i]; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)  kanji = 1;
		if ((c1 >= 0x80) && (c1 < 0xa1 || c1 > 0xfe)) iseuc = 0;
	}

	if (kanji && iseuc) return KCODE_EUC;
	if (kanji && !iseuc) return KCODE_SJIS;

	return KCODE_ASCII;
}

/* SJISからEUCに変換する */
void codeconv_sjis2euc(char *str)
{
	int i;

	char tmp[256];

	unsigned char c1, c2;

	for (i = 0; str[i] && (i < 256); i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c2 >= 0x9f)
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe0 : 0x60);
				c2 += 2;
			}
			else
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe1 : 0x61);
				c2 += 0x60 + (c2 < 0x7f);
			}
			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[i] = '\0';
	strcpy(str, tmp);
}

/* EUCからSJISに変換する */
void codeconv_euc2sjis(char *str)
{
	int i;

	char tmp[256];

	unsigned char c1, c2;


	for (i = 0; str[i] && (i < 256); i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c1 % 2)
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x31 : 0x71);
				c2 -= 0x60 + (c2 < 0xe0);
			}
			else
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x30 : 0x70);
				c2 -= 2;
			}
			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[i] = '\0';
	strcpy(str, tmp);
}

/* 与えられた文字列をそれぞれの環境に合った漢字コードにする */
void codeconv(char *str)
{
	int kcode;

	kcode = check_kcode(str);

	switch (kcode)
	{
#ifdef EUC
		case KCODE_SJIS:
			codeconv_sjis2euc(str);
			break;
#endif /* EUC */

#ifdef SJIS
		case KCODE_EUC:
			codeconv_euc2sjis(str);
			break;
#endif /* SJIS */

		default:
			break;
	}
}

/*
 * "of Foo"を"Fooの"に,、
 * "'Bar'"を"『Bar』"に変換する。
 */
const char *e_to_j(const char *str)
{
	static char buf[256];
	int len = strlen(str);

	if (strncmp(str, "of ", 3) == 0)
	{
		strcpy(buf, &str[3]);
		strcat(buf, "の");
	}
	else if (str[0] == '\'' && str[len-1] == '\'')
	{
		strcpy(buf, "『");
		strncat(buf, &str[1], len-2);
		strcat(buf, "』");
	}
	else
	{
		strcpy(buf, str);
	}

	return buf;
}

#endif
