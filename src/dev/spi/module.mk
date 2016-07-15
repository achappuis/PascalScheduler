ifeq ($(SPI),IFX_USIC)
OBJECTS+= src/dev/spi/ifx_usic.o \
	${XMCLIB_CMSIS}Infineon/XMC1100_series/Source/system_XMC1100.o \
	${XMCLIB}src/xmc_spi.o \
	${XMCLIB}src/xmc1_usic.o \
	${XMCLIB}src/xmc_usic.o \
	${XMCLIB}src/xmc1_scu.o \
	${XMCLIB}src/xmc1_gpio.o \
	${XMCLIB}src/xmc_gpio.o
CCFLAGS+=-DSPI_MODULE
ifeq ($(SPI_USIC),CH0)
CCFLAGS+=-DSPI_USIC_CH0
endif
ifeq ($(SPI_USIC),CH1)
CCFLAGS+=-DSPI_USIC_CH1
endif
endif
