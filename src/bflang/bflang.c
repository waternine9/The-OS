#include "bflang.h"
#include "../mem.h"


size_t BFCodeSize(uint8_t* Code)
{
    size_t Size = 0;
    while (*Code++) Size++;
    return Size;
}

bool BFCodeStep(bf_tokenizer* Tokenizer)
{
    if (Tokenizer->At >= Tokenizer->Size) return false;
    return true;
}

size_t BFTellNext(bf_tokenizer* Tokenizer, uint8_t Char)
{
    int I = Tokenizer->At;
    while (I < Tokenizer->Size)
    {
        if (Tokenizer->Code[I] == Char) return I;

        I++;
    }
    return 0xFFFFFFFF;
}

size_t BFTellNextMatching(bf_tokenizer* Tokenizer, uint8_t CharDec, uint8_t CharInc)
{
    size_t Counter = 0;
    int I = Tokenizer->At;
    while (I < Tokenizer->Size)
    {
        if (Tokenizer->Code[I] == CharInc) Counter++;
        if (Tokenizer->Code[I] == CharDec)
        {
            if (Counter == 1) return I;
            Counter--;
        }

        I++;
    }
    return 0xFFFFFFFF;
}
bool BFCmpName(bf_name X, bf_name Y)
{
    if (X.Len != Y.Len) return false;
    for (int I = 0; I < X.Len; I++)
    {
        if (X.Name[I] != Y.Name[I]) return false;
    }
    return true;
}

size_t BFFindNextOperator(bf_tokenizer* Tokenizer)
{
    for (size_t I = Tokenizer->At; I < Tokenizer->Size; I++)
    {
        if (Tokenizer->Code[I] == '+'
            || Tokenizer->Code[I] == '-'
            || Tokenizer->Code[I] == '*'
            || Tokenizer->Code[I] == '/'
            || Tokenizer->Code[I] == '=')
        {
            return I;
        }
    }
    return 0xFFFFFFFF;
}

size_t BFTellNextCom(bf_tokenizer* Tokenizer)
{
    int Counter = 0;
    for (size_t I = Tokenizer->At; I < Tokenizer->Size; I++)
    {
        if (Tokenizer->Code[I] == '[') Counter++;
        if (Tokenizer->Code[I] == ']') Counter--;
        if (Tokenizer->Code[I] == ',')
        {
            if (Counter == 0) return I;
        }
    }
    return 0xFFFFFFFF;
}

bool BFFindVariable(bf_name SearchFor, bf_scope* SearchIn, bf_variable** OutPtr)
{

    for (int I = 0; I < SearchIn->VarsCount; I++)
    {
        if (BFCmpName(SearchFor, SearchIn->Vars[I]->Name))
        {
            *OutPtr = SearchIn->Vars[I];
            return true;
        }
    }
    if (SearchIn->Parent == 0) return false;
    return BFFindVariable(SearchFor, SearchIn->Parent, OutPtr);
}



bf_token* BFTokenizeExpr(bf_tokenizer* Tokenizer, bf_scope* CurrentScope);

bf_token* BFTokenizeFuncCall(bf_tokenizer* Tokenizer, bf_scope* CurrentScope)
{
    bf_token* Token = (bf_token*)malloc(sizeof(bf_token));
    memset(Token, 0, sizeof(bf_token));
    size_t NextSqrBr = BFTellNext(Tokenizer, '[');
    size_t NextMatchingSqrBr = BFTellNextMatching(Tokenizer, ']', '[');
    Token->Type = BFFUNCCALL;
    bf_name FuncName;
    FuncName.Len = NextSqrBr - Tokenizer->At;
    FuncName.Name = Tokenizer->Code + Tokenizer->At;

    for (int I = 0; I < Tokenizer->BFNumFunctions; I++)
    {
        if (BFCmpName(FuncName, Tokenizer->Functions[I]->Name))
        {
            Token->CallFunction = Tokenizer->Functions[I];
            break;
        }
    }
    if (!Token->CallFunction) return NULL;

    Token->Params = (bf_token**)malloc(sizeof(bf_token*));

    
    Tokenizer->At = NextSqrBr + 1;

    size_t NextCom = BFTellNextCom(Tokenizer);

    if (NextSqrBr + 1 == NextMatchingSqrBr) return Token;

    bool BreakOut = false;

    while (1)
    {
        bf_token* Param = BFTokenizeExpr(Tokenizer, CurrentScope);

        Token->Params[Token->NumParams++] = Param;
        bf_token** NewParams = (bf_token**)malloc((Token->NumParams + 1) * sizeof(bf_token*));
        memcpy(NewParams, Token->Params, Token->NumParams * sizeof(bf_token*));
        free(Token->Params, Token->NumParams * sizeof(bf_token*));
        Token->Params = NewParams;

        Tokenizer->At = NextCom + 1;

        if (NextCom == 0xFFFFFFFF)
        {
            return Token;
        }
        else
        {
            NextCom = BFTellNextCom(Tokenizer);
        }
    }
    return Token;

}

bf_token_type BFGetOpType(uint8_t C)
{
    switch (C)
    {
        case '+':
            return BFADD;
        case '-':
            return BFSUB;
        case '*':
            return BFMUL;
        case '/':
            return BFDIV;
        case '=':
            return BFASSIGN;
        default:
            return BFADD;
    }
}

bf_token* BFTokenizeExpr(bf_tokenizer* Tokenizer, bf_scope* CurrentScope)
{
    
    bf_token* Tok = (bf_token*)malloc(sizeof(bf_token));
    memset(Tok, 0, sizeof(bf_token));

    if (Tokenizer->Code[Tokenizer->At] == '(')
    {
        
        size_t NextMatching = BFTellNextMatching(Tokenizer, ')', '(');
        
        if (NextMatching == 0xFFFFFFFF) 
        {
            return NULL;
        }
        Tokenizer->At++;
        bf_token* NextToken = BFTokenizeExpr(Tokenizer, CurrentScope);
        Tok->First = NextToken;
        Tokenizer->At = NextMatching + 1;
        Tok->Type = BFGetOpType(Tokenizer->Code[Tokenizer->At]);
        Tokenizer->At++;

        Tok->Second = BFTokenizeExpr(Tokenizer, CurrentScope);

        return Tok;
    }

    size_t NextOp = BFFindNextOperator(Tokenizer);
    size_t NextSemi = BFTellNext(Tokenizer, ';');
    size_t NextBr = BFTellNext(Tokenizer, ')');
    size_t NextRSqrBr = BFTellNext(Tokenizer, ']');
    size_t NextCom = BFTellNext(Tokenizer, ',');
    if (NextOp < NextBr) NextBr = NextOp;
    if (NextSemi < NextBr) NextBr = NextSemi;
    if (NextRSqrBr < NextBr) NextBr = NextRSqrBr;
    if (NextCom < NextBr) NextBr = NextCom;

    size_t NextSqrBr = BFTellNext(Tokenizer, '[');

    if (NextSqrBr < NextBr && NextRSqrBr != 0xFFFFFFFF)
    {
        
        bf_token* MyTok = BFTokenizeFuncCall(Tokenizer, CurrentScope);
        
        if (NextRSqrBr == NextCom - 1 || NextRSqrBr == NextSemi - 1) return MyTok;

        bf_token* SurroundToken = (bf_token*)malloc(sizeof(bf_token));
        memset(SurroundToken, 0, sizeof(bf_token));
        SurroundToken->First = MyTok;
        Tokenizer->At = NextRSqrBr + 1;
        SurroundToken->Type = BFGetOpType(Tokenizer->Code[Tokenizer->At]);
        Tokenizer->At++;
        SurroundToken->Second = BFTokenizeExpr(Tokenizer, CurrentScope);
        return SurroundToken;        
    }

        
    if ('0' <= Tokenizer->Code[Tokenizer->At] && Tokenizer->Code[Tokenizer->At] <= '9')
    {
        int Multiplier = 1;
        int Val = 0;
        for (int I = NextBr - 1;I >= Tokenizer->At;I--)
        {
            int C = Tokenizer->Code[I];
            if (C < '0' || C > '9') return 0;
            Val += Multiplier * (C - '0');
            Multiplier *= 10;
        }
        Tok->IsConstant = true;
        Tok->Const.Val = Val;
    }
    else
    {
        bf_variable* OutPtr;
        bf_name VarName;
        VarName.Name = Tokenizer->Code + Tokenizer->At;
        VarName.Len = NextBr - Tokenizer->At;
        
        if (BFFindVariable(VarName, CurrentScope, &OutPtr))
        {
            Tok->IsVar = true;
            Tok->Var = OutPtr;
        }
        else
        {
            bf_variable* NewVar = (bf_variable*)malloc(sizeof(bf_variable));
            NewVar->Name = VarName;
            NewVar->Val = 0;
            CurrentScope->Vars[CurrentScope->VarsCount++] = NewVar;
            bf_variable** NewVars = (bf_variable**)malloc(sizeof(bf_variable*) * (CurrentScope->VarsCount + 1));
            memcpy(NewVars, CurrentScope->Vars, sizeof(bf_variable*) * CurrentScope->VarsCount);
            free(CurrentScope->Vars, sizeof(bf_variable*) * CurrentScope->VarsCount);
            CurrentScope->Vars = NewVars;

            Tok->IsVar = true;
            Tok->Var = NewVar;
        }
    }
    

    if (NextOp > NextBr)
    {
        return Tok;
    }

    bf_token* SurroundTok = (bf_token*)malloc(sizeof(bf_token));
    SurroundTok->First = Tok;

    SurroundTok->Type = BFGetOpType(Tokenizer->Code[NextOp]);

    Tokenizer->At = NextBr + 1;

    SurroundTok->Second = BFTokenizeExpr(Tokenizer, CurrentScope);
    return SurroundTok;
}

bf_token* BFTokenizeLine(bf_tokenizer* Tokenizer, bf_scope* CurrentScope)
{

    return BFTokenizeExpr(Tokenizer, CurrentScope);
}



void BFTokenizeFunction(bf_tokenizer* Tokenizer)
{
    size_t NextSqrBr = BFTellNext(Tokenizer, '[');
    if (NextSqrBr == 0xFFFFFFFF) return;
    bf_function* CurrentFunction = (bf_function*)malloc(sizeof(bf_function));
    CurrentFunction->Name.Name = Tokenizer->Code + Tokenizer->At;
    CurrentFunction->Name.Len = NextSqrBr - Tokenizer->At;
    CurrentFunction->RootScope.Lines = (bf_token**)malloc(sizeof(bf_token*));
    CurrentFunction->RootScope.LineCount = 0;
    CurrentFunction->RootScope.Parent = 0;
    Tokenizer->At = NextSqrBr + 1;
    size_t NextSqrBr2 = BFTellNext(Tokenizer, ']');
    
    if (NextSqrBr2 == 0xFFFFFFFF) return;


    // Parse Arguments

    CurrentFunction->RootScope.Vars = (bf_variable**)malloc(sizeof(bf_variable*));
    CurrentFunction->RootScope.ArgsCount = 0;

    if (NextSqrBr + 1 != NextSqrBr2)
    {
        while (1)
        {
            uint8_t NextCom = BFTellNext(Tokenizer, ',');
            bool BreakOut = NextCom > NextSqrBr2 || NextCom == 0xFFFFFFFF;
            if (BreakOut)
            {
                NextCom = NextSqrBr2;
            }
            bf_variable *Var = (bf_variable*)malloc(sizeof(bf_variable));
            Var->Name.Name = Tokenizer->Code + Tokenizer->At;
            Var->Name.Len = NextCom - Tokenizer->At;
            Var->Val = 0;
            CurrentFunction->RootScope.Vars[CurrentFunction->RootScope.ArgsCount++] = Var;
            bf_variable** NewArgs = (bf_variable**)malloc(sizeof(bf_variable*) * (CurrentFunction->RootScope.ArgsCount + 1));
            memcpy(NewArgs, CurrentFunction->RootScope.Vars, sizeof(bf_variable*) * CurrentFunction->RootScope.ArgsCount);
            free(CurrentFunction->RootScope.Vars, sizeof(bf_variable*) * CurrentFunction->RootScope.ArgsCount);
            CurrentFunction->RootScope.Vars = NewArgs;
            Tokenizer->At = NextCom + 1;
            if (BreakOut) break;

        }
    }

    CurrentFunction->RootScope.VarsCount = CurrentFunction->RootScope.ArgsCount;

    Tokenizer->At = NextSqrBr2 + 1;
    size_t NextMatchingBr = BFTellNextMatching(Tokenizer, ')', '(');
    if (NextMatchingBr == 0xFFFFFFFF) return;

    Tokenizer->At++;

    
    while (Tokenizer->At < NextMatchingBr)
    {
        size_t NextSemi = BFTellNext(Tokenizer, ';');
        if (NextSemi == 0xFFFFFFFF) return;
        CurrentFunction->RootScope.Lines[CurrentFunction->RootScope.LineCount++] = BFTokenizeLine(Tokenizer, &CurrentFunction->RootScope);
        bf_token** NewLines = (bf_token**)malloc(sizeof(bf_token*) * (CurrentFunction->RootScope.LineCount + 1));
        memcpy(NewLines, CurrentFunction->RootScope.Lines, sizeof(bf_token*) * CurrentFunction->RootScope.LineCount);
        free(CurrentFunction->RootScope.Lines, sizeof(bf_token*) * CurrentFunction->RootScope.LineCount);
        CurrentFunction->RootScope.Lines = NewLines;

        Tokenizer->At = NextSemi + 1;
    }
    Tokenizer->At = NextMatchingBr + 1;
    Tokenizer->Functions[Tokenizer->BFNumFunctions++] = CurrentFunction;
    bf_function** NewFuncs = (bf_function**)malloc(sizeof(bf_function*) * (Tokenizer->BFNumFunctions + 1));
    memset(NewFuncs, 0, sizeof(bf_function*) * (Tokenizer->BFNumFunctions + 1));
    memcpy(NewFuncs, Tokenizer->Functions, sizeof(bf_function*) * Tokenizer->BFNumFunctions);
    free(Tokenizer->Functions, sizeof(bf_function*) * Tokenizer->BFNumFunctions);
    Tokenizer->Functions = NewFuncs;
}

// Here we go!
bf_function** BFTokenize(uint8_t* Code, size_t Len)
{
    bf_tokenizer Tokenizer;
    Tokenizer.At = 0;
    Tokenizer.Code = Code;
    Tokenizer.Size = Len;
    Tokenizer.Functions = (bf_function**)malloc(sizeof(bf_function*));

    Tokenizer.BFNumFunctions = 0;
    while (BFCodeStep(&Tokenizer))
    {
        BFTokenizeFunction(&Tokenizer);
    }

    return Tokenizer.Functions;
}

int BFExecuteFunc(bf_function* Func);

int BFExecuteToken(bf_token* Tok)
{
    if (Tok->IsConstant) return Tok->Const.Val;
    if (Tok->IsVar) 
    {
        return Tok->Var->Val;
    }
    switch (Tok->Type)
    {
    case BFASSIGN:
        Tok->First->Var->Val = BFExecuteToken(Tok->Second);
        
        return Tok->First->Var->Val;
    case BFADD:
        return BFExecuteToken(Tok->First) + BFExecuteToken(Tok->Second);
    case BFSUB:
        return BFExecuteToken(Tok->First) - BFExecuteToken(Tok->Second);
    case BFMUL:
        return BFExecuteToken(Tok->First) * BFExecuteToken(Tok->Second);
    case BFDIV:
        return BFExecuteToken(Tok->First) / BFExecuteToken(Tok->Second);
    case BFFUNCCALL:
        for (int I = 0; I < Tok->NumParams; I++)
        {
            Tok->CallFunction->RootScope.Vars[I]->Val = BFExecuteToken(Tok->Params[I]);
        }
        return BFExecuteFunc(Tok->CallFunction);
    }
}

int BFExecuteFunc(bf_function* Func)
{
    for (int I = 0; I < Func->RootScope.LineCount; I++)
    {
        int R = BFExecuteToken(Func->RootScope.Lines[I]);

        if (I == Func->RootScope.LineCount - 1)
        {
            return R;
        }
    }
}

int BFRun(bf_function** Funcs)
{
    bf_name MainName;
    MainName.Len = 4;
    MainName.Name = (uint8_t*)malloc(MainName.Len);
    const char* SMainName = "Main";
    memcpy(MainName.Name, SMainName, 4);
    while (*Funcs)
    {
        if (BFCmpName((*Funcs)->Name, MainName))
        {
            return BFExecuteFunc(*Funcs);
        }
        Funcs++;
    }
    return 0;
}

int BFRunSource(char* Code, size_t CodeSize)
{
    char* Sanitized = malloc(CodeSize);
    size_t SanitizedSize = 0;
    for (int I = 0;I < CodeSize;I++)
    {
        if (Code[I] != ' ' && Code[I] != '\t' && Code[I] != '\n')
        {
            Sanitized[SanitizedSize++] = Code[I];
        }
    }

    bf_function** Funcs = BFTokenize(Sanitized, SanitizedSize);
    return BFRun(Funcs);
}



