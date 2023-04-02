#ifndef H_BOS_FORMAT
#define H_BOS_FORMAT

#include <stdarg.h>
#include <stddef.h>

void FormatWriteStringVa(char *Dest, size_t N, const char *Fmt, va_list Va);
void FormatWriteString(char *Dest, size_t N, const char *Fmt, ...);

#endif // H_BOS_FORMAT