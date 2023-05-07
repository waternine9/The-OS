mkdir -p bin
gcc -c -m32 src/*.cpp -fno-rtti -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nodefaultlibs -fno-builtin -fno-pic -O3
nasm -f elf32 src/boot/boot.asm -o boot.o
ld -m elf_i386 *.o -T link.ld -o bin/boot.img

qemu-img resize bin/boot.img 512M