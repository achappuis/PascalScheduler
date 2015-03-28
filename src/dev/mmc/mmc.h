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

#ifndef DEV_MMC_H
#define DEV_MMC_H

#include <stdint.h>

#include "sys/dev.h"
#include "../spi/spi.h"

#define TIMEOUT 128

#define MMC_ERROR  -1
#define MMC_OK      2

enum mmc_voltage {
  MMC_VOLTAGE_UNDEF = (0x0 << 8),
  MMC_VOLTAGE_27_36 = (0x1 << 8),
  MMC_VOLTAGE_LOW   = (0x2 << 8)
};

enum mmc_ocr_voltage {
  MMC_OCR_VOLTAGE_27_28 = (1 << 15),
  MMC_OCR_VOLTAGE_28_29 = (1 << 16),
  MMC_OCR_VOLTAGE_29_30 = (1 << 17),
  MMC_OCR_VOLTAGE_30_31 = (1 << 18),
  MMC_OCR_VOLTAGE_31_32 = (1 << 19),
  MMC_OCR_VOLTAGE_32_33 = (1 << 20),
  MMC_OCR_VOLTAGE_33_34 = (1 << 21),
  MMC_OCR_VOLTAGE_34_35 = (1 << 22),
  MMC_OCR_VOLTAGE_35_36 = (1 << 23),
  MMC_OCR_HCS           = (1 << 30),
  MMC_OCR_CCS           = (1 << 30),
  MMC_OCR_BUSY          = (1 << 31)
};

#define MMC_CMD(_a) (0x40 | _a)

enum mmc_cmd {
  MMC_CMD01 = MMC_CMD(1),
  MMC_CMD08 = MMC_CMD(8),
  MMC_CMD09 = MMC_CMD(9),
  MMC_CMD10 = MMC_CMD(10),
  MMC_CMD17 = MMC_CMD(17),
  MMC_CMD41 = MMC_CMD(41),
  MMC_CMD58 = MMC_CMD(58)
};

enum mmc_status {
  STATUS_IDLE         = 1,
  STATUS_ERASE_RESET  = 2,
  STATUS_ILLEGAL_CMD  = 4,
  STATUS_CRC_ERROR    = 8,
  STATUS_SEQ_ERROR    = 16,
  STATUS_ADDR_ERROR   = 32,
  STATUS_PARAM_ERROR  = 64
};

enum mmc_flag {
  MMC_DISABLED = 1,
  MMC_V1       = 2,
  MMC_V2       = 4,
  MMC_SDHC     = 8
};

#define MAX_BLOCK_LENGTH 512

struct mmc_cid {
  uint32_t psn;
  char pnm[6];
  uint16_t mdt_year;
  char oid[3];
  uint8_t mdt_month;
  uint8_t crc7;
  uint8_t prv;
  uint8_t mid;
};

struct mmc_csd {
  uint32_t c_size;
  uint32_t c_mult;
  uint32_t read_bl_len;
  uint32_t write_bl_len;
};

struct mmc_info {
  struct mmc_csd csd;
  struct mmc_cid cid;
  uint32_t spi_channel;
  uint32_t mem_size;
  char flags;
};

#endif
