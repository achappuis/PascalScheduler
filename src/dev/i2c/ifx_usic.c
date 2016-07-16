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

#include "i2c.h"
#include "ifx_usic.h"
#include "../gpio/gpio.h"

#include <xmc_gpio.h>
#include <xmc_i2c.h>

static XMC_I2C_CH_CONFIG_t i2c_config;

enum usic_tdf {
    I2C_WRITE         = 0x000,
    I2C_READ          = 0x200,
    I2C_READ_NACK     = 0x300,
    I2C_START         = 0x400,
    I2C_RESTART       = 0x500,
    I2C_STOP          = 0x600,
};

void
_usic_i2c_send(uint16_t uwData)
{
  while (XMC_USIC_CH_TXFIFO_IsFull(I2C_CHANNEL) == true)
  {
  }

  XMC_USIC_CH_TXFIFO_PutData(I2C_CHANNEL, (uint16_t)uwData);
}

uint8_t
_usic_i2c_read()
{
  return (uint8_t)XMC_USIC_CH_RXFIFO_GetData(I2C_CHANNEL);
}

int
usic_i2c_open(struct vnode *vnode)
{
  vnode->data = &i2c_config;
  XMC_I2C_CH_CONFIG_t *data = (XMC_I2C_CH_CONFIG_t *)vnode->data;

  data->baudrate = 100000U;

  XMC_I2C_CH_Init(I2C_CHANNEL, data);

  /* Configure input stage */
  XMC_I2C_CH_SetInputSource(I2C_CHANNEL, XMC_I2C_CH_INPUT_SDA, USIC0_C0_DX0_P2_1);
  XMC_I2C_CH_SetInputSource(I2C_CHANNEL, XMC_I2C_CH_INPUT_SCL, USIC0_C0_DX1_P2_0);

  /* Configure FIFO */
  XMC_USIC_CH_RXFIFO_Configure(I2C_CHANNEL, 0, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);
  XMC_USIC_CH_TXFIFO_Configure(I2C_CHANNEL, 16, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

  /* Start i2c channel */
  XMC_I2C_CH_Start(I2C_CHANNEL);

  // Enable digital path
  XMC_GPIO_EnableDigitalInput(P2_1);
  XMC_GPIO_EnableDigitalInput(P2_0);
  // Configure pins
  XMC_GPIO_SetMode(P2_1, XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT6);
  XMC_GPIO_SetMode(P2_0, XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT7);

  return 0;
}

int
usic_i2c_close(struct vnode UNUSED *vnode)
{
    XMC_I2C_CH_Stop(I2C_CHANNEL);
    return 0;
}

int
usic_i2c_write(struct vnode UNUSED *vnode, struct uio *uio)
{
    uint8_t *ptr;
    uint8_t timeout =0;

	XMC_I2C_CH_MasterStart(I2C_CHANNEL, vnode->endpt, XMC_I2C_CH_CMD_WRITE);
  uint32_t statusFlag = 0;
	while((statusFlag & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U && timeout < 200)
	{
    statusFlag = XMC_I2C_CH_GetStatusFlag(I2C_CHANNEL);
		timeout++;
	}
	XMC_I2C_CH_ClearStatusFlag(I2C_CHANNEL, XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

  ptr = uio->uio_iov->iov_base;
  for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
    _usic_i2c_send(*ptr);
  }
	XMC_I2C_CH_MasterStop(I2C_CHANNEL);

    return 0;
}

int
usic_i2c_read(struct vnode UNUSED *vnode, struct uio *uio)
{
    uint8_t *ptr;
    uint8_t ans;

	XMC_I2C_CH_MasterStart(I2C_CHANNEL, vnode->endpt, XMC_I2C_CH_CMD_READ);
    ptr = uio->uio_iov->iov_base;
    for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {

      if (uio->uio_resid > 1) {
          XMC_I2C_CH_MasterReceiveAck(I2C_CHANNEL);
      } else {
          XMC_I2C_CH_MasterReceiveNack(I2C_CHANNEL);
      }
      while((XMC_I2C_CH_GetStatusFlag(I2C_CHANNEL) & (XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION |
                                                       XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) == 0U)
      {
        /* wait for ACK */
      }
      XMC_I2C_CH_ClearStatusFlag(I2C_CHANNEL, XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION |
                                               XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);
      ans = _usic_i2c_read();
      *ptr = ans;
    }
	XMC_I2C_CH_MasterStop(I2C_CHANNEL);

    return 0;
}

int
usic_i2c_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
    switch (code) {
    case I2C_IOCTL_SET_PEER:
	vnode->endpt  = *(int*)data;
	break;
    }

    return 0;
}

struct dev_ops i2c = { usic_i2c_open, usic_i2c_close, usic_i2c_read, usic_i2c_write, usic_i2c_ioctl };
