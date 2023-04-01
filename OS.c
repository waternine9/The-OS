#include <stdint.h>

void OS_Start() {
  const char *string = "HELLO";
  for (int i = 0; string[i]; i++) {
    *((uint8_t*)(0xB8000+i*2)) = string[i];
  }
}
