LIBSPEC=-L/opt/newlib/arm-none-eabi/lib/armv6-m
INCDIRS=-Isrc/lib/libc/include -Isrc/include
LIBS=/usr/lib/gcc/arm-none-eabi/4.8/armv6-m/libgcc.a

CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
RM=rm

#CCFLAGS=-Werror -Wall -Wextra -O0 -g -fno-builtin -nostdlib
CCFLAGS=-Wall -Wextra -O0 -g -fno-builtin -nostdlib
CCFLAGS+= -funsigned-char
CCFLAGS+= -fwrapv
CCFLAGS+= -Wmissing-format-attribute
# CCFLAGS+= -Wpointer-arith
CCFLAGS+= -Wformat-nonliteral
CCFLAGS+= -Winit-self
CCFLAGS+= -Wwrite-strings
CCFLAGS+= -Wshadow
CCFLAGS+= -Wenum-compare
CCFLAGS+= -Wempty-body
CCFLAGS+= -Wparentheses
CCFLAGS+= -Wcast-align
CCFLAGS+= -Wstrict-aliasing
# CCFLAGS+= --pedantic-errors
CCFLAGS+=-DSTACK_DEBUG

ASFLAGS=
LDFLAGS=
