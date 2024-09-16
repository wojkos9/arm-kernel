TARGET = arm-none-eabi
CC = $(TARGET)-gcc
AS = $(TARGET)-as
LD = $(TARGET)-ld

DEFS = -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_LONG_LONG

CFLAGS = -g -mcpu=cortex-a15 -nostdlib -nostartfiles -ffreestanding -std=c99 -I ./printf $(DEFS)

all: kernel

start.o: start.s
	$(AS) -g -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel: start.o main.o printf/printf.o | link.ld
	$(LD) -T link.ld -o $@ $^

QEMU_CMD = qemu-system-arm -M virt,highmem=off -cpu cortex-a15 -m 128 -nographic

run: kernel
	$(QEMU_CMD) -kernel $<

debug: kernel
	$(QEMU_CMD) -s -S -kernel $<

clean:
	rm *.o printf/*.o kernel || true