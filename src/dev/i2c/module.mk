ifeq ($(I2C),IFX_USIC)

$(TARGET): src/dev/i2c/ifx_usic.o
clean: src/dev/i2c/ifx_usic.o

CCFLAGS+=-DI2C_MODULE

ifeq ($(I2C_USIC),CH0)
CCFLAGS+=-DI2C_USIC_CH0
endif
ifeq ($(I2C_USIC),CH1)
CCFLAGS+=-DI2C_USIC_CH1
endif

endif
