LIBSPEC=-L /opt/newlib/arm-none-eabi/lib/armv6-m \
    -L ${XMCLIB}
INCDIRS=-I src/lib/libc/include -Isrc/include \
    -I ${XMCLIB}XMClib/inc/ \
    -I ${XMCLIB}CMSIS/Include/ \
    -I ${XMCLIB}CMSIS/Infineon/XMC1100_series/Include/
LIBS=/usr/lib/gcc/arm-none-eabi/4.8/armv6-m/libgcc.a

CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
RM=rm

CCFLAGS=-Wall -Wextra -mcpu=cortex-m0 -mthumb -O0 -g -fno-builtin -nostdlib -DXMC1100_Q024x0008
ASFLAGS=
LDFLAGS=
