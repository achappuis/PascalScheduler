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

#ifndef SYS_DEV_H
#define SYS_DEV_H

#include <stdint.h>
#include <types.h>

#define driver_map(id, table) devices[id] = table

/**
 */
enum uio_rw   {
  UIO_READ, /**< Read operation */
  UIO_WRITE /**< Write operation */
};

/**
 */
enum seek_whence   {
  SEEK_SET, /**< The offset will be from the beginning */
  SEEK_CUR, /**< The offset will be from the current offset */
  SEEK_END /**< The offset will be from the end*/
};

/**
 */
enum dev_type {
  I2C, /**< I2C Device */
  SPI, /**< SPI Device */
  UART, /**< UART Device */
  PWM, /**< PWM Device */
  LCD, /**< LCD Device */
  MMC, /**< MMC Device */
  DEV_T_RTC /**< RTC Device*/
};

/**
 */
enum dev_id {
  DEV_I2C, /**< I2C Device */
  DEV_SPI, /**< SPI Device */
  DEV_UART, /**< UART Device */
  DEV_PWM, /**< PWM Device */
  DEV_LCD, /**< LCD Device */
  DEV_MMC, /**< MMC Device */
  DEV_RTC, /**< RTC Device*/

  DRIVER_NB
};

/** Vnodes are used by open/close/read/write/ioctl operations.
 */
struct vnode {
    enum	dev_type dev_type;/**< What kind of driver is taking care of this vnode */
    int		dev_id;/**< Which driver is taking care of this vnode */
    int		endpt;/**< Use for example by I2C or SPI to store peer address */
    int		offset;/**< Used if device is seekable */
    void* data;/**< Some device data pointer */
};

/**
 */
struct iovec {
    void	*iov_base; /**< Pointer to a buffer */
    int		iov_len;
};

/** Used by d_read and d_write.
 */
struct uio {
    struct  iovec *uio_iov; /**< Pointer to a buffer */
    int     uio_offset;
    int     uio_resid; /**< Number of element to read/write */
    enum    uio_rw uio_rw; /**< Is this buffer for read/write */
};

/** Switch structure for a device driver.
 */
struct dev_ops {
    int	(*d_open) (struct vnode *vnode);
    int	(*d_close)(struct vnode *vnode);
    int	(*d_read) (struct vnode *vnode, struct uio *);
    int	(*d_write)(struct vnode *vnode, struct uio *);
    int	(*d_ioctl)(struct vnode *vnode, uint8_t code, void *data);
};

int dev_open(struct vnode *vnode);
int dev_ioctl(struct vnode *vnode, uint8_t code, void *data);
int dev_write(const void * ptr, size_t count, struct vnode *vnode);
int dev_read(void * ptr, size_t count, struct vnode *vnode);
int dev_seek(int offset, enum seek_whence whence, struct vnode *vnode);

#endif
