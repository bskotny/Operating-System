dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

bcc -ansi -c -o shelll_c.o shelll.c
as86 lib.asm -o lib_asm.o
ld86 -o shelll -d shelll_c.o lib_asm.o 
dd if=shelll of=floppya.img bs=512 conv=notrunc seek=3


dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30
./loadFile message.txt
./loadFile shelll
./loadFile tstpr2
./loadFile phello



