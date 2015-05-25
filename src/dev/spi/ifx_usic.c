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

#include "spi.h"
#include "../gpio/gpio.h"
#include "xmc1100.h"

#include <xmc_gpio.h>
#include <xmc_spi.h>

#define __UNUSED(x) (void)(x)

static XMC_SPI_CH_CONFIG_t spi_config;

int
_usic_spi_enable_slave(int slave)
{
    XMC_SPI_CH_EnableSlaveSelect(XMC_SPI0_CH0, slave);
    // For some reason, I'm not able to correctly drive CS with the previous.
    // TODO: drive with the dedicated functions.
    XMC_GPIO_SetOutputLow(XMC_GPIO_PORT0,9);
    return 0;
}

int
_usic_spi_disable_slave()
{
    XMC_SPI_CH_DisableSlaveSelect(XMC_SPI0_CH0);
    // For some reason, I'm not able to correctly drive CS with the previous.
    XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT0,9);
    return 0;
}

uint8_t
_usic_spi_send(uint8_t uwData)
{
  XMC_SPI_CH_Transmit(XMC_SPI0_CH0, (uint16_t)uwData, XMC_SPI_CH_MODE_STANDARD);
  while((XMC_SPI_CH_GetStatusFlag(XMC_SPI0_CH0) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U);
  XMC_SPI_CH_ClearStatusFlag(XMC_SPI0_CH0, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);
  return 0;
}

uint8_t
_usic_spi_read()
{
  XMC_SPI_CH_Receive(XMC_SPI0_CH0, XMC_SPI_CH_MODE_STANDARD);
  return (uint8_t)XMC_SPI_CH_GetReceivedData(XMC_SPI0_CH0);
}

int
usic_spi_open(struct vnode *vnode)
{
  XMC_GPIO_CONFIG_t rx_pin_config;
  XMC_GPIO_CONFIG_t tx_pin_config;
  XMC_GPIO_CONFIG_t selo_pin_config;
  XMC_GPIO_CONFIG_t clk_pin_config;

  vnode->data = &spi_config;
  XMC_SPI_CH_CONFIG_t *data = (XMC_SPI_CH_CONFIG_t *)vnode->data;

  data->baudrate = 50000;
  data->bus_mode = XMC_SPI_CH_BUS_MODE_MASTER;
  data->selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS;
  data->parity_mode = XMC_USIC_CH_PARITY_MODE_NONE;

  /* Initialize and Start SPI */
  XMC_SPI_CH_Init(XMC_SPI0_CH0, data);
  XMC_SPI_CH_ConfigureShiftClockOutput(XMC_SPI0_CH0,
      XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_ENABLED,
      XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
  XMC_SPI_CH_SetFrameLength(XMC_SPI0_CH0, 64);
  XMC_SPI_CH_SetWordLength(XMC_SPI0_CH0, 8);
  XMC_SPI_CH_SetBitOrderMsbFirst(XMC_SPI0_CH0);
  XMC_SPI_CH_SetInputSource(XMC_SPI0_CH0, XMC_SPI_CH_INPUT_DIN0, USIC0_C0_DX0_P2_0);

  XMC_SPI_CH_Start(XMC_SPI0_CH0);

  XMC_SPI_CH_EnableMasterClock(XMC_SPI0_CH0);

  /* GPIO Input pin configuration */
  rx_pin_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(XMC_GPIO_PORT2, 0, &rx_pin_config);

  /* GPIO Output pin configuration */
  tx_pin_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
  XMC_GPIO_Init(XMC_GPIO_PORT0,15, &tx_pin_config);

  /* GPIO Slave Select line pin configuration */
  //selo_pin_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
  selo_pin_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
  // see _usic_spi_enable_slave
  XMC_GPIO_Init(XMC_GPIO_PORT0,9, &selo_pin_config);

  /* GPIO Clock pin configuration */
  clk_pin_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
  XMC_GPIO_Init(XMC_GPIO_PORT0,8, &clk_pin_config);

  return 0;
}

int
usic_spi_close(struct vnode *vnode)
{
    __UNUSED(vnode);
    XMC_SPI_CH_Stop(XMC_SPI0_CH0);
    return 0;
}

int
usic_spi_write(struct vnode *vnode, struct uio *uio)
{
  __UNUSED(vnode);
  uint8_t *ptr;

  ptr = uio->uio_iov->iov_base;
  for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
    _usic_spi_send(*ptr);
  }

  return 0;
}

int
usic_spi_read(struct vnode *vnode, struct uio *uio)
{
  __UNUSED(vnode);
  uint8_t *ptr;
  uint8_t ans;

  ptr = uio->uio_iov->iov_base;
  for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
      ans = _usic_spi_read();
      *ptr = ans;
  }

  return 0;
}

int
usic_spi_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
    __UNUSED(vnode);

    switch (code) {
    case SPI_IOCTL_ENABLE_SLAVE:
        return _usic_spi_enable_slave(*((int*)data));
        break;
    case SPI_IOCTL_DISABLE_SLAVE:
        return _usic_spi_disable_slave();
        break;
    default:
        break;
    }

    return 0;
}

struct dev_ops spi = { usic_spi_open, usic_spi_close, usic_spi_read, usic_spi_write, usic_spi_ioctl };
