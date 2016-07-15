ifeq ($(PWM),IFX_CCU4)

OBJECTS+= src/dev/pwm/ifx_ccu4.o

CCFLAGS+=-DPWM_MODULE

endif
