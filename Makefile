include Config.inc
include Makefile.inc

ifndef VERBOSE
S=@
endif

TARGET=bin/main.elf
all:$(TARGET)

build: $(TARGET)

include src/module.mk
include src/arch/arm/soc/XMC1x/module.mk
# include src/arch/arm/STM32F4/module.mk

$(TARGET): $(OBJECTS)
	@mkdir -p bin/
	${S}$(CC) $^ $(LIBS) $(INCDIRS) -T $(LD_SCRIPT) -nostartfiles -fno-builtin -nostdlib -o $(TARGET)
	@echo "\nDone."

%.o: %.c
	${S}$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

%.o: %.s
	${S}$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

%.o: %.S
	${S}$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: check doc clean

check:
	cppcheck --enable=all $(INCDIRS) -DXMC1100_T038x0064 -DTARGET_XMC `find src -name "*.c"`

clean:
	@rm -f $(OBJECTS)

doc:
	@echo "\nGenerating documentation"
	@mkdir -p doc/natural/
	@naturaldocs -i src/ -o HTML doc/natural/ -p doc/

#$(TARGET): vendors/pff3/src/pff.o \
#	vendors/pff3/src/diskio.o
#clean:  vendors/pff3/src/pff.o \
#	vendors/pff3/src/diskio.o
