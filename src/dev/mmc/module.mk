ifeq ($(MMC),MMC_SPI)
$(TARGET): src/dev/mmc/mmc_spi.o \
	${XMCLIB}CMSIS/Infineon/XMC1100_series/Source/system_XMC1100.o \
	${XMCLIB}XMClib/src/xmc_spi.o \
	${XMCLIB}XMClib/src/xmc1_usic.o \
	${XMCLIB}XMClib/src/xmc_usic.o \
	${XMCLIB}XMClib/src/xmc1_scu.o \
	${XMCLIB}XMClib/src/xmc1_gpio.o \
	${XMCLIB}XMClib/src/xmc_gpio.o
clean: src/dev/mmc/mmc_spi.o
CCFLAGS+=-DMMC_MODULE
endif
