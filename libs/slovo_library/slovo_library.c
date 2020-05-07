#include <fcntl.h>
#include <iconv_library.h>
#include <slovo_library.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DICT_NAME "../../dict/dict.txt"

static int dict_size = 0;
static char *dict_buf = NULL;
#define MIN_LETTERS_NUM 3
#define MAX_LETTERS_NUM 10
#define DICTS_NUM (MAX_LETTERS_NUM - MIN_LETTERS_NUM + 1)
static char *dict_size_buf[DICTS_NUM];
static int dict_size_buf_size[DICTS_NUM] = {0};
static char *cur_dict = NULL;
static int cur_dict_size = 0;

int LoadDictionary(void) {
  int h_dict = -1;
  struct stat statbuf = {0};

  h_dict = open(DICT_NAME, O_RDONLY);
  if (h_dict < 0) {
    printf("Dictionary file %s not found\n", DICT_NAME);
    return -1;
  }
  fstat(h_dict, &statbuf);
  dict_size = statbuf.st_size;
  dict_buf = malloc(dict_size);
  if (!dict_buf) {
    printf("Dictionary allocation error\n");
    return -2;
  }
  read(h_dict, dict_buf, dict_size);
  close(h_dict);

  return 0;
}

char ToLower(char bukva) {
  if ((uint8_t)bukva == 0xa8 || (uint8_t)bukva == 0xb8) {
    return 0xe5;
  }
  if ((uint8_t)bukva >= 0xc0 && (uint8_t)bukva < 0xe0) {
    return (uint8_t)bukva + 0x20;
  }

  return bukva;
}

int CompareWords(char *word1, char *word2, int length) {
  for (int i = 0; i < length; i++) {
    if ((uint8_t)(word1[i]) > (uint8_t)(word2[i])) {
      return 1;
    }
    if ((uint8_t)(word1[i]) < (uint8_t)(word2[i])) {
      return -1;
    }
  }

  return 0;
}

static void printcp(char *text, int len) {
#define MAX_BUF 32
  char buf[MAX_BUF] = {0};

  ConvertFromCp1251ToUtf8(text, len, buf, MAX_BUF);
  printf("%s", buf);
}

void AddToDict(char *dict, int *size, char *word, int word_length) {
#ifdef NO_SORT
  // simple
  for (int j = 0; j < word_length; j++) {
    dict[*size] = ToLower(word[j]);
    (*size)++;
  }
#else
  // sorted
  int cur_idx = *size;
  int cmp_result = 0;

  for (int j = 0; j < word_length; j++) {
    word[j] = ToLower(word[j]);
  }

  if (!cur_idx) {
    memcpy(dict, word, word_length);
    *size = word_length;
    return;
  } else {
    while (1) {
      if (cur_idx >= word_length) {
        cur_idx -= word_length;
        cmp_result = CompareWords(word, dict + cur_idx, word_length);
      } else {
        cur_idx = -word_length;
        cmp_result = 2;
      }
      if (!cmp_result) {
        return;
      }
      if (cmp_result > 0) {
        cur_idx += word_length;
        memmove(dict + cur_idx + word_length, dict + cur_idx, *size - cur_idx);
        memcpy(dict + cur_idx, word, word_length);
        *size = *size + word_length;
        return;
      }
    }
  }
#endif
}

void CreateSizeDictionary(char *dict, int *size, int word_length) {
  int index = 0;
  int i = 0;

  while (1) {
    i = index;
    while ((uint8_t)(dict_buf[index]) > 0x20) {
      index++;
      if (index >= dict_size) {
        break;
      }
    }

    if (index - i == word_length) {
      AddToDict(dict, size, dict_buf + i, word_length);
    }

    if (index >= dict_size) {
      break;
    }

    while ((uint8_t)(dict_buf[index]) <= 0x20) {
      index++;
      if (index >= dict_size) {
        break;
      }
    }
  }
}

bool CreateDictionaries(void) {
  for (int i = 0; i < DICTS_NUM; i++) {
    dict_size_buf[i] = malloc(dict_size);
    dict_size_buf_size[i] = 0;
    if (!dict_size_buf[i]) {
      return false;
    }
    CreateSizeDictionary(dict_size_buf[i], &(dict_size_buf_size[i]),
                         i + MIN_LETTERS_NUM);
  }

  return true;
}

void ClearDictionaries(void) {
  for (int i = 0; i < DICTS_NUM; i++) {
    free(dict_size_buf[i]);
    dict_size_buf_size[i] = 0;
  }
}

#ifndef NO_SORT
int CheckWordLimits(char *word, int word_length, int lo_idx, int high_idx) {
  int cur_idx = 0;
  int rc = 0;

  if (high_idx - lo_idx < 32) {
    for (cur_idx = lo_idx; cur_idx <= high_idx; cur_idx += word_length) {
      if (!memcmp(word, cur_dict + cur_idx, word_length)) {
        return 0;
      }
    }
    return -1;
  }

  cur_idx = (((lo_idx + high_idx) / word_length) / 2) * word_length;
  rc = CompareWords(word, cur_dict + cur_idx, word_length);
  if (!rc) {
    return 0;
  }
  if (lo_idx == high_idx) {
    return -1;
  }
  if (rc < 0) {
    return CheckWordLimits(word, word_length, lo_idx, cur_idx);
  }
  return CheckWordLimits(word, word_length, cur_idx, high_idx);
}
#endif

bool CheckWord(char *word, int word_length) {
#ifdef NO_SORT
  // simple
  int index = 0;

  for (index = 0; index < cur_dict_size; index += word_length) {
    if (!memcmp(word, cur_dict + index, word_length)) {
      return true;
    }
  }

  return false;
#else
  // sorted
  if (!CheckWordLimits(word, word_length, 0, cur_dict_size - word_length)) {
    return true;
  }
  return false;
#endif
}

#define FIELD_DXY 4

static char field[FIELD_DXY][FIELD_DXY] = {{0xe2, 0xed, 0xe7, 0xef},
                                           {0xe4, 0xe2, 0xe4, 0xf0},
                                           {0xf2, 0xe5, 0xe8, 0xe5},
                                           {0xfc, 0xee, 0xe2, 0xe4}};

static bool used_field[FIELD_DXY][FIELD_DXY] = {0};

#define MAX_LETTERS 20
#define MAX_WORDS 100
static char curr_word[MAX_LETTERS] = {0};
static char used_word[MAX_WORDS][MAX_LETTERS] = {0};
static int used_words_num = 0;

static bool CheckInUsedWords(char *word, int size) {
  for (int i = 0; i < used_words_num; i++) {
    if (!memcmp(word, used_word[i], size)) {
      return true;
    }
  }

  if (used_words_num < MAX_WORDS) {
    memcpy(used_word[used_words_num], word, size);
    used_words_num++;
  }

  return false;
}

void TestPosition(int x, int y, int pos, int size) {
  if (x < 0 || x >= FIELD_DXY || y < 0 || y >= FIELD_DXY || used_field[y][x]) {
    return;
  }

  curr_word[pos] = field[y][x];
  used_field[y][x] = true;
  if (size <= 1) {
    if (CheckWord(curr_word, pos + 1)) {
      if (!CheckInUsedWords(curr_word, pos + 1)) {
        char buff[100] = {0};

        curr_word[pos + 1] = 0;
        ConvertFromCp1251ToUtf8(curr_word, pos + 2, buff, 100);
        printf("%s\n", buff);
      }
    }
  } else {
    TestPosition(x - 1, y - 1, pos + 1, size - 1);
    TestPosition(x, y - 1, pos + 1, size - 1);
    TestPosition(x + 1, y - 1, pos + 1, size - 1);
    TestPosition(x - 1, y, pos + 1, size - 1);
    TestPosition(x + 1, y, pos + 1, size - 1);
    TestPosition(x - 1, y + 1, pos + 1, size - 1);
    TestPosition(x, y + 1, pos + 1, size - 1);
    TestPosition(x + 1, y + 1, pos + 1, size - 1);
  }
  used_field[y][x] = false;
}

void ShowAnswers(int size) {
  memset(used_field, 0, sizeof(used_field));
  used_words_num = 0;
  cur_dict = dict_size_buf[size - MIN_LETTERS_NUM];
  cur_dict_size = dict_size_buf_size[size - MIN_LETTERS_NUM];
  for (int x = 0; x < FIELD_DXY; x++) {
    for (int y = 0; y < FIELD_DXY; y++) {
      TestPosition(x, y, 0, size);
    }
  }
}

void ShowAllAnswers(void) {
  for (int i = MIN_LETTERS_NUM; i <= MAX_LETTERS_NUM; i++) {
    ShowAnswers(i);
  }
}

void GetField(void) {
  char field_str[100] = {0};
  char cvt_str[100] = {0};

  printf("Input field 4x4:\n");
  for (int i = 0; i < FIELD_DXY; i++) {
    printf("Enter string %i: ", i + 1);
    fgets(field_str, 100, stdin);
    ConvertFromUtf8ToCp1251(field_str, 100, cvt_str, 100);
    for (int x = 0; x < FIELD_DXY; x++) {
      field[i][x] = ToLower(cvt_str[x]);
    }
  }
}

void StartSlovo(void) {
  printf("Slovo.\n");

  GetField();

  if (LoadDictionary()) {
    return;
  }

  if (!CreateDictionaries()) {
    return;
  }

  ShowAllAnswers();

  ClearDictionaries();
  free(dict_buf);
}
