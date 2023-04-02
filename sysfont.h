#include <stdint.h>

const uint8_t SysFont_Blank[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};
const uint8_t SysFont_A[5] = {
    0b00100,
    0b01110,
    0b01010,
    0b11111,
    0b10001,
};
const uint8_t SysFont_B[5] = {
    0b11110,
    0b10001,
    0b11110,
    0b10001,
    0b11110
};
const uint8_t SysFont_C[8] = {
    0b01111,
    0b10000,
    0b10000,
    0b10000,
    0b01111
};
const uint8_t SysFont_D[8] = {
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b11110
};
const uint8_t SysFont_E[8] = {
    0b11111,
    0b10000,
    0b11111,
    0b10000,
    0b11111
};
const uint8_t SysFont_F[8] = {
    0b11111,
    0b10000,
    0b11111,
    0b10000,
    0b10000
};
const uint8_t SysFont_G[8] = {
    0b11111,
    0b10000,
    0b11111,
    0b10001,
    0b11111
};
const uint8_t SysFont_H[8] = {
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b10001
};
const uint8_t SysFont_I[8] = {
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b11111
};
const uint8_t SysFont_J[8] = {
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b11100
};
const uint8_t SysFont_K[8] = {
    0b10001,
    0b10010,
    0b11100,
    0b10010,
    0b10001
};
const uint8_t SysFont_L[8] = {
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111
};
const uint8_t SysFont_M[8] = {
    0b10001,
    0b11011,
    0b10101,
    0b10001,
    0b10001
};
const uint8_t SysFont_N[8] = {
    0b10001,
    0b11001,
    0b10101,
    0b10011,
    0b10001
};
const uint8_t SysFont_O[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b11111
};
const uint8_t SysFont_P[8] = {
    0b11111,
    0b10001,
    0b11111,
    0b10000,
    0b10000
};
const uint8_t SysFont_Q[8] = {
    0b01110,
    0b10001,
    0b10101,
    0b10010,
    0b01101
};
const uint8_t SysFont_R[8] = {
    0b11111,
    0b10001,
    0b11111,
    0b10010,
    0b10001
};
const uint8_t SysFont_S[8] = {
    0b11111,
    0b10000,
    0b11111,
    0b00001,
    0b11111
};
const uint8_t SysFont_T[8] = {
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100
};
const uint8_t SysFont_U[8] = {
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110
};
const uint8_t SysFont_V[8] = {
    0b10001,
    0b10001,
    0b01010,
    0b01010,
    0b00100
};
const uint8_t SysFont_W[8] = {
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b01010
};
const uint8_t SysFont_X[8] = {
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001
};
const uint8_t SysFont_Y[8] = {
    0b10001,
    0b01010,
    0b00100,
    0b00100,
    0b00100
};
const uint8_t SysFont_Z[8] = {
    0b11111,
    0b00010,
    0b00100,
    0b01000,
    0b11111
};
const uint8_t SysFont_0[8] = {
    0b11111,
    0b10011,
    0b10101,
    0b11001,
    0b11111
};
const uint8_t SysFont_1[8] = {
    0b00100,
    0b01100,
    0b00100,
    0b00100,
    0b11111
};
const uint8_t SysFont_2[8] = {
    0b01110,
    0b10001,
    0b00010,
    0b00100,
    0b11111
};
const uint8_t SysFont_3[8] = {
    0b11111,
    0b00001,
    0b11111,
    0b00001,
    0b11111
};
const uint8_t SysFont_4[8] = {
    0b00111,
    0b01001,
    0b11111,
    0b00001,
    0b00001
};
const uint8_t SysFont_5[8] = {
    0b11111,
    0b10000,
    0b11111,
    0b00001,
    0b01111
};
const uint8_t SysFont_6[8] = {
    0b01111,
    0b10000,
    0b11110,
    0b10001,
    0b01110
};
const uint8_t SysFont_7[8] = {
    0b11111,
    0b00010,
    0b00100,
    0b01000,
    0b10000
};
const uint8_t SysFont_8[8] = {
    0b01110,
    0b10001,
    0b01110,
    0b10001,
    0b01110
};
const uint8_t SysFont_9[8] = {
    0b01110,
    0b10001,
    0b01110,
    0b00100,
    0b01000
};

const uint8_t* SysFont_GetGlyph(char c)
{
    if (c == 'A')
    {
        return SysFont_A;
    }
    if (c == 'B')
    {
        return SysFont_B;
    }
    if (c == 'C')
    {
        return SysFont_C;
    }
    if (c == 'D')
    {
        return SysFont_D;
    }
    if (c == 'E')
    {
        return SysFont_E;
    }
    if (c == 'F')
    {
        return SysFont_F;
    }
    if (c == 'G')
    {
        return SysFont_G;
    }
    if (c == 'H')
    {
        return SysFont_H;
    }
    if (c == 'I')
    {
        return SysFont_I;
    }
    if (c == 'J')
    {
        return SysFont_J;
    }
    if (c == 'K')
    {
        return SysFont_K;
    }
    if (c == 'L')
    {
        return SysFont_L;
    }
    if (c == 'M')
    {
        return SysFont_M;
    }
    if (c == 'N')
    {
        return SysFont_N;
    }
    if (c == 'O')
    {
        return SysFont_O;
    }
    if (c == 'P')
    {
        return SysFont_P;
    }
    if (c == 'Q')
    {
        return SysFont_Q;
    }
    if (c == 'R')
    {
        return SysFont_R;
    }
    if (c == 'S')
    {
        return SysFont_S;
    }
    if (c == 'T')
    {
        return SysFont_T;
    }
    if (c == 'U')
    {
        return SysFont_U;
    }
    if (c == 'V')
    {
        return SysFont_V;
    }
    if (c == 'W')
    {
        return SysFont_W;
    }
    if (c == 'X')
    {
        return SysFont_X;
    }
    if (c == 'Y')
    {
        return SysFont_Y;
    }
    if (c == 'Z')
    {
        return SysFont_Z;
    }
    if (c == '0')
    {
        return SysFont_0;
    }
    if (c == '1')
    {
        return SysFont_1;
    }
    if (c == '2')
    {
        return SysFont_2;
    }
    if (c == '3')
    {
        return SysFont_3;
    }
    if (c == '4')
    {
        return SysFont_4;
    }
    if (c == '5')
    {
        return SysFont_5;
    }
    if (c == '6')
    {
        return SysFont_6;
    }
    if (c == '7')
    {
        return SysFont_7;
    }
    if (c == '8')
    {
        return SysFont_8;
    }
    if (c == '9')
    {
        return SysFont_9;
    }
    return SysFont_Blank;
}
