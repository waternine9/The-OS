#include "format.h"

typedef struct {
    char *Dest;
    size_t N;
    size_t I;
} destination;

typedef struct {
    size_t Number;
    size_t Sign;
} num;

const char *BASE_CHARS = "0123456789ABCDEF";

static num Int2Num(int i) {
    return (num){(i < 0) ? -i : i, i < 0};
}
static num Uint2Num(unsigned int i) {
    return (num){i, 0};
}
static void WriteDestination(destination *Dest, char C)
{
    if (Dest->I < Dest->N) {
        Dest->Dest[Dest->I] = C;
    }
    Dest->I++;
}
static void WriteDestinationStr(destination *Dest, const char *S)
{
    while (*S) {
        WriteDestination(Dest, *S++);
    }
}
static int NumWidthBaseN(size_t Value, size_t Base)
{
    if (Base == 1) return 0;
    if (Base > 16) return 0;

    size_t End = 0;
    do {
        Value /= Base;
        End += 1;
    } while (Value);

    return End;
}
static void WriteDestinationBaseN(destination *Dest, size_t Value, size_t Base)
{
    if (Base == 1) return;
    if (Base > 16) return;

    size_t End = NumWidthBaseN(Value, Base);

    char Buf[64] = { 0 };
    do {
        size_t Index = Value % Base;
        Value /= Base;

        Buf[--End] = BASE_CHARS[Index];
    } while (Value);

    WriteDestinationStr(Dest, Buf);
}
static int IsDigit(int c)
{
    return c >= '0' && c <= '9';
}
static int DigitValue(int c)
{
    return c - '0';
}
static void Number(destination *Dest, int Width, int ZeroPad, num Num, int Base)
{
    if (Num.Sign) {
        WriteDestination(Dest, '-');
    }

    int L = NumWidthBaseN(Num.Number, Base);
    for (; L < Width; L++) {
        if (ZeroPad) WriteDestination(Dest, '0');
        else         WriteDestination(Dest, ' ');
    }

    WriteDestinationBaseN(Dest, Num.Number, Base);
} 
static void String(destination *Dest, int Width, int ZeroPad, const char *Str)
{
    // TODO: This can be optimized if we just fetch first Width characters.
    size_t L = FormatCStringLength(Str);

    for (; L < Width; L++) {
        if (ZeroPad) WriteDestination(Dest, '0');
        else         WriteDestination(Dest, ' ');
    }   

    WriteDestinationStr(Dest, Str);
}
void FormatWriteStringVa(char *Dest, size_t N, const char *Fmt, va_list Va)
{
    if (N == 0) return;

    destination Buffer = { 0 };
    Buffer.Dest = Dest;
    Buffer.N = N-1;
    int Width = 0;
    int ZeroPad = 0;

    for (int I = 0; Fmt[I]; I++) {
        Width = 0;
        switch (Fmt[I]) {
            case '%':
                I++;
                if (Fmt[I] == '0') {
                    ZeroPad = 1;
                    I++;
                }
                while (IsDigit(Fmt[I])) {
                    Width += DigitValue(Fmt[I++]);
                }
                switch (Fmt[I]) {
                    case 's':
                        String(&Buffer, Width, ZeroPad, va_arg(Va, const char *));
                        break;
                    case 'd':
                        Number(&Buffer, Width, ZeroPad, Int2Num(va_arg(Va, int)), 10);
                        break;
                    case 'u':
                        Number(&Buffer, Width, ZeroPad, Uint2Num(va_arg(Va, unsigned int)), 10);
                        break;
                    case 'x':
                        Number(&Buffer, Width, ZeroPad, Uint2Num(va_arg(Va, unsigned int)), 16);
                        break;
                    default:
                    case '%':
                        WriteDestination(&Buffer, '%');
                        break;
                }
                break;
            default:
                WriteDestination(&Buffer, Fmt[I]);
                break;
        }
    }

    WriteDestination(&Buffer, '\0');
}
void FormatWriteString(char *Dest, size_t N, const char *Fmt, ...)
{
    va_list Args;
    va_start(Args, Fmt);
    FormatWriteStringVa(Dest, N, Fmt, Args);
    va_end(Args);
}
size_t FormatCStringLength(const char *String)
{
    size_t L = 0;
    while (*String) {
        String++;
        L++;
    }

    return L;
}
