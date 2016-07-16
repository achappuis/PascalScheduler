/*
Copyright Aurélien Chappuis (2014)

aurelienchappuis@ymail.com

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#include "pwm.h"
#include "../gpio/gpio.h"
#include "xmc1100.h"

int
ccu4_pwm_open(struct vnode UNUSED *vnode)
{
    // P0.5 -> Push Pull ALT4
    GPIO_PIN_MUX(P0_IOCR4, GPIO_OUTPUT_PP | GPIO_OUTPUT_ALT4 , GPIO_PI5);

    //SCU_CCUCON = 0x1;
    CCU4_GIDLC = 0x101; // Enable module and Enable timer slice 0

    CCU4_CC40PRS   = 100; // Set period
    CCU4_CC40CRS   = 5; // Set duty cycle
    CCU4_CC40TIMER = 0;
    CCU4_GCSS = 0x1;// request Shadow transfer

    CCU4_CC40TCSET = 1; // Start Timer

    return 0;
}

int
ccu4_pwm_close(struct vnode UNUSED *vnode)
{
    return 0;
}

int
ccu4_pwm_write(struct vnode UNUSED *vnode, struct uio UNUSED *uio)
{
    return 0;
}

int
ccu4_pwm_read(struct vnode UNUSED *vnode, struct uio UNUSED *uio)
{
    return 0;
}

int
ccu4_pwm_ioctl(struct vnode UNUSED *vnode, uint8_t code, void *data)
{
    switch (code) {
    case PWM_IOCTL_SET_DUTY:
	CCU4_CC40CRS   = *(uint32_t*)data;
	break;
    case PWM_IOCTL_SET_PERIODE:
	CCU4_CC40PRS   = *(uint32_t*)data;
	break;
    case PWM_IOCTL_SET_UPDATE:
	CCU4_GCSS = 0x1;
	break;
    }

    return 0;
}

struct dev_ops pwm = { ccu4_pwm_open, ccu4_pwm_close, ccu4_pwm_read, ccu4_pwm_write, ccu4_pwm_ioctl };
