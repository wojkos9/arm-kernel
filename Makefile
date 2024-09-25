TARGET = arm-none-eabi
CC = $(TARGET)-gcc
AS = $(TARGET)-as
LD = $(TARGET)-ld

DEFS = -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_LONG_LONG

CFLAGS = -g -mcpu=cortex-a15 -nostdlib -nostartfiles -ffreestanding -std=c99 -I . -I ./printf $(DEFS)

all: kernel

start.o: start.s
	$(AS) -g -o $@ $<

OBJS = start.o main.o cpu.o gic.o printf/printf.o
HDRS = $(wildcard *.h)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

kernel: link.ld $(OBJS) $(HDRS)
	$(LD) -T $< -o $@ $(OBJS)

QEMU_CMD = qemu-system-arm -M virt,highmem=off -m 128 -nographic

run: kernel
	$(QEMU_CMD) -kernel $<

debug: kernel
	$(QEMU_CMD) -gdb tcp::1235 -S -kernel $<

clean:
	rm *.o printf/*.o kernel || true
