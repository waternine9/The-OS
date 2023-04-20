#ifndef H_TOS_FORMAT
#define H_TOS_FORMAT

#include <stdarg.h>
#include <stddef.h>

void FormatWriteStringVa(char *Dest, size_t N, const char *Fmt, va_list Va);
void FormatWriteString(char *Dest, size_t N, const char *Fmt, ...);
size_t FormatCStringLength(const char *String);

#endif // H_TOS_FORMAT
