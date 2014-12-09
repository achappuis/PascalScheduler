ifeq ($(PWM),IFX_CCU4)

$(TARGET): src/dev/pwm/ifx_ccu4.o
clean: src/dev/pwm/ifx_ccu4.o

CCFLAGS+=-DPWM_MODULE

endif
