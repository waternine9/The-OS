#include "beescript.h"
#include <stdbool.h>
#include <stddef.h>
#include "kernel.h"
#include "cmd.h"


typedef struct {
    const char *Start;
} recorder;


typedef struct {
    const char *Source;
    size_t Position;
    batch_script *Script;
} batch_executive;

static recorder StartRecording(batch_executive *Executive)
{
    return (recorder) { Executive->Source + Executive->Position };
}
static batch_token EndRecording(batch_executive *Executive, recorder Recorder)
{
    return (batch_token) { Recorder.Start, (Executive->Source + Executive->Position)-Recorder.Start };
}

const char *PutStringBuf(batch_script *S, batch_token Token) {
    const char *Ptr = NULL;
    
    if (Token.Length >= (256-S->StringBufLength)) return NULL; // TODO: Error
    
    Ptr = S->StringBuf+S->StringBufLength;

    for (int I = 0; I < Token.Length; I++) {
        S->StringBuf[S->StringBufLength++] = Token.Pointer[I];
    }
    
    return Ptr;
}

static bool TokensEqual(batch_token A, batch_token B)
{
    if (A.Length != B.Length) {
        return false;
    }

    for (int I = 0; I < A.Length; I++) {
        if (A.Pointer[I] != B.Pointer[I]) {
            return false;
        }
    }
    return true;
}
static bool TokensEqualString(batch_token A, const char *S)
{
    for (int I = 0; I < A.Length && A.Pointer[I] && S[I]; I++) {
        if (A.Pointer[I] != S[I]) {
            return false;
        }
    }
    return true;
}

static int FindVariable(batch_executive *Executive, batch_token Name)
{
    for (size_t I = 0; I < Executive->Script->VariableCount; I++) {
        if (TokensEqual(Executive->Script->Variables[I].Name, Name)) {
            return I;
        }
    }
    return -1;
}
static void SetVariable(batch_executive *Executive, batch_variable Var)
{
    int V = FindVariable(Executive, Var.Name);
    if (V != -1) {
        Executive->Script->Variables[V] = Var;
    } else {
        if (Executive->Script->VariableCount < 32) {
            Executive->Script->Variables[Executive->Script->VariableCount++] = Var;
        } else {
            // TODO: Handle error!
            return;
        }
    }
}
static batch_token *GetVariable(batch_executive *Executive, batch_token Name)
{
    int V = FindVariable(Executive, Name);
    if (V != -1) {
        return &Executive->Script->Variables[V].Value;
    }
    return NULL;
}

static void AllocVariable(batch_executive *Executive, batch_variable Var)
{
    if (Executive->Script->VariableCount < 32) {
        Executive->Script->Variables[Executive->Script->VariableCount++] = Var;
    }
}

static int GetCh(batch_executive *Executive)
{
    return Executive->Source[Executive->Position];
}
static void NextCh(batch_executive *Executive)
{
    if (Executive->Source[Executive->Position]) {
        Executive->Position += 1;
    }
}
static void ExecuteCommandC(batch_executive *E, size_t Argc, batch_token *Argv)
{
    if (TokensEqualString(Argv[0], "echo")) {
        for (size_t I = 1; I < Argc; I++) {
            KPrintf("%.*s ", (int)Argv[I].Length, Argv[I].Pointer);
        }
        KPrintf("\n");
    } else if (TokensEqualString(Argv[0], "set")) {
        if (Argc != 3) {
            // TODO: Error
            return;
        }

        batch_token Name = Argv[1];
        Name.Pointer = PutStringBuf(E->Script, Name);
        batch_token Value = Argv[2];
        Value.Pointer = PutStringBuf(E->Script, Value);
        

        SetVariable(E, (batch_variable) { Name, Value });
    } else if (TokensEqualString(Argv[0], "val")) {
        if (Argc != 2) {
            // TODO: Error
            return;
        }

        batch_token *Var = GetVariable(E, Argv[1]);
        if (Var == NULL) {
            KPrintf("Error: variable doesn't exist\n");
        } else {
            KPrintf("%.*s\n", (int)Var->Length, Var->Pointer);
        }
    } else {
        KPrintf("Error: Command doesn't exist\n");
    }
}
static void ExecuteCommand(batch_executive *E)
{
    batch_token Args[32];
    size_t ArgsLength = 0;
   
    while (GetCh(E) != '\0' && GetCh(E) != '\n') {
        recorder R = StartRecording(E);
        while (GetCh(E) && GetCh(E) != ' ' && GetCh(E) != '\n') {
            NextCh(E);
        }
        batch_token Arg = EndRecording(E, R);

        if (GetCh(E) == ' ') {
            NextCh(E);
        }

        if (Arg.Length != 0) {
            if (ArgsLength < 32-1) {
                Args[ArgsLength++] = Arg;
            } else {
                // TODO: ERROR
                return;
            }
        }
    }

    if (GetCh(E) == '\n') {
        NextCh(E);
    }

    /*
    for (size_t I = 0; I < ArgsLength; I++) {
        // KPrintf("Token: %.*s |", (int)Args[I].Length, Args[I].Pointer);
    }
    */

    ExecuteCommandC(E, ArgsLength, Args);
}
void Bee_ExecuteBatchScript(batch_script *Script)
{
    batch_executive Executive = { 0 };
    Executive.Source = Script->Source;
    Executive.Position = 0;
    Executive.Script = Script;

    while (GetCh(&Executive) != '\0') {
        ExecuteCommand(&Executive);
    }
}