#ifndef H_STRING
#define H_STRING

#include <stdint.h>
#include <stddef.h>
#include "linked_list.hpp"

typedef LinkedList<uint8_t> String;

String StrFromCStr(const char* str);
bool StrStartsWith(String str, String startswith);
String StrFormat(const char* Format, ...);

#endif // H_STRING