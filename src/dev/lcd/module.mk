ifeq ($(LCD),HD44780)
$(TARGET): src/dev/lcd/hd44780.o
clean: src/dev/lcd/hd44780.o
CCFLAGS+=-DLCD_MODULE
endif

ifeq ($(LCD_BUS),I2C)
CCFLAGS+=-DLCD_I2C_BUS
endif

ifdef LCD_ADD
CCFLAGS+=-DLCD_ADDRESS=$(LCD_ADD)
endif