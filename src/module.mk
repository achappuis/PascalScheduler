$(TARGET): src/main.o \
	src/c_init.o\
	src/circular_buffer.o\
	src/init.o\
	src/isr.o\
	src/scheduler.o\
	src/signals.o\
	src/syscall.o\


clean: src/main.o \
	src/c_init.o\
	src/circular_buffer.o\
	src/init.o\
	src/isr.o\
	src/scheduler.o\
	src/signals.o\
	src/syscall.o\

include src/dev/module.mk
include src/sys/module.mk
include src/lib/module.mk
