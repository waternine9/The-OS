///
/// BRIEF: A shell interpreter
///

#ifndef H_BOS_BEESCRIPT
#define H_BOS_BEESCRIPT

typedef struct {
    const char *Source;
} batch_script;


void Bee_ExecuteBatchScript(batch_script *Script);


#endif // H_BOS_BEESCRIPT