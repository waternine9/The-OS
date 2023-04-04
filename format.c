#include "format.h"

typedef struct {
    char *Dest;
    size_t N;
    size_t I;
} destination;

const char *BASE_CHARS = "0123456789ABCDEF";

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
static void WriteDestinationBaseN(destination *Dest, size_t Value, size_t Base)
{
    if (Base == 1) return;
    if (Base > 16) return;

    size_t ValueCopy = Value;
    size_t End = 0;
    do {
        ValueCopy /= Base;
        End += 1;
    } while (ValueCopy);

    char Buf[64] = { 0 };
    do {
        size_t Index = Value % Base;
        Value /= Base;

        Buf[--End] = BASE_CHARS[Index];
    } while (Value);

    WriteDestinationStr(Dest, Buf);
}
static void WriteDestinationInt(destination *Dest, int Value)
{
    if (Value < 0) {
        WriteDestination(Dest, '-');
        Value = -Value;
    }

    WriteDestinationBaseN(Dest, Value, 10);
}
static void WriteDestinationUint(destination *Dest, unsigned int Value)
{
    WriteDestinationBaseN(Dest, Value, 10);
}
static void WriteDestinationHex(destination *Dest, unsigned int Value)
{
    WriteDestinationBaseN(Dest, Value, 16);
}
void FormatWriteStringVa(char *Dest, size_t N, const char *Fmt, va_list Va)
{
    if (N == 0) return;

    destination Buffer = { 0 };
    Buffer.Dest = Dest;
    Buffer.N = N-1;

    for (int I = 0; Fmt[I]; I++) {
        switch (Fmt[I]) {
            case '%':
                I++;
                switch (Fmt[I]) {
                    case 's':
                        WriteDestinationStr(&Buffer, va_arg(Va, const char *));
                        break;
                    case 'd':
                        WriteDestinationInt(&Buffer, va_arg(Va, int));
                        break;
                    case 'u':
                        WriteDestinationUint(&Buffer, va_arg(Va, unsigned int));
                        break;
                    case 'x':
                        WriteDestinationHex(&Buffer, va_arg(Va, unsigned int));
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
