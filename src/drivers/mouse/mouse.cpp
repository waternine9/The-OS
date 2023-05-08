#include "../../io.hpp"
#include "mouse.hpp"

const uint8_t MousePointerBlack[8] = {
        0b11111111,
        0b10000010,
        0b10000100,
        0b10000100,
        0b10000010,
        0b10110001,
        0b11001001,
        0b10000111
};
const uint8_t MousePointerFull[8] = {
        0b11111111,
        0b11111110,
        0b11111100,
        0b11111100,
        0b11111110,
        0b11111111,
        0b11001111,
        0b10000111
};

void MouseWrite(uint8_t AWrite)
{
    IO_Out8(0x64, 0xD4);
    IO_Out8(0x60, AWrite);
}
uint8_t MouseRead()
{
    return IO_In8(0x60);
}
void MouseInstall()
{
    MouseWrite(0xFF);
    MouseRead();

    // Tell the mouse to use default settings
    MouseWrite(0xF6);
    MouseRead();

    // Enable IRQ 12
    IO_Out8(0x64, 0xAD); // Disable first PS/2 port (keyboard)
    IO_Out8(0x64, 0xA7); // Disable second PS/2 port (mouse)

    IO_Out8(0x64, 0x20); // Request current configuration byte
    uint8_t config = IO_In8(0x60); // Read the configuration byte from the data port (0x60)

    config |= 0x03; // Enable IRQ1 (keyboard) and IRQ12 (mouse) by setting bits 0 and 1
    config &= ~0x30; // Clear bits 4 and 5 to enable translation for the first PS/2 port (keyboard)

    IO_Out8(0x64, 0x60); // Set the new configuration byte
    IO_Out8(0x60, config);

    IO_Out8(0x64, 0xAE); // Enable first PS/2 port (keyboard)
    IO_Out8(0x64, 0xA8); // Enable second PS/2 port (mouse)

    IO_Out8(0x64, 0xD4); // Prepare to send command to the mouse
    IO_Out8(0x60, 0xF4); // Enable mouse data reporting (movement and button events)
    // Wait for the mouse's ACK (0xFA)
}