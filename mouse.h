#include "io.h"
#include "format.h"
extern void kernel_InitMouse();

int MouseX = 320;
int MouseY = 240;
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

void MouseHandler(uint8_t sensitivity)
{
    static int state = 0;

    switch (state)
    {
        case 0:
        {
            uint8_t firstByte = IO_In8(0x60);
            if ((firstByte & 0x08) == 0x08)
            {
                MouseByte[0] = firstByte;
                state = 1;
            }
            break;
        }
        case 1:
        {
            MouseByte[1] = IO_In8(0x60);
            state = 2;
            break;
        }
        case 2:
        {
            MouseByte[2] = IO_In8(0x60);
            state = 0;

            // Calculate the relative mouse movement
            MouseX += (int8_t)MouseByte[1] / sensitivity;
            MouseY -= (int8_t)MouseByte[2] / sensitivity; // Subtract to invert Y-axis (optional)

            KPrintf("MOUSE MOVE %d %d\n", (int)MouseByte[1], (int)MouseByte[2]);
            break;
        }
        default:
            state = 0;
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

  
}