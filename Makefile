# Variables for dependencies and the output hard disk file
KERNEL = vigil.elf
OS = vigil.hdd

# Variables for programs
ECHFS_UTILS=./3rdparty/echfs/echfs-utils
LIMINE_INSTALL=./3rdparty/limine/limine-install
LIMINE_SYS=./3rdparty/limine/limine.sys
LIMINE_EFI=./3rdparty/limine/BOOTX64.EFI
QEMU=qemu-system-x86_64

# Flags for some programs
QEMU_FLAGS=-d int -machine q35 -smp 4 -hda $(OS) -debugcon stdio -m 2G -enable-kvm

# Miscelaneous
LIMINE_CFG=limine.cfg

# Building
os-bios:
	$(MAKE) clean
	$(MAKE) -C kernel
	dd if=/dev/zero of=$(OS) bs=1M count=0 seek=64

	parted -s $(OS) mklabel gpt
	parted -s $(OS) mkpart primary 2048s 100%

	$(ECHFS_UTILS) -g -p0 $(OS) quick-format 512
	$(ECHFS_UTILS) -g -p0 $(OS) import kernel/$(KERNEL) boot/$(KERNEL)
	$(ECHFS_UTILS) -g -p0 $(OS) import $(LIMINE_CFG) boot/limine.cfg
	$(ECHFS_UTILS) -g -p0 $(OS) import $(LIMINE_SYS) boot/limine.sys
	$(LIMINE_INSTALL) $(OS)

# Tooling
tools:
	$(MAKE) 3rdparty/echfs
	$(MAKE) 3rdparty/limine
	$(MAKE) lai

3rdparty/echfs:
	mkdir -p 3rdparty/echfs
	git clone https://github.com/echfs/echfs 3rdparty/echfs --depth=1
	$(MAKE) -C 3rdparty/echfs/ echfs-utils

3rdparty/limine:
	mkdir -p 3rdparty/limine
	git clone https://github.com/limine-bootloader/limine 3rdparty/limine --depth=1 --branch=latest-binary
	$(MAKE) -C 3rdparty/limine/ limine-install

lai:
	git clone https://github.com/managarm/lai kernel/fw/acpi/aml/lai --depth=1

# Testing
run-bios:
	$(QEMU) $(QEMU_FLAGS)

run-uefi:
	$(QEMU) $(QEMU_FLAGS) $(OS) -bios ./3rdparty/ovmf/OVMF.fd

# Misc
clean:
	$(MAKE) -C kernel clean
	rm -f $(OS)