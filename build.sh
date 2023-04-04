mkdir -p bin
gcc -c -m32 *.c -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nodefaultlibs -fno-builtin -fno-pic
nasm -f elf32 boot.s -o boot.o
ld -m elf_i386 *.o -T link.ld -o bin/boot.img
rm *.o
