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

#include <xmc_gpio.h>
#include <xmc_uart.h>

#define __UNUSED(x) (void)(x)

static XMC_UART_CH_CONFIG_t uart_config;

uint8_t
_usic_uart_send(uint8_t uwData)
{
  while (XMC_USIC_CH_TXFIFO_IsFull(UART_CHANNEL) == true)
  {
  }

  XMC_USIC_CH_TXFIFO_PutData(UART_CHANNEL, (uint16_t)uwData);
  return 0;
}

uint8_t
_usic_uart_read()
{
  while (XMC_USIC_CH_RXFIFO_IsEmpty(UART_CHANNEL) == true)
  {
  }

  return (uint8_t)XMC_USIC_CH_RXFIFO_GetData(UART_CHANNEL);
}

int
usic_uart_open(struct vnode *vnode)
{
  vnode->data = &uart_config;
  XMC_UART_CH_CONFIG_t *data = (XMC_UART_CH_CONFIG_t *)vnode->data;

  data->baudrate = UART_BAUD_RATE;
  data->data_bits = 8U;
  data->parity_mode = XMC_USIC_CH_PARITY_MODE_NONE;
  data->stop_bits = 1U;

  XMC_UART_CH_Init(UART_CHANNEL, data);

  /* Configure input stage */
  XMC_UART_CH_SetInputSource(UART_CHANNEL, XMC_UART_CH_INPUT_RXD, UART_INPUT_SOURCE);

  /* Configure FIFO */
  XMC_USIC_CH_RXFIFO_Configure(UART_CHANNEL, 32, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);
  XMC_USIC_CH_TXFIFO_Configure(UART_CHANNEL, 48, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

  /* Start UART channel */
  XMC_UART_CH_Start(UART_CHANNEL);

  /* Configure pins */
  XMC_GPIO_SetMode(UART_TX_PIN, UART_TX_ALT);
  XMC_GPIO_SetMode(UART_RX_PIN, UART_RX_ALT);
  return 0;
}

int
usic_uart_close(struct vnode *vnode)
{
    __UNUSED(vnode);
    XMC_UART_CH_Stop(UART_CHANNEL);
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
      ans = _usic_uart_read();
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
