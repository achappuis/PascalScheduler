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

#include "sys/dev.h"

extern struct dev_ops* devices[DRIVER_NB];

#define __UNUSED(x) (void)(x)
#define __UNCONST(a)    ((void *)(unsigned long)(const void *)(a))

/*
  Function: dev_open
  _open is used to initialize a driver.

  Parameters:
    vnode - _

  Returns:
    0 if successful, an error code otherwise.

  See Also:
    - <close>
    - <ioctl>
    - <fread>
    - <fwrite>
*/
int
dev_open(struct vnode *vnode)
{
      return devices[vnode->dev_id]->d_open(vnode);
}

/*
  Function: dev_close
  _close is used to de-initialize a driver.

  Parameters:
    vnode - _

  Returns:
    0 if successful, an error code otherwise.

  See Also:
    - <open>
    - <ioctl>
    - <fread>
    - <fwrite>
*/
int
dev_close(struct vnode *vnode)
{
      return devices[vnode->dev_id]->d_close(vnode);
}

/*
  Function: dev_ioctl
  ioctl function.

  Parameters:
    vnode - _
    code  - _
    data  - _

  Returns:
    0 if successful, an error code otherwise.

  See Also:
    - <open>
    - <close>
    - <fread>
    - <fwrite>
*/
int
dev_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
      return devices[vnode->dev_id]->d_ioctl(vnode, code, data);
}

/*
  Function: dev_write

  Parameters:
    ptr    - A buffer to write data from.
    count  - Number of elements in the buffer
    vnode  - Node to write to.

  Returns:
    Number of elements successfully written.

  See Also:
    - <open>
    - <close>
    - <ioctl>
    - <fread>
*/
int
dev_write(const void * ptr, size_t count, struct vnode *vnode )
{
  struct iovec iov;
  struct uio uio;

  iov.iov_base = __UNCONST(ptr);
  iov.iov_len = count;

  uio.uio_resid = count;
  uio.uio_iov = &iov;
  uio.uio_rw = UIO_WRITE;

  devices[vnode->dev_id]->d_write(vnode, &uio);

  return (count - uio.uio_resid);
}

/*
  Function: dev_read

  Parameters:
    ptr    - A buffer to write data to.
    count  - Number of elements to read.
    vnode  - Node to read from.

  Returns:
    Number of elements successfully read.

  See Also:
    - <open>
    - <close>
    - <ioctl>
    - <fwrite>
*/
int
dev_read(void * ptr, size_t count, struct vnode *vnode)
{
  struct iovec iov;
  struct uio uio;

  iov.iov_base = __UNCONST(ptr);
  iov.iov_len = 0;

  uio.uio_resid = count;
  uio.uio_iov = &iov;
  uio.uio_rw = UIO_READ;

  while (uio.uio_resid > 0) {
      iov.iov_base = __UNCONST(ptr) + (count - uio.uio_resid);
      devices[vnode->dev_id]->d_read(vnode, &uio);
  }
  return (count - uio.uio_resid);
}

/*
  Function: dev_seek

  For now, it is not possible to seek from the end.
  No proper test is made to check if the offset is seekable, an issues will
  probably be raised during read/write operation. In the futur, adding a seek
  callback may be a solution to prevent from this.

  Parameters:
    offset - Offset
    whence - <seek_whence>
    vnode  - Node to read from.

  Returns:
    On success, the new offset from the beginning of the file.
    On error -1.

  See Also:
    - <open>
    - <close>
    - <ioctl>
    - <fwrite>
*/
int
dev_seek(int offset, enum seek_whence whence, struct vnode *vnode)
{
    switch(whence) {
    case SEEK_SET:
        if (offset < 0)
            return -1;
        vnode->offset = offset;
        break;
    case SEEK_CUR:
        if (offset < 0 && (-offset > vnode->offset))
            return -1;
        vnode->offset += offset;
        break;
    default:
        return -1;
    }

    return vnode->offset;
}
