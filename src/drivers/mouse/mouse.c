#include "../../io.h"
#include "../../format.h"
#include "mouse.h"

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

click_animation ClickAnimation; 


int8_t MouseByte[3];

void MouseWait(uint8_t AType) //unsigned char
{
  uint16_t TimeOut=100000; //unsigned int
  if(AType==0)
  {
    while(TimeOut--) //Data
    {
      if((IO_In8(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(TimeOut--) //Signal
    {
      if((IO_In8(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

void MouseWrite(uint8_t AWrite) //unsigned char
{
  //Wait to be able to send a command
  MouseWait(1);
  //Tell the mouse we are sending a command
  IO_Out8(0x64, 0xD4);
  //Wait for the final part
  MouseWait(1);
  //Finally write
  IO_Out8(0x60, AWrite);
}
uint8_t MouseRead()
{
  //Get's response from mouse
  MouseWait(0);
  return IO_In8(0x60);
}
void MouseInstall()
{
  uint8_t _status;  //unsigned char

  //Enable the auxiliary mouse device
  MouseWait(1);
  IO_Out8(0x64, 0xA8);
 
  //Enable the interrupts
  MouseWait(1);
  IO_Out8(0x64, 0x20);
  MouseWait(0);
  _status=(IO_In8(0x60) | 2);
  MouseWait(1);
  IO_Out8(0x64, 0x60);
  
  MouseWait(1);
  IO_Out8(0x60, _status);
  
  MouseWrite(0xFF);
  MouseRead();  //Acknowledge

  //Tell the mouse to use default settings
  MouseWrite(0xF6);
  MouseRead();  //Acknowledge
 
  //Enable the mouse
  MouseWrite(0xF4);
  MouseRead();  //Acknowledge

  

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
  
  IO_Out8(0x60, 0xF4); // Enable keyboard data reporting (scanning)
  // Wait for the keyboard's ACK (0xFA)

  IO_Out8(0x64, 0xD4); // Prepare to send command to the mouse
  IO_Out8(0x60, 0xF4); // Enable mouse data reporting (movement and button events)
  // Wait for the mouse's ACK (0xFA)
}