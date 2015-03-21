ifeq ($(UART),IFX_USIC)
$(TARGET): src/dev/uart/ifx_usic.o \
	${XMCLIB}CMSIS/Infineon/XMC1100_series/Source/system_XMC1100.o \
	${XMCLIB}XMClib/src/xmc_uart.o \
	${XMCLIB}XMClib/src/xmc1_usic.o \
	${XMCLIB}XMClib/src/xmc_usic.o \
	${XMCLIB}XMClib/src/xmc1_scu.o \
	${XMCLIB}XMClib/src/xmc_gpio.o	
clean: src/dev/uart/ifx_usic.o
CCFLAGS+=-DUART_MODULE
ifeq ($(UART_USIC),CH0)
CCFLAGS+=-DUART_USIC_CH0
endif
ifeq ($(UART_USIC),CH1)
CCFLAGS+=-DUART_USIC_CH1
endif
endif

ifeq ($(UART_BAUD),19200)
CCFLAGS+=-DUART_19200
endif
ifeq ($(UART_BAUD),115200)
CCFLAGS+=-DUART_115200
endif
