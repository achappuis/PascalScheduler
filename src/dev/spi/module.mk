ifeq ($(SPI),IFX_USIC)
$(TARGET): src/dev/spi/ifx_usic.o \
	${XMCLIB}CMSIS/Infineon/XMC1100_series/Source/system_XMC1100.o \
	${XMCLIB}XMClib/src/xmc_spi.o \
	${XMCLIB}XMClib/src/xmc1_usic.o \
	${XMCLIB}XMClib/src/xmc_usic.o \
	${XMCLIB}XMClib/src/xmc1_scu.o \
	${XMCLIB}XMClib/src/xmc1_gpio.o \
	${XMCLIB}XMClib/src/xmc_gpio.o
clean: src/dev/spi/ifx_usic.o
CCFLAGS+=-DSPI_MODULE
ifeq ($(SPI_USIC),CH0)
CCFLAGS+=-DSPI_USIC_CH0
endif
ifeq ($(SPI_USIC),CH1)
CCFLAGS+=-DSPI_USIC_CH1
endif
endif
