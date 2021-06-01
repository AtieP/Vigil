KERNEL = vigil.elf

CC = x86_64-elf-gcc
LD = x86_64-elf-ld
AS = nasm

CFILES := $(shell find . -type f -name '*.c')
LDFILE = linker.ld
ASFILES := $(shell find . -type f -name '*.asm')
OFILES := $(CFILES:.c=.o) $(ASFILES:.asm=.o)
HFILES := $(shell find . -type f -name '*.h')

# User controllable flags
CFLAGS = -Wall -Wextra -O2 -pipe

# Internal flags, should not be changed
INTERNALCFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-mcmodel=kernel \
	-fno-stack-protector \
	-mno-red-zone \
	-mno-mmx \
	-mno-80387 \
	-mno-sse \
	-mno-sse2 \
	-mno-3dnow \
	-fno-pic \
	-no-pie \
	-I. \
	-Ifw/acpi/aml/lai/include \
	-fsanitize=undefined

INTERNALLDFLAGS := \
	-static \
	-nostdlib \
	-z max-page-size=0x1000

INTERNALASFLAGS := \
	-f elf64

$(KERNEL): $(OFILES)
	$(LD) -T$(LDFILE) $(INTERNALLDFLAGS) $(OFILES) -o $@

%.o: %.asm
	$(AS) $(INTERNALASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL)
