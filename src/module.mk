OBJECTS+= src/main.o \
	src/c_init.o\
	src/circular_buffer.o\
	src/isr.o\
	src/scheduler.o\
	src/signals.o\
	src/syscall.o\
	src/mutex.o\
	src/kernel.o

include src/dev/module.mk
include src/sys/module.mk
include src/lib/module.mk
