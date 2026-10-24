
AS      := clang
CXX     := clang++
LD      := ld.lld

BUILD   := build
ISO_DIR := iso
KERNEL  := $(BUILD)/harbouros.bin
ISO     := $(BUILD)/harbouros.iso

ASFLAGS  := -target i686-elf
CXXFLAGS := -target i686-elf -ffreestanding -fno-exceptions -fno-rtti \
            -fno-stack-protector -fno-pic -fno-pie
LDFLAGS  := -m elf_i386 -T linker.ld -nostdlib

.PHONY: all run clean

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot/boot.s | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(BUILD)/boot.o $(BUILD)/kernel.o linker.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/boot.o $(BUILD)/kernel.o

$(ISO): $(KERNEL) iso/boot/grub/grub.cfg
	cp $(KERNEL) $(ISO_DIR)/boot/harbouros.bin
	grub-file --is-x86-multiboot $(ISO_DIR)/boot/harbouros.bin
	grub-mkrescue -o $@ $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -boot d -cdrom $(ISO)

clean:
	rm -f $(BUILD)/boot.o $(BUILD)/kernel.o $(KERNEL) $(ISO)
