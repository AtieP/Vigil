# Vigil
Vigil is a x86_64 operating system.

![Vigil](vigil.png?raw=true "Vigil")

# Features
- Booted by Limine using the stivale2 protocol
- Physical memory manager
- Virtual memory manager
- "Slab" kernel heap allocator
- ACPI support thanks to LAI
- APIC support
- UBSan
- Basic kernel threading

# WIP
- POSIX compatibility
- VFS
- PCI Express drivers
- Advanced scheduling and multicore
- Userspace and GUI

## Building
# Prerequisites
Get an x86_64-elf-gcc compiler and NASM. Then, run `make tools` (this will download echfs-utils and limine). To build the kernel, run `make`. To run it using QEMU, simply run `make run-bios`. If you wish to run it in real hardware using BIOS, simply flash `vigil.hdd` on a disk drive. If the computer supports UEFI instead, you must make a EFI partition which contains 3rdparty/limine/BOOTX64, limine.cfg and kernel/vigil.elf.
