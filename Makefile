include Config.inc
include Makefile.inc

TARGET=bin/main.elf
all:$(TARGET) doc

build: $(TARGET)

$(TARGET):
	@mkdir -p bin/
	@$(CC) $^ $(LIBS) $(INCDIRS) -T tools/linker_script.ld -nostartfiles -fno-builtin -nostdlib -o $(TARGET)
	@echo "\nDone."

%.o: %.c
	@$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"
	
%.o: %.s
	@$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: doc clean

clean: 
	@rm $^

doc:
	@echo "\nGenerating documentation"
	@mkdir -p doc/natural/
	@naturaldocs -i src/ -o HTML doc/natural/ -p doc/

include src/module.mk
