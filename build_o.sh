python3 buildresources.py
mkdir -p bin
gcc -c -m32 src/*.c src/drivers/*/*.c src/fonts/*.c -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nodefaultlibs -fno-builtin -fno-pic -O3
nasm -f elf32 kernel/boot.s -o boot.o
ld -m elf_i386 *.o -T link.ld -o bin/boot.img
rm *.o
rm resources.bin

qemu-img resize -fraw bin/boot.img 512M
