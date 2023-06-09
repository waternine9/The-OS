mkdir -p bin
gcc -c -m32 src/*.c src/fonts/*.c src/drivers/*/*.c -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nodefaultlibs -fno-builtin -fno-pic
nasm -f elf32 kernel/boot.s -o boot.o
ld -m elf_i386 *.o -T link.ld -o bin/boot.img
rm *.o
