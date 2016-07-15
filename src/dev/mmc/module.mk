ifeq ($(MMC),MMC_SPI)
OBJECTS+= src/dev/mmc/mmc_spi.o \
	${XMCLIB_CMSIS}Infineon/XMC1100_series/Source/system_XMC1100.o \
	${XMCLIB}src/xmc_spi.o \
	${XMCLIB}src/xmc1_usic.o \
	${XMCLIB}src/xmc_usic.o \
	${XMCLIB}src/xmc1_scu.o \
	${XMCLIB}src/xmc1_gpio.o \
	${XMCLIB}src/xmc_gpio.o
CCFLAGS+=-DMMC_MODULE
endif
