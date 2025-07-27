# Cross-compiler toolchain prefix
ARMGNU ?= aarch64-linux-gnu

# Directories
BUILD_DIR = build
SRC_DIR = src

# Compiler and Assembler flags
# Added include paths for src/headers and src/libc/headers
COPS = -Wall -nostdlib -nostdinc -ffreestanding -I$(SRC_DIR)/headers -I$(SRC_DIR)/libc/headers -mgeneral-regs-only
ASMOPS = -I$(SRC_DIR)/headers

# --- Source File Discovery ---
# Use 'find' to recursively locate all .c and .S files
C_FILES = $(shell find $(SRC_DIR) -name '*.c')
ASM_FILES = $(shell find $(SRC_DIR) -name '*.S')

# --- Object File Mapping ---
# Map source files to object files, preserving directory structure in the build folder
# e.g., src/libc/stdio/printf.c -> build/libc/stdio/printf.o
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_FILES))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%.o,$(ASM_FILES))

# --- Dependency File Mapping ---
DEP_FILES = $(OBJ_FILES:.o=.d)

# Include dependency files if they exist
-include $(DEP_FILES)

$(info $$OBJ_FILES is [${OBJ_FILES}])

# --- Build Rules ---

# Default target
all: kernel8.img

# Rule to link the kernel
kernel8.img: $(OBJ_FILES) $(SRC_DIR)/linker.ld
	@echo "Linking object files..."
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	@echo "Creating final kernel image..."
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
	@echo "Build complete: kernel8.img"

# Generic rule to compile C source files
# Creates object files in the build directory with the same subdirectory structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling C: $<"
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

# Generic rule to compile Assembly source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	@echo "Assembling: $<"
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

# Rule to clean build artifacts
clean:
	@echo "Cleaning build directory and kernel image..."
	@rm -rf $(BUILD_DIR) kernel8.img
	@echo "Clean complete."

.PHONY: all clean