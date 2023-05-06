#ifndef TOS_BFLANG_H
#define TOS_BFLANG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    BFASSIGN,
    BFADD,
    BFSUB,
    BFMUL,
    BFDIV,
    BFFUNCCALL
} bf_token_type;

typedef struct {
    uint8_t* Name;
    size_t Len;
} bf_name;

typedef struct {
    int Val;
    bf_name Name;
} bf_variable;

typedef struct {
    int Val;
} bf_constant;

struct _bf_token;

typedef struct _bf_scope {
    struct _bf_token** Lines;
    size_t LineCount;
    bf_variable** Vars; // First vars are the arguments...
    size_t ArgsCount;
    size_t VarsCount;
    struct _bf_scope* Parent;
} bf_scope;

typedef struct {
    bf_scope RootScope;
    bf_name Name;
} bf_function;

typedef struct _bf_token {
    bf_token_type Type;
    bf_variable* Var;
    bf_constant Const;
    struct _bf_token** Params;
    size_t NumParams;
    struct _bf_token* First;
    struct _bf_token* Second;
    bf_scope* NextScope;
    bf_function* CallFunction;
    bool IsConstant;
    bool IsVar;
    int Val;
} bf_token;

typedef struct
{
    uint8_t* Code;
    size_t Size;
    size_t At;
    bf_function** Functions;
    size_t BFNumFunctions;
} bf_tokenizer;

int BFRunSource(char* Code, size_t CodeSize);
#endif
