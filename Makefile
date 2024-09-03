CC = aarch64-linux-gnu-gcc
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -ffreestanding -O0 -Wall -Wextra -g -I include
LDFLAGS = -nostdlib

SRC_DIR = src
BUILD_DIR = build

SRCS = $(SRC_DIR)/boot/start.S \
       $(SRC_DIR)/kernel/kernel.c \
       $(SRC_DIR)/kernel/pmm.c \
       $(SRC_DIR)/kernel/shell.c \
       $(SRC_DIR)/kernel/fs.c \
       $(SRC_DIR)/drivers/uart.c \
       $(SRC_DIR)/lib/string.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

TARGET = kernel.bin

$(TARGET): $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -T linker/linker.ld -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(@D)
	$(AS) $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a53 -kernel $< -nographic -m 128M

debug: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a53 -kernel $< -nographic -m 128M -s -S

.PHONY: clean run
