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

static XMC_UART_CH_CONFIG_t uart_config;

uint8_t
_usic_uart_send(uint8_t uwData)
{
  XMC_UART_CH_Transmit(XMC_UART0_CH1, (uint16_t)uwData);
  return 0;
}

uint8_t
_usic_uart_read()
{
  return (uint8_t)XMC_UART_CH_GetReceivedData(XMC_UART0_CH1);
}

int
usic_uart_open(struct vnode *vnode)
{
  vnode->data = &uart_config;
  XMC_UART_CH_CONFIG_t *data = (XMC_UART_CH_CONFIG_t *)vnode->data;

  data->baudrate = 115200;
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
    uint8_t *ptr;

    ptr = uio->uio_iov->iov_base;
    for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
      _usic_uart_send(*ptr);
    }

    return 0;
}

int
usic_uart_read(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(vnode);
    uint8_t *ptr;
    uint8_t ans;

    ptr = uio->uio_iov->iov_base;
    for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
      do {
        ans = _usic_uart_read();
      } while(!ans);

      *ptr = ans;
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
