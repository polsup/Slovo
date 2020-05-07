#ifndef ICONV_LIBRARY_H
#define ICONV_LIBRARY_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void ConvertFromCp1251ToUtf8(char *in_buf, size_t in_buf_length, char *out_buf,
                             size_t out_buf_length);

void ConvertFromUtf8ToCp1251(char *in_buf, size_t in_buf_length, char *out_buf,
                             size_t out_buf_length);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // ICONV_LIBRARY_H
