#include <iconv_library.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cp1251_to_utf8(char *str, char *res, int len) {
  static const long utf[256] = {
      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,
      12,   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
      24,   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
      36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
      48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,
      60,   61,   62,   63,   64,   65,   66,   67,   68,   69,   70,   71,
      72,   73,   74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
      84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
      96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,
      108,  109,  110,  111,  112,  113,  114,  115,  116,  117,  118,  119,
      120,  121,  122,  123,  124,  125,  126,  127,  1026, 1027, 8218, 1107,
      8222, 8230, 8224, 8225, 8364, 8240, 1033, 8249, 1034, 1036, 1035, 1039,
      1106, 8216, 8217, 8220, 8221, 8226, 8211, 8212, 8250, 8482, 1113, 8250,
      1114, 1116, 1115, 1119, 160,  1038, 1118, 1032, 164,  1168, 166,  167,
      1025, 169,  1028, 171,  172,  173,  174,  1031, 176,  177,  1030, 1110,
      1169, 181,  182,  183,  1105, 8470, 1108, 187,  1112, 1029, 1109, 1111,
      1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051,
      1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063,
      1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075,
      1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
      1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099,
      1100, 1101, 1102, 1103};

  int i = 0, j = 0;

  for (; i < len; ++i) {
    long c = utf[(unsigned char)str[i]];
    if (c < 0x80) {
      res[j++] = c;
    } else if (c < 0x800) {
      res[j++] = c >> 6 | 0xc0;
      res[j++] = c & 0x3f | 0x80;
    } else if (c < 0x10000) {
      res[j++] = c >> 12 | 0xe0;
      res[j++] = c >> 6 & 0x3f | 0x80;
      res[j++] = c & 0x3f | 0x80;
    } else if (c < 0x200000) {
      res[j++] = c >> 18 | 0xf0;
      res[j++] = c >> 12 & 0x3f | 0x80;
      res[j++] = c >> 6 & 0x3f | 0x80;
      res[j++] = c & 0x3f | 0x80;
    }
  }
  res[j] = '\0';
}

typedef struct ConvLetter {
  char win1251;
  int unicode;
} Letter;

static Letter g_letters[] = {
    {0x82, 0x201A},  // SINGLE LOW-9 QUOTATION MARK
    {0x83, 0x0453},  // CYRILLIC SMALL LETTER GJE
    {0x84, 0x201E},  // DOUBLE LOW-9 QUOTATION MARK
    {0x85, 0x2026},  // HORIZONTAL ELLIPSIS
    {0x86, 0x2020},  // DAGGER
    {0x87, 0x2021},  // DOUBLE DAGGER
    {0x88, 0x20AC},  // EURO SIGN
    {0x89, 0x2030},  // PER MILLE SIGN
    {0x8A, 0x0409},  // CYRILLIC CAPITAL LETTER LJE
    {0x8B, 0x2039},  // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    {0x8C, 0x040A},  // CYRILLIC CAPITAL LETTER NJE
    {0x8D, 0x040C},  // CYRILLIC CAPITAL LETTER KJE
    {0x8E, 0x040B},  // CYRILLIC CAPITAL LETTER TSHE
    {0x8F, 0x040F},  // CYRILLIC CAPITAL LETTER DZHE
    {0x90, 0x0452},  // CYRILLIC SMALL LETTER DJE
    {0x91, 0x2018},  // LEFT SINGLE QUOTATION MARK
    {0x92, 0x2019},  // RIGHT SINGLE QUOTATION MARK
    {0x93, 0x201C},  // LEFT DOUBLE QUOTATION MARK
    {0x94, 0x201D},  // RIGHT DOUBLE QUOTATION MARK
    {0x95, 0x2022},  // BULLET
    {0x96, 0x2013},  // EN DASH
    {0x97, 0x2014},  // EM DASH
    {0x99, 0x2122},  // TRADE MARK SIGN
    {0x9A, 0x0459},  // CYRILLIC SMALL LETTER LJE
    {0x9B, 0x203A},  // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    {0x9C, 0x045A},  // CYRILLIC SMALL LETTER NJE
    {0x9D, 0x045C},  // CYRILLIC SMALL LETTER KJE
    {0x9E, 0x045B},  // CYRILLIC SMALL LETTER TSHE
    {0x9F, 0x045F},  // CYRILLIC SMALL LETTER DZHE
    {0xA0, 0x00A0},  // NO-BREAK SPACE
    {0xA1, 0x040E},  // CYRILLIC CAPITAL LETTER SHORT U
    {0xA2, 0x045E},  // CYRILLIC SMALL LETTER SHORT U
    {0xA3, 0x0408},  // CYRILLIC CAPITAL LETTER JE
    {0xA4, 0x00A4},  // CURRENCY SIGN
    {0xA5, 0x0490},  // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
    {0xA6, 0x00A6},  // BROKEN BAR
    {0xA7, 0x00A7},  // SECTION SIGN
    {0xA8, 0x0401},  // CYRILLIC CAPITAL LETTER IO
    {0xA9, 0x00A9},  // COPYRIGHT SIGN
    {0xAA, 0x0404},  // CYRILLIC CAPITAL LETTER UKRAINIAN IE
    {0xAB, 0x00AB},  // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    {0xAC, 0x00AC},  // NOT SIGN
    {0xAD, 0x00AD},  // SOFT HYPHEN
    {0xAE, 0x00AE},  // REGISTERED SIGN
    {0xAF, 0x0407},  // CYRILLIC CAPITAL LETTER YI
    {0xB0, 0x00B0},  // DEGREE SIGN
    {0xB1, 0x00B1},  // PLUS-MINUS SIGN
    {0xB2, 0x0406},  // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
    {0xB3, 0x0456},  // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
    {0xB4, 0x0491},  // CYRILLIC SMALL LETTER GHE WITH UPTURN
    {0xB5, 0x00B5},  // MICRO SIGN
    {0xB6, 0x00B6},  // PILCROW SIGN
    {0xB7, 0x00B7},  // MIDDLE DOT
    {0xB8, 0x0451},  // CYRILLIC SMALL LETTER IO
    {0xB9, 0x2116},  // NUMERO SIGN
    {0xBA, 0x0454},  // CYRILLIC SMALL LETTER UKRAINIAN IE
    {0xBB, 0x00BB},  // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    {0xBC, 0x0458},  // CYRILLIC SMALL LETTER JE
    {0xBD, 0x0405},  // CYRILLIC CAPITAL LETTER DZE
    {0xBE, 0x0455},  // CYRILLIC SMALL LETTER DZE
    {0xBF, 0x0457}   // CYRILLIC SMALL LETTER YI
};

static int utf8_to_cp1251(const char *utf8, char *windows1251, size_t n) {
  int i = 0;
  int j = 0;
  for (; i < (int)n && utf8[i] != 0; ++i) {
    char prefix = utf8[i];
    char suffix = utf8[i + 1];
    if ((prefix & 0x80) == 0) {
      windows1251[j] = (char)prefix;
      ++j;
    } else if ((~prefix) & 0x20) {
      int first5bit = prefix & 0x1F;
      first5bit <<= 6;
      int sec6bit = suffix & 0x3F;
      int unicode_char = first5bit + sec6bit;

      if (unicode_char >= 0x410 && unicode_char <= 0x44F) {
        windows1251[j] = (char)(unicode_char - 0x350);
      } else if (unicode_char >= 0x80 && unicode_char <= 0xFF) {
        windows1251[j] = (char)(unicode_char);
      } else if (unicode_char >= 0x402 && unicode_char <= 0x403) {
        windows1251[j] = (char)(unicode_char - 0x382);
      } else {
        int count = sizeof(g_letters) / sizeof(Letter);
        for (int k = 0; k < count; ++k) {
          if (unicode_char == g_letters[k].unicode) {
            windows1251[j] = g_letters[k].win1251;
            goto NEXT_LETTER;
          }
        }
        // can't convert this char
        return 0;
      }
    NEXT_LETTER:
      ++i;
      ++j;
    } else {
      // can't convert this chars
      return 0;
    }
  }
  windows1251[j] = 0;
  return 1;
}

void ConvertFromCp1251ToUtf8(char *in_buf, size_t in_buf_length, char *out_buf,
                             size_t out_buf_length) {
  cp1251_to_utf8(in_buf, out_buf, in_buf_length);
}

void ConvertFromUtf8ToCp1251(char *in_buf, size_t in_buf_length, char *out_buf,
                             size_t out_buf_length) {
  utf8_to_cp1251(in_buf, out_buf, in_buf_length);
}
