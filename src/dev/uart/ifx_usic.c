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

#include "uart.h"
#include "ifx_usic.h"
#include "../gpio/gpio.h"
#include "xmc1100.h"

#include <xmc_gpio.h>
#include <xmc_uart.h>

#define __UNUSED(x) (void)(x)

uint32_t
_usic_uart_send(uint32_t uwData)
{
  XMC_UART_CH_Transmit(XMC_UART0_CH1, (uint16_t)uwData);
  return 0;
}

uint32_t
_usic_uart_read()
{
  //   if (TRBSR & 0x8) { // RFIFO Empty
	// return 0;
  //   }
  //   while(TRBSR & 0x20);
  //   return (uint32_t)(OUTR & 0xFFFF);
  return (uint32_t)XMC_UART_CH_GetReceivedData(XMC_UART0_CH1);
}

int
usic_uart_open(struct vnode *vnode)
{
  XMC_UART_CH_CONFIG_t *data = (XMC_UART_CH_CONFIG_t *)vnode->data;

  data->baudrate = 9600;
  data->data_bits = 8U;
  data->parity_mode = XMC_USIC_CH_PARITY_MODE_NONE;
  data->stop_bits = 1U;

  XMC_UART_CH_Init(XMC_UART0_CH1, data);
  XMC_UART_CH_SetInputSource(XMC_UART0_CH1, XMC_UART_CH_INPUT_RXD, USIC0_C1_DX0_P0_6);

  /* Start UART channel */
  XMC_UART_CH_Start(XMC_UART0_CH1);

  /* Configure pins */
  XMC_GPIO_SetMode(P0_7, XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7);
  XMC_GPIO_SetMode(P0_6, XMC_GPIO_MODE_INPUT_TRISTATE);

  return 0;
}

int
usic_uart_close(struct vnode *vnode)
{
    __UNUSED(vnode);
    XMC_UART_CH_Stop(XMC_UART0_CH1);
    return 0;
}

int
usic_uart_write(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(vnode);
    uint32_t *ptr;
    int low_add;

#define	SEND_BYTE(a)	  _usic_uart_send(((*(ptr)) & (0xFF << 8*(a))) >> 8*(a) ); uio->uio_resid--

    if (uio->uio_rw != UIO_WRITE) {
	return -1;
    }

    // iov_base should be aligned
    ptr = uio->uio_iov->iov_base;
    low_add = (int)((uint32_t)ptr & 0x3); // lowest 2 bits
    ptr = (uint32_t*)((uint32_t)ptr & (~0x3));
    switch(low_add) {
    case 0:		// Aligned
	break;
    case 1:
	SEND_BYTE(1);
	if(uio->uio_resid > 0) { SEND_BYTE(2); }
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
	break;
    case 2:
	SEND_BYTE(2);
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
	break;
    case 3:
	SEND_BYTE(3);
	break;
    }
    if (uio->uio_resid == 0) {
	return 0;
    }

    for(; uio->uio_resid > 0; ptr++) {
	SEND_BYTE(0);
	if(uio->uio_resid > 0) { SEND_BYTE(1); }
	if(uio->uio_resid > 0) { SEND_BYTE(2); }
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
    }

    return 0;
}

int
usic_uart_read(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(vnode);
    uint32_t *ptr;
    int low_add;
    int offset;
    uint32_t ans;

#define	CLEAR_ANS(__a) *ptr &= ~(0xFF << 8*(__a))
#define	WRITE_ANS(__a) *ptr |= ans << 8*(__a);offset++;offset%=4; uio->uio_resid--
#define READ_INNER(__a) ans = _usic_uart_read();\
	if (ans==0) { return -1; }\
	CLEAR_ANS(__a);\
	WRITE_ANS(__a);

    if (uio->uio_rw != UIO_READ) {
	return -1;
    }

    // iov_base should be aligned
    ptr = uio->uio_iov->iov_base;
    offset = 0;
    low_add = (int)((uint32_t)ptr & 0x3); // lowest 2 bits
    ptr = (uint32_t*)((uint32_t)ptr & (~0x3));
    switch(low_add) {
    case 0:		// Aligned
	break;
    case 1:
	READ_INNER(1);
	if(uio->uio_resid > 0) { READ_INNER(2); }
	if(uio->uio_resid > 0) { READ_INNER(3); }
	break;
    case 2:
	READ_INNER(2);
	if(uio->uio_resid > 0) { READ_INNER(3); }
	break;
    case 3:
	READ_INNER(3);
	break;
    }
    if (uio->uio_resid == 0) {
	return 0;
    }

    for(; uio->uio_resid > 0;uio->uio_resid--, ptr++) {
	READ_INNER(0);
	if(uio->uio_resid > 0) { READ_INNER(1); }
	if(uio->uio_resid > 0) { READ_INNER(2); }
	if(uio->uio_resid > 0) { READ_INNER(3); }
    }

    return 0;
}

int
usic_uart_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
    __UNUSED(vnode);
    __UNUSED(code);
    __UNUSED(data);

    return 0;
}

struct dev_ops uart = { usic_uart_open, usic_uart_close, usic_uart_read, usic_uart_write, usic_uart_ioctl };
