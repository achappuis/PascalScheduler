LIBSPEC=-L /opt/newlib/arm-none-eabi/lib/armv6-m
INCDIRS=-Isrc/lib/libc/include -Isrc/include
LIBS=/usr/lib/gcc/arm-none-eabi/4.8/armv6-m/libgcc.a

CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
RM=rm

CCFLAGS=-Wall -Wextra -mcpu=cortex-m0 -mthumb -O0 -g -fno-builtin -nostdlib
ASFLAGS=
LDFLAGS=
