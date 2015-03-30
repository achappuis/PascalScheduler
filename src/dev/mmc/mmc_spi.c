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

#include "mmc.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define __UNUSED(x) (void)(x)

extern struct vnode spi_node;

static uint8_t
mmc_crc7(uint8_t *data,uint8_t length)
{
    uint8_t i, ibit, c, crc;

    crc = 0x00;
    for (i = 0; i < length; i++, data++) {
        c = *data;

        for (ibit = 0; ibit < 8; ibit++) {
            crc = crc << 1;
            if ((c ^ crc) & 0x80) crc = crc ^ 0x09;
            c = c << 1;
        }
        crc = crc & 0x7F;
    }

    return (crc << 1) | 0x1;
}

void
swap_words(uint8_t *data)
{
    uint8_t tmp;

    tmp = data[3];
    data[3] = data[0];
    data[0] = tmp;

    tmp = data[2];
    data[2] = data[1];
    data[1] = tmp;
}

static void
mmc_send(uint8_t *buf, int size) {
    dev_write(buf, size, &spi_node);
}

static void
mmc_send_cmd(enum mmc_cmd cmd, uint32_t arg)
{
    uint8_t array[7];
    array[0] = 0xFF;
    array[1] = cmd;
    array[2] = (arg & (0xFF << 3*8)) >> 3*8;
    array[3] = (arg & (0xFF << 2*8)) >> 2*8;
    array[4] = (arg & (0xFF << 1*8)) >> 1*8;
    array[5] = (arg & 0xFF);
    array[6] = mmc_crc7(array+1, 5);
    mmc_send(array, 7);
}


static void
mmc_send_acmd(enum mmc_cmd cmd, uint32_t arg)
{
    uint8_t array[7];
    array[0] = 0xFF;
    array[1] = 0x77;
    array[2] = 0;
    array[3] = 0;
    array[4] = 0;
    array[5] = 0;
    array[6] = 0x01;
    mmc_send(array, 7);
    array[0] = 0xFF;
    array[1] = 0xFF;
    array[2] = 0xFF;
    array[3] = 0xFF;
    mmc_send(array, 4);
    array[0] = cmd;
    array[1] = (arg & (0xFF << 3*8)) >> 3*8;
    array[2] = (arg & (0xFF << 2*8)) >> 2*8;
    array[3] = (arg & (0xFF << 1*8)) >> 1*8;
    array[4] = (arg & 0xFF);
    array[5] = mmc_crc7(array, 5);
    mmc_send(array, 6);
}

static uint8_t
mmc_read_byte() {
    uint8_t buf;
    dev_read(&buf, 1, &spi_node);
    return buf;
}

static uint8_t
mmc_spi_wait_unbusy() {
    uint8_t status;
    do {
        status = mmc_read_byte();
    } while(status == 0xFF);
    return status;
}

static uint8_t
mmc_spi_read_status() {
    uint8_t status;
    do {
        status = mmc_read_byte();
    } while(status == 0xFF);
    return status;
}

static int
mmc_spi_wait_start_block(uint8_t ignore_status) {
    uint8_t status;
    do {
        status = mmc_read_byte();
        if ((status & 0xF0) == 0 && !ignore_status)
            return MMC_ERROR;
    } while(status != 0xFE);
    return MMC_OK;
}

static void
mmc_read(uint8_t *buf, int size) {
  buf[0] = mmc_spi_wait_unbusy();
  dev_read(buf+1, size-1, &spi_node);
  return;
}

static int
mmc_read_sector(struct mmc_info *mmc_info, uint8_t *buf, int sector) {
    uint8_t crc[2];

    mmc_send_cmd(MMC_CMD17, sector * mmc_info->read_block_size);
    if (mmc_spi_read_status() != 0)
        return MMC_ERROR;

    if (mmc_spi_wait_start_block(0) == MMC_ERROR)
        return MMC_ERROR;

    mmc_read(buf, mmc_info->read_block_size); // sector
    // mmc_read(crc, 2);   // CRC

    return MMC_OK;
}

static int
mmc_write_sector(struct mmc_info *mmc_info, uint8_t *buf, int sector) {
    uint8_t head[2];
    head[0] = 0xFF;
    head[1] = 0xFE;

    mmc_send_cmd(MMC_CMD24, sector * mmc_info->write_block_size);
    if (mmc_spi_read_status() != 0)
        return MMC_ERROR;

    mmc_send(head, 2);
    mmc_send(buf, mmc_info->write_block_size);

    return MMC_OK;
}

#define mmc_read_r1(_buf) mmc_read((_buf), 1);
#define mmc_read_r2(_buf) mmc_read((_buf), 2);
#define mmc_read_r3(_buf) mmc_read((_buf), 5);
#define mmc_read_r7(_buf) mmc_read((_buf), 5);

static int
sd_decode_csd(uint32_t *buf, struct mmc_info *mmc_info)
{
    struct mmc_csd *mmc_csd = &(mmc_info->csd);

    mmc_csd->c_structure = UNSTUFF_BITS(buf,126,2);
    mmc_csd->read_bl_len = UNSTUFF_BITS(buf,80,4);
    mmc_csd->write_bl_len = UNSTUFF_BITS(buf,22,4);
    mmc_info->read_block_size = 1 << mmc_csd->read_bl_len;
    mmc_info->write_block_size = 1 << mmc_csd->write_bl_len;

    if (mmc_csd->c_structure == 0) { // CSD Version 1
        mmc_csd->c_size = UNSTUFF_BITS(buf,62,12);
        mmc_csd->c_mult = UNSTUFF_BITS(buf,47,3);
        int mult     = 1 << (mmc_csd->c_mult + 2);
        int blocknr  = (mmc_csd->c_size + 1) * mult;
        mmc_info->mem_size = blocknr * mmc_info->read_block_size / 1024 / 1024;
    } else if (mmc_csd->c_structure == 1) { // CSD Version 2
        mmc_csd->c_size = UNSTUFF_BITS(buf,48,22);
        mmc_info->mem_size = (mmc_csd->c_size+1 * 512);
    } else {
        return MMC_ERROR;
    }

    return MMC_OK;
}

static int
mmc_decode_csd(uint32_t *buf, struct mmc_info *mmc_info)
{
    struct mmc_csd *mmc_csd = &(mmc_info->csd);

    mmc_csd->c_structure = UNSTUFF_BITS(buf,126,2);
    mmc_csd->read_bl_len = UNSTUFF_BITS(buf,80,4);
    mmc_csd->write_bl_len = UNSTUFF_BITS(buf,22,4);
    mmc_info->read_block_size = 1 << mmc_csd->read_bl_len;
    mmc_info->write_block_size = 1 << mmc_csd->write_bl_len;

// CSD Version 1.0
// CSD Version 1.1
// CSD Version 1.2
    mmc_csd->c_size = UNSTUFF_BITS(buf,62,12);
    mmc_csd->c_mult = UNSTUFF_BITS(buf,47,3);
    int mult     = 1 << (mmc_csd->c_mult + 2);
    int blocknr  = (mmc_csd->c_size + 1) * mult;
    mmc_info->mem_size = blocknr * mmc_info->read_block_size / 1024 / 1024;

    return MMC_OK;
}

static int
mmc_read_csd(struct mmc_info *mmc_info)
{
    uint32_t buf[5]; //16 byte + 2 CRC byte
    uint32_t i;

    mmc_send_cmd(MMC_CMD09, 0);
    mmc_spi_wait_start_block(1);
    mmc_read((uint8_t*)buf, 18);
    if (mmc_crc7((uint8_t*)buf, 15) != ((uint8_t*)buf)[15]) return MMC_ERROR;

    for (i = 0;i < 4;i++)
        swap_words((uint8_t*)&buf[i]);


    if (UNSTUFF_BITS(buf,122,4) == 0) {
        printf("MMC : MMC 1.0 or SD, opting for the SD option\n");// there is probably a better option!
        return sd_decode_csd(buf, mmc_info);
    } else {
        return mmc_decode_csd(buf, mmc_info);
    }

    return MMC_ERROR;
}

static int
mmc_read_cid(struct mmc_info *mmc_info)
{
    struct mmc_cid *mmc_cid = &(mmc_info->cid);
    uint32_t buf[5]; //16 byte + 2 CRC byte
    uint32_t i;

    mmc_send_cmd(MMC_CMD10, 0);
    mmc_spi_wait_start_block(1);
    mmc_read((uint8_t*)buf, 18);
    if (mmc_crc7((uint8_t*)buf, 15) != ((uint8_t*)buf)[15]) return MMC_ERROR;

    strncpy(mmc_cid->oid, (char *)&((uint8_t*)buf)[1], 2);mmc_cid->oid[2]= '\0';
    strncpy(mmc_cid->pnm, (char *)&((uint8_t*)buf)[3], 5);mmc_cid->pnm[5]= '\0';

    for (i = 0;i < 4;i++)
        swap_words((uint8_t*)&buf[i]);

    mmc_cid->mid = UNSTUFF_BITS(buf,120,8);
    mmc_cid->prv = UNSTUFF_BITS(buf,56,8);
    mmc_cid->psn = UNSTUFF_BITS(buf,24,32);;
    mmc_cid->mdt_year = UNSTUFF_BITS(buf,12,8); + 2000;
    mmc_cid->mdt_month = UNSTUFF_BITS(buf,8,4);
    mmc_cid->crc7 = UNSTUFF_BITS(buf,1,7);

    return MMC_OK;
}

static int
mmc_get_info(struct mmc_info *mmc_info)
{
    struct mmc_cid *mmc_cid = &(mmc_info->cid);
    uint8_t buf[18]; //16 byte + 2 CRC byte

    /*
    * Read OCR
    */
    mmc_send_cmd(MMC_CMD58, 0);
    mmc_read_r3(buf);
    if (buf[0] != 0) return MMC_ERROR;

    uint32_t mmc_ocr = buf[4] | buf[3] << 8 | buf[2] << 16 | buf[1] << 24;
    if(((mmc_ocr >> 31) & 0x01) == 0) {
        printf("MMC : Unexpected OCR value. Power up did not complete.\n");
        return MMC_ERROR;
    }
    if(((mmc_ocr >> 30) & 0x01) != 0) {
        printf("MMC : Memory Card is a SDHC!\n");
        mmc_info->flags |= SD_SDHC;
    }

    /*
    * Read CSD/CID
    */
    if (mmc_read_csd(mmc_info) == MMC_ERROR) {
      return MMC_ERROR;
    }
    if (mmc_read_cid(mmc_info) == MMC_ERROR) {
      return MMC_ERROR;
    }

    /*
    * Print infos
    */
    printf("MMC : New Multimedia Card %s - %s\n", mmc_cid->oid, mmc_cid->pnm);
    printf("MMC : Size is %d MiB\n",  mmc_info->mem_size);

    return MMC_OK;
}

static int
mmc_init(struct mmc_info *mmc_info)
{
    uint8_t array[7];
    uint8_t buf[17];

    array[0] = 0xFF;
    array[1] = 0xFF;
    mmc_send(array, 2);

    mmc_send_cmd(MMC_CMD08, MMC_VOLTAGE_27_36 | 0xAA);
    mmc_read_r7(buf);
    if ((buf[0] & STATUS_ILLEGAL_CMD) != 0) {
        printf("MMC : Memory Card Version is 1!\n");
        mmc_info->flags |= MMC_V1;
        do {
            mmc_send_cmd(MMC_CMD01, 0);
            mmc_read_r1(buf);
        } while(buf[0] != 0);
    } else {
        printf("MMC : Memory Card Version is 2!\n");
        mmc_info->flags |= MMC_V2;
        do {
            mmc_send_acmd(MMC_CMD41, MMC_OCR_VOLTAGE_30_31 | MMC_OCR_HCS);
            mmc_read_r1(buf);
        } while(buf[0] != 0);
    }

    return mmc_get_info(mmc_info);
}

static int
mmc_to_idle()
{
    int i = 0;
    uint8_t array[6];
    array[0] = 0x40;
    array[1] = 0;
    array[2] = 0;
    array[3] = 0;
    array[4] = 0;
    array[5] = 0x95;
    mmc_send(array, 6);

    while(mmc_read_byte() != 0x01) {
        i++;
        if (i > TIMEOUT)
            return MMC_ERROR;
    }

    return MMC_OK;
}

int
mmc_spi_open(struct vnode *vnode)
{
    struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
    if (mmc_info == NULL)
        goto MMC_SPI_CRITICAL;

    mmc_info->flags = 0;

    int i = 0;
    uint8_t array[6];
    array[0] = 0xFF;

    dev_ioctl(&spi_node, SPI_IOCTL_DISABLE_SLAVE, NULL);

    for (i = 0; i < 10; i++) {
        mmc_send(array, 1);
    }

    dev_ioctl(&spi_node, SPI_IOCTL_ENABLE_SLAVE, &(mmc_info->spi_channel));

    if (mmc_to_idle() == MMC_ERROR) {
        printf("MMC : Error\n");
        goto MMC_SPI_ERROR;
    }

    if (mmc_init(mmc_info) == MMC_ERROR) {
        printf("MMC : Init Error\n");
        goto MMC_SPI_ERROR;
    }

    return 0;

MMC_SPI_ERROR:
    mmc_info->flags = MMC_DISABLED;
MMC_SPI_CRITICAL:
    mmc_to_idle();
    return -1;
}

int
mmc_spi_close(struct vnode *vnode)
{
    struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
    if (mmc_info == NULL)
        return -1;

    mmc_to_idle();
    return 0;
}

int
mmc_spi_write(struct vnode *vnode, struct uio *uio)
{
  __UNUSED(uio);

  struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
  int i;
  uint8_t *ptr;
  uint8_t buf[MAX_BLOCK_LENGTH];

  if (mmc_info == NULL)
      return -1;
  if (uio->uio_rw != UIO_WRITE)
      return -1;

  uint32_t read_sector = vnode->offset / mmc_info->read_block_size;
  uint32_t read_offset = vnode->offset % mmc_info->read_block_size;
  i = read_offset;

  if ((uio->uio_resid+read_offset) > mmc_info->read_block_size)
      return -1;

  if (mmc_read_sector(mmc_info, buf, read_sector) != MMC_OK)
      return -1;

  ptr = uio->uio_iov->iov_base;
  for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
      buf[i++] = *ptr;
  }

  if (mmc_write_sector(mmc_info, buf, read_sector) != MMC_OK)
      return -1;

  return 0;
}

int
mmc_spi_read(struct vnode *vnode, struct uio *uio)
{
    struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
    int i;
    uint8_t *ptr;
    uint8_t buf[MAX_BLOCK_LENGTH];

    if (mmc_info == NULL)
        return -1;
    if (uio->uio_rw != UIO_READ)
        return -1;

    uint32_t read_sector = vnode->offset / mmc_info->read_block_size;
    uint32_t read_offset = vnode->offset % mmc_info->read_block_size;
    i = read_offset;

    if ((uio->uio_resid+read_offset) > mmc_info->read_block_size)
        return -1;

    if (mmc_read_sector(mmc_info, buf, read_sector) != MMC_OK)
        return -1;

    ptr = uio->uio_iov->iov_base;
    for(; uio->uio_resid > 0; uio->uio_resid--, ptr++) {
        *ptr = buf[i++];
    }

    return 0;
}

int
mmc_spi_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
    __UNUSED(code);
    __UNUSED(data);

    struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
    if (mmc_info == NULL)
        return -1;


    return 0;
}

struct dev_ops mmc = { mmc_spi_open, mmc_spi_close, mmc_spi_read, mmc_spi_write, mmc_spi_ioctl };
