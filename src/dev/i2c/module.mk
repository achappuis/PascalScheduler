ifeq ($(I2C),IFX_USIC)
OBJECTS+= src/dev/i2c/ifx_usic.o \
	${XMCLIB}src/xmc_i2c.o
CCFLAGS+=-DI2C_MODULE
ifeq ($(I2C_USIC),CH0)
CCFLAGS+=-DI2C_USIC_CH0
endif
ifeq ($(I2C_USIC),CH1)
CCFLAGS+=-DI2C_USIC_CH1
endif

endif
