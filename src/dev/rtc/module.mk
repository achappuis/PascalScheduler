ifeq ($(RTC),DS1307)
OBJECTS+= src/dev/rtc/ds1307.o
CCFLAGS+=-DRTC_MODULE
endif
