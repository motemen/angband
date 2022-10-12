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
 * �������l������ strstr
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


/* ������s��x�o�C�g�ڂ�������1�o�C�g�ڂ��ǂ������肷�� */
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
{"-","="}, /* '-'�͓����Œ����Ƃ��Ďg���Ă���̂ŁA�ʂ̕����ɕς���B */
{"dd","�bd"},{"kk","�bk"},{"ss","�bs"},{"tt","�bt"},
{"mb","nb"},{"mp","np"},{"mv","nv"},{"mm","nm"},
{"x","ks"},{"f$","v$"},{"ph","f"},
/* iant:�����g */
{"^ia","ya"},{"^ii","yi"},{"^iu","yu"},{"^ie","ye"},{"^io","yo"},
/* sindar:�V���_�[��  parantir:�p�����e�B�A  feanor:�t�F�A�m�[�� */
{"ar$","a-ru$"},{"ir$","ia$"},{"or$","o-ru$"},
{"rha","ra"},{"rhi","ri"},{"rhu","ru"},{"rhe","re"},{"rho","ro"},
{"ra","��"},{"ri","��"},{"ru","��"},{"re","��"},{"ro","��"},
{"ir","ia"},{"ur","ua"},{"er","ea"},
{"i'r","i-a"},{"u'r","u-a"},{"e'r","e-a"},
/* paurlhach:�p�E�A���n */
{"ach$","a�n$"},{"uch$","u�t$"},{"och$","o�z$"},
{"ar","a��"},
{"'","-"},
/* quenya:�N�E�F���� */
{"qu","kw"},
{"tya","�`��"},{"tyi","�`�B"},{"tyu","�`��"},{"tye","�`�F"},{"tyo","�`��"},
{"sha","�V��"},{"shi","�V"},{"shu","�V��"},{"she","�V�F"},{"sho","�V��"},
{"tha","�T"},{"thi","�V"},{"thu","�X"},{"the","�Z"},{"tho","�\"},{"th","�X"},
{"cha","�n"},{"chi","�q"},{"chu","�t"},{"che","�w"},{"cho","�z"},{"ch","�q"},
{"kha","�n"},{"khi","�q"},{"khu","�t"},{"khe","�w"},{"kho","�z"},{"kh","�q"},
{"dha","�U"},{"dhi","�W"},{"dhu","�Y"},{"dhe","�["},{"dho","�]"},{"dh","�Y"},
{"lha","��"},{"lhi","��"},{"lhu","��"},{"lhe","��"},{"lho","��"},
{"hwa","�z��"},{"hwi","�t�C"},{"hwu","�t�E"},{"hwe","�t�G"},{"hwo","�t�I"},
{"hya","�q��"},{"hyi","�q�B"},{"hyu","�q��"},{"hye","�q�F"},{"hyo","�q��"},
{"ya","��"},{"yu","��"},{"yo","��"},
{"my","�~��"},
{"ba","�o"},{"bi","�r"},{"bu","�u"},{"be","�x"},{"bo","�{"},
{"ca","�J"},{"ci","�L"},{"cu","�N"},{"ce","�P"},{"co","�R"},
{"da","�_"},{"di","�f�B"},{"du","�h�D"},{"de","�f"},{"do","�h"},
{"fa","�t�@"},{"fi","�t�B"},{"fu","�t"},{"fe","�t�F"},{"fo","�t�H"},
{"ga","�K"},{"gi","�M"},{"gu","�O"},{"ge","�Q"},{"go","�S"},
{"ha","�n"},{"hi","�q"},{"hu","�t"},{"he","�w"},{"ho","�z"},
{"ja","�W��"},{"ji","�W"},{"ju","�W��"},{"je","�W�F"},{"jo","�W��"},
{"ka","�J"},{"ki","�L"},{"ku","�N"},{"ke","�P"},{"ko","�R"},
{"la","��"},{"li","��"},{"lu","��"},{"le","��"},{"lo","��"},
{"ma","�}"},{"mi","�~"},{"mu","��"},{"me","��"},{"mo","��"},
{"na","�i"},{"ni","�j"},{"nu","�k"},{"ne","�l"},{"no","�m"},
{"pa","�p"},{"pi","�s"},{"pu","�v"},{"pe","�y"},{"po","�|"},
{"qu","�N"},
{"sa","�T"},{"si","�V"},{"su","�X"},{"se","�Z"},{"so","�\"},
{"ta","�^"},{"ti","�e�B"},{"tu","�g�D"},{"te","�e"},{"to","�g"},
{"va","���@"},{"vi","���B"},{"vu","��"},{"ve","���F"},{"vo","���H"},
{"wa","��"},{"wi","�E�B"},{"wu","�E"},{"we","�E�F"},{"wo","�E�H"},
{"za","�U"},{"zi","�W"},{"zu","�Y"},{"ze","�["},{"zo","�]"},
{"b","�u"},{"c","�N"},{"d","�h"},{"f","�t"},{"g","�O"},
{"h","�t"},{"j","�W��"},{"k","�N"},{"l","��"},{"m","��"},
{"n","��"},{"p","�v"},{"q","�N"},{"r","��"},{"s","�X"},
{"t","�g"},{"v","��"},{"w","�E"},{"y","�C"},{"z","�Y"},
{"a","�A"},{"i","�C"},{"u","�E"},{"e","�G"},{"o","�I"},
{"-","�["},
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

/*���{�ꓮ�����p (�ł��ł���,�ł� etc) */

#define CMPTAIL(y) strncmp(&in[l-strlen(y)],y,strlen(y))

/* ����,�R�遄����,�R�� */
static void jverb1( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("�ł���")==0) sprintf(&out[l-6],"�ł�");

	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�ł�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�˂�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("�߂�")==0) sprintf(&out[l-4],"��");
	else if (CMPTAIL("���")==0) sprintf(&out[l-4],"��");

	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");

	else                       sprintf(&out[l],"������");
}

/* ����,�R��> �����ďR�� */
static void jverb2( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("�ł���")==0) sprintf(&out[l-6],"�ł���");

	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");

	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"�Ă���");
	else if (CMPTAIL("�ł�")==0) sprintf(&out[l-4],"�ł�");
	else if (CMPTAIL("�˂�")==0) sprintf(&out[l-4],"�˂�");
	else if (CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"�ւ�");
	else if (CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"�ׂ�");
	else if (CMPTAIL("�߂�")==0) sprintf(&out[l-4],"�߂�");
	else if (CMPTAIL("���")==0) sprintf(&out[l-4],"���");

	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"�˂�");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"�ւ�");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"���");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"���");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");

	else                       sprintf(&out[l],"���Ƃɂ��");
}

/* ����,�R�� > ��������R������ */
static void jverb3( const char *in , char *out)
{
	int l=strlen(in);
	strcpy(out,in);

	if      (CMPTAIL("�ł���")==0) sprintf(&out[l-6],"�ł���");

	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");

	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("����")==0) sprintf(&out[l-4],"����");
	else if (CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"�Ă���");
	else if (CMPTAIL("�ł�")==0) sprintf(&out[l-4],"�ł�");
	else if (CMPTAIL("�˂�")==0) sprintf(&out[l-4],"�˂�");
	else if (CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"�ւ�");
	else if (CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"�ׂ�");
	else if (CMPTAIL("�߂�")==0) sprintf(&out[l-4],"�߂�");
	else if (CMPTAIL("���")==0) sprintf(&out[l-4],"�ꂽ");

	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"�˂�");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"�ւ�");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"��");
	else if (CMPTAIL("��")==0) sprintf(&out[l-2],"����");

	else                       sprintf(&out[l],"���Ƃ�");
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
		"�A", "�B", "�C", "�D", "�E", "�H", "�I",
		"�R", "�S", "�T", "�U", "�X", "�[", "�`", 
		"�j", "�n", "�p", "�v", "�x", 
		"��", "��", "��", "��", "��", "��", "��", "��", "��",
		"�@", "�B", "�D", "�F", "�H", "�b", "��", "��", "��",
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
 *   ���{����܂ޒ����������K���Ȉʒu�ŉ��s���ă������ɏ�������
 *   ��: str = "���������� abc dddd eeeeeeeee", width = 6
 *   �̂Ƃ� buf="������\0����\0abc\0dddd\0eeeeee\0eee\0\0"
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

/* ���������݂��A���̊����R�[�h��EUC���ǂ������ׂ�B*/
static int check_kcode(const char *str)
{
	int i;
	int kanji, iseuc;
	unsigned char c1;

	/* ���������݂��A���̊����R�[�h��EUC���ǂ������ׂ�B*/
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

/* SJIS����EUC�ɕϊ����� */
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

/* EUC����SJIS�ɕϊ����� */
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

/* �^����ꂽ����������ꂼ��̊��ɍ����������R�[�h�ɂ��� */
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
 * "of Foo"��"Foo��"��,�A
 * "'Bar'"��"�wBar�x"�ɕϊ�����B
 */
const char *e_to_j(const char *str)
{
	static char buf[256];
	int len = strlen(str);

	if (strncmp(str, "of ", 3) == 0)
	{
		strcpy(buf, &str[3]);
		strcat(buf, "��");
	}
	else if (str[0] == '\'' && str[len-1] == '\'')
	{
		strcpy(buf, "�w");
		strncat(buf, &str[1], len-2);
		strcat(buf, "�x");
	}
	else
	{
		strcpy(buf, str);
	}

	return buf;
}

#endif
