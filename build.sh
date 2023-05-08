mkdir -p bin
g++ -c -m32 src/*.cpp src/drivers/*/*.cpp -fno-rtti -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nodefaultlibs -fno-builtin -fno-pic -O0
nasm -f elf32 src/boot/boot.asm -o boot.o
ld -m elf_i386 *.o -T link.ld -o bin/boot.img
rm *.o
qemu-img resize bin/boot.img 32M