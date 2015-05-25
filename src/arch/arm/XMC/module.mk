LIBSPEC+=-L ${XMCLIB}
INCDIRS+=-I ${XMCLIB}XMClib/inc/ \
		-I ${XMCLIB}CMSIS/Include/ \
		-I ${XMCLIB}CMSIS/Infineon/XMC1100_series/Include/ \
		-I src/arch/arm/XMC/include/

LD_SCRIPT=src/arch/arm/XMC/linker_script.ld
CCFLAGS+=-mcpu=cortex-m0 -mthumb -DXMC1100_Q024x0008 -DTARGET_XMC

$(TARGET): src/arch/arm/XMC/init.o

clean: src/arch/arm/XMC/init.o
