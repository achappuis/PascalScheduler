include Config.inc
include Makefile.inc

TARGET=bin/main.elf
all:$(TARGET)

build: $(TARGET)

include src/arch/arm/XMC/module.mk
# include src/arch/arm/STM32F4/module.mk

$(TARGET):
	@mkdir -p bin/
	@$(CC) $^ $(LIBS) $(INCDIRS) -T $(LD_SCRIPT) -nostartfiles -fno-builtin -nostdlib -o $(TARGET)
	@echo "\nDone."

%.o: %.c
	@$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

%.o: %.s
	@$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

%.o: %.S
	@$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: doc clean

clean:
	@rm $^

doc:
	@echo "\nGenerating documentation"
	@mkdir -p doc/natural/
	@naturaldocs -i src/ -o HTML doc/natural/ -p doc/

$(TARGET): vendors/pff3/src/pff.o \
	vendors/pff3/src/diskio.o
clean:  vendors/pff3/src/pff.o \
	vendors/pff3/src/diskio.o

include src/module.mk
