ifeq ($(UART),IFX_USIC)
$(TARGET): src/dev/uart/ifx_usic.o
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