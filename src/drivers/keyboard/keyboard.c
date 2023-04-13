#include "keyboard.h"
#include "../../io.h"
#include "../../mem.h"

// KEYBOARD DRIVER
// TODO: Handle fake shifts properly
// TODO: Handle keypad

static keyboard GlobalKeyboard;
static keyboard_key GlobalKeys[128];
static size_t GlobalKeysCount;

const char TransLo[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, '\t', 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

const char TransHi[128] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, '\t', 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 
    'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '
};

uint8_t TransformCharcode(keyboard *Kbd, uint8_t Key) {
    if (Key >= 128) return 0;
    uint8_t tl = TransLo[Key];
    
    if (Kbd->LShift || Kbd->RShift) {
        if (Kbd->CapsLock && tl >= 'a' && tl <= 'z') {
            return tl;
        }
        return TransHi[Key];
    }
    if (Kbd->CapsLock && tl >= 'a' && tl <= 'z') {
        return TransHi[Key];
    }
    return TransLo[Key];
}
#if 0
static uint8_t IsFull() {
    return IO_In8(0x64)&1;
}
#endif

static uint8_t ReadKey() {
    return IO_In8(0x60);
}

static void Keyboard_HandleInterrupt_Internal(keyboard *Kbd, keyboard_key *Keys, uint32_t KeysMax, uint32_t *KeysLen)
{
    uint8_t Alternative = 0;
    if (*KeysLen >= KeysMax) {
        return;
    }
     
    uint8_t K = ReadKey();
    if (K == 0xE0) {
        Alternative = 1;
        K = ReadKey();
    }
    if (K == 0xAA) {
        Kbd->LShift = 0;
        return;
    }
        
    uint8_t Released = K >= 128;
    if (Released) return;
    if (Alternative) K += 128;
    
    switch (K) {
        case KEY_LSHIFT: case KEY_FKLSHIFT: 
            Kbd->LShift = !Released; 
            break;
        case KEY_RSHIFT: case KEY_FKRSHIFT:
            Kbd->RShift = !Released; 
            break;
        case KEY_LCTRL:       Kbd->LCtrl = !Released; break;
        case KEY_RCTRL:       Kbd->RCtrl = !Released; break;
        case KEY_LALT:        Kbd->LAlt = !Released; break;
        case KEY_RALT:        Kbd->RAlt = !Released; break;
        case KEY_CAPS_LOCK:   if (!Released) Kbd->CapsLock = !Kbd->CapsLock; break;
        
        case 0: return; // ran out of input
    }
    
    Keys[(*KeysLen)++] = (keyboard_key){
        Kbd->CapsLock,
        Kbd->LShift,
        Kbd->RShift,
        Kbd->LCtrl,
        Kbd->RCtrl,
        Kbd->LAlt,
        Kbd->RAlt,
        Kbd->LShift||Kbd->RShift,
        Kbd->LCtrl||Kbd->RCtrl,
        Kbd->LAlt||Kbd->RAlt,
        Released, K,
        TransformCharcode(Kbd, K),
        K < 128 ? TransLo[K] : 0
    };
    Alternative = 0;
}

void Keyboard_HandleInterrupt()
{
    Keyboard_HandleInterrupt_Internal(&GlobalKeyboard, GlobalKeys, 128, &GlobalKeysCount);
}

void Keyboard_CollectEvents(keyboard *Kbd, keyboard_key *Keys, uint32_t KeysMax, uint32_t *KeysLen)
{ 
    size_t KeysToCopy = KeysMax;
    if (GlobalKeysCount < KeysToCopy) {
        KeysToCopy = GlobalKeysCount;
    }

    memcpy(Kbd, &GlobalKeyboard, sizeof GlobalKeyboard);
    memcpy(Keys, &GlobalKeys, sizeof(keyboard_key) * KeysToCopy);

    *KeysLen = GlobalKeysCount;
    //ConPrintf("Keys: %d\n", (int)GlobalKeysCount);
    GlobalKeysCount = 0;
}
