LD_SCRIPT=src/arch/arm/cortex-m0/linker_script.ld
CCFLAGS+=-mcpu=cortex-m0 -mthumb

OBJECTS+=src/arch/arm/cortex-m0/init.o \
	src/arch/arm/cortex-m0/isr_s.o \
	src/arch/arm/cortex-m0/scheduler_s.o
