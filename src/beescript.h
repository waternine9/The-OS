///
/// BRIEF: A shell interpreter
///

#ifndef H_BOS_BEESCRIPT
#define H_BOS_BEESCRIPT

#include <stddef.h>

typedef struct {
    const char *Pointer;
    int Length;
} batch_token;

typedef struct {
    batch_token Name;
    batch_token Value;
} batch_variable;

typedef struct {
    const char *Source;
    batch_variable Variables[32];
    size_t VariableCount;
    char StringBuf[256];
    int StringBufLength;
} batch_script;


void Bee_ExecuteBatchScript(batch_script *Script);


#endif // H_BOS_BEESCRIPT