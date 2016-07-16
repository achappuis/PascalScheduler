include "../../cortex-m0/module.mk"

LIBSPEC+=-L${XMCLIB}
INCDIRS+=-I${XMCLIB}inc/ \
		-I${XMCLIB_CMSIS}Include/ \
		-I${XMCLIB_CMSIS}Infineon/XMC1100_series/Include/

CCFLAGS+=-DXMC1100_T038x0064 -DTARGET_XMC
