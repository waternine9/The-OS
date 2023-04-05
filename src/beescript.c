#include "beescript.h"
#include <stdbool.h>
#include <stddef.h>
#include "kernel.h"


typedef struct {
    const char *Start;
} recorder;

typedef struct {
    const char *Pointer;
    int Length;
} token;

typedef struct {
    token Name;
    token Value;
} batch_variable;

typedef struct {
    batch_variable Variables[32];
    size_t VariableCount;
    const char *Source;
    size_t Position;
} batch_executive;

static recorder StartRecording(batch_executive *Executive)
{
    return (recorder) { Executive->Source + Executive->Position };
}
static token EndRecording(batch_executive *Executive, recorder Recorder)
{
    return (token) { Recorder.Start, (Executive->Source + Executive->Position)-Recorder.Start };
}

static bool TokensEqual(token A, token B)
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
static bool TokensEqualString(token A, const char *S)
{
    for (int I = 0; I < A.Length && A.Pointer[I] && S[I]; I++) {
        if (A.Pointer[I] != S[I]) {
            return false;
        }
    }
    return true;
}

static int FindVariable(batch_executive *Executive, token Name)
{
    for (size_t I = 0; I < Executive->VariableCount; I++) {
        if (TokensEqual(Executive->Variables[I].Name, Name)) {
            return I;
        }
    }
    return -1;
}
static void SetVariable(batch_executive *Executive, batch_variable Var)
{
    int V = FindVariable(Executive, Var.Name);
    if (V != -1) {
        Executive->Variables[V] = Var;
    } else {
        if (Executive->VariableCount < 32) {
            Executive->Variables[Executive->VariableCount++] = Var;
        } else {
            // TODO: Handle error!
            return;
        }
    }
}
static token *GetVariable(batch_executive *Executive, token Name)
{
    int V = FindVariable(Executive, Name);
    if (V != -1) {
        return &Executive->Variables[V].Value;
    }
    return NULL;
}

static void AllocVariable(batch_executive *Executive, batch_variable Var)
{
    if (Executive->VariableCount < 32) {
        Executive->Variables[Executive->VariableCount++] = Var;
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
static void ExecuteCommandC(batch_executive *E, size_t Argc, token *Argv)
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

        SetVariable(E, (batch_variable) { Argv[1], Argv[2] });
    } else if (TokensEqualString(Argv[0], "val")) {
        if (Argc != 2) {
            // TODO: Error
            return;
        }

        token *Var = GetVariable(E, Argv[1]);
        if (Var == NULL) {
            KPrintf("Error: variable doesn't exist\n");
        } else {
            KPrintf("%.*s\n", (int)Var->Length, Var->Pointer);
        }
    }
}
static void ExecuteCommand(batch_executive *E)
{
    token Args[32];
    size_t ArgsLength = 0;
   
    while (GetCh(E) != '\0' && GetCh(E) != '\n') {
        recorder R = StartRecording(E);
        while (GetCh(E) && GetCh(E) != ' ' && GetCh(E) != '\n') {
            NextCh(E);
        }
        token Arg = EndRecording(E, R);

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

    while (GetCh(&Executive) != '\0') {
        ExecuteCommand(&Executive);
    }
}