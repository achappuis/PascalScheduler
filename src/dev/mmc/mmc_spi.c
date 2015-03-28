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

static void
mmc_send(uint8_t *buf, uint8_t size) {
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

    mmc_send_cmd(MMC_CMD17, sector * mmc_info->csd.read_bl_len);
    if (mmc_spi_read_status() != 0)
        return MMC_ERROR;

    if (mmc_spi_wait_start_block(0) == MMC_ERROR)
        return MMC_ERROR;

    mmc_read(buf, mmc_info->csd.read_bl_len); // sector
    mmc_read(crc, 2);   // CRC

    return MMC_OK;
}

#define mmc_read_r1(_buf) mmc_read((_buf), 1);
#define mmc_read_r2(_buf) mmc_read((_buf), 2);
#define mmc_read_r3(_buf) mmc_read((_buf), 5);
#define mmc_read_r7(_buf) mmc_read((_buf), 5);

static int
mmc_read_csd(struct mmc_info *mmc_info)
{
    struct mmc_csd *mmc_csd = &(mmc_info->csd);
    uint8_t buf[18]; //16 byte + 2 CRC byte

    mmc_info->mem_size = 0;

    mmc_send_cmd(MMC_CMD09, 0);
    mmc_spi_wait_start_block(1);
    mmc_read(buf, 18);
    if (mmc_crc7(buf, 15) != buf[15]) return MMC_ERROR;

    mmc_csd->c_size=0;mmc_csd->c_mult=0;mmc_csd->read_bl_len=0;
    // For Some reason some cards seems no to comply any of this CSD.
    if ((buf[0] & 0xC0) == 0x00) { // CSD Version 1
        mmc_csd->read_bl_len = (buf[5] & 0x0F);
        mmc_csd->c_size      = (buf[5] & 0x0F);
        mmc_csd->c_mult      = ((buf[10] & 0x03) << 1) | ((buf[11] & 0x8) >> 7);
        mmc_info->mem_size = 0;
    } else if ((buf[0] & 0xC0) == 0x40) { // CSD Version 2
        mmc_csd->c_size = (uint32_t)buf[9] | ((uint32_t)buf[8] << 8) | (((uint32_t)buf[7] & 0x1F) << 16);
        mmc_info->mem_size = (mmc_csd->c_size+1 * 512);// Size in Mib
    }

    mmc_info->csd.read_bl_len = 512;
    mmc_info->csd.write_bl_len = 512;

    return MMC_OK;
}

static int
mmc_read_cid(struct mmc_info *mmc_info)
{
    struct mmc_cid *mmc_cid = &(mmc_info->cid);
    uint8_t buf[18]; //16 byte + 2 CRC byte

    mmc_send_cmd(MMC_CMD10, 0);
    mmc_spi_wait_start_block(1);
    mmc_read(buf, 18);
    if (mmc_crc7(buf, 15) != buf[15]) return MMC_ERROR;

    mmc_cid->mid = buf[0];
    strncpy(mmc_cid->oid, (char *)&buf[1], 2);mmc_cid->oid[2]= '\0';
    strncpy(mmc_cid->pnm, (char *)&buf[3], 5);mmc_cid->pnm[5]= '\0';
    mmc_cid->prv = buf[8];
    mmc_cid->psn = buf[9] << 24 |  buf[10] << 16 |  buf[11] << 8 |  buf[12];
    mmc_cid->mdt_year = ((buf[14] & 0xF0) > 4) + (buf[13] & 0x0F) + 2000;
    mmc_cid->mdt_month = (buf[14] & 0x0F);
    mmc_cid->crc7 = buf[15];

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
        mmc_info->flags |= MMC_SDHC;
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
    if (mmc_info == NULL)
        return -1;

    return 0;
}

int
mmc_spi_read(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(uio);

    struct mmc_info *mmc_info = (struct mmc_info *)vnode->data;
    int i;
    uint8_t *ptr;
    uint8_t buf[MAX_BLOCK_LENGTH];

    if (mmc_info == NULL)
        return -1;
    if (uio->uio_rw != UIO_READ)
        return -1;

    uint32_t read_sector = vnode->offset / mmc_info->csd.read_bl_len;
    uint32_t read_offset = vnode->offset % mmc_info->csd.read_bl_len;
    i = read_offset;

    if ((uio->uio_resid+read_offset) > mmc_info->csd.read_bl_len)
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
