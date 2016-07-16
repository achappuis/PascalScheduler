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

#include "rtc.h"
#include "ds1307.h"
#include "../i2c/i2c.h"

int _BCD_decimal(int v)
{
    return (v & 0x0F) + 10 * ((v & 0x70) >> 4);
}

int _decimal_BCD(int v)
{
    return ((v % 10) & 0x0F) | (((v / 10) & 0x0F ) << 4);
}

int
ds1307_rtc_open(struct vnode UNUSED *vnode)
{

  return 0;
}

int
ds1307_rtc_close(struct vnode UNUSED *vnode)
{
    return 0;
}

int
ds1307_rtc_write(struct vnode UNUSED *vnode, struct uio UNUSED *uio)
{
    return 0;
}

int
ds1307_rtc_read(struct vnode UNUSED *vnode, struct uio UNUSED *uio)
{
    return 0;
}

int
ds1307_rtc_ioctl(struct vnode UNUSED *vnode, uint8_t code, void *data)
{
    int rtc_addr = 0x68 << 1;
    char buf[8];
    struct tm * time_data = (struct tm *)data;

    extern struct vnode i2c_node;
    dev_ioctl(&i2c_node, I2C_IOCTL_SET_PEER, &rtc_addr);

    switch (code) {
      case RTC_IOCTL_SET_TIME:
        buf[0] = 0x00;
        buf[DS1307_ADDRESS_SECONDS + 1] = _decimal_BCD(time_data->tm_sec);
        buf[DS1307_ADDRESS_MINUTES + 1] = _decimal_BCD(time_data->tm_min);
        buf[DS1307_ADDRESS_HOURS   + 1] = _decimal_BCD(time_data->tm_hour);
        buf[DS1307_ADDRESS_DAY     + 1] = _decimal_BCD(time_data->tm_mday);
        buf[DS1307_ADDRESS_MONTH   + 1] = _decimal_BCD(time_data->tm_mon);
        buf[DS1307_ADDRESS_YEAR    + 1] = _decimal_BCD(time_data->tm_year);
        dev_write(buf, 8, &i2c_node);
        break;
      case RTC_IOCTL_GET_TIME:
        buf[0] = 0x00;
        dev_write(buf, 1, &i2c_node);
        dev_read(buf, 7, &i2c_node);

        time_data->tm_sec  = _BCD_decimal(buf[DS1307_ADDRESS_SECONDS]);
        time_data->tm_min  = _BCD_decimal(buf[DS1307_ADDRESS_MINUTES]);
        time_data->tm_hour = _BCD_decimal(buf[DS1307_ADDRESS_HOURS]);
        time_data->tm_mday = _BCD_decimal(buf[DS1307_ADDRESS_DAY]);
        time_data->tm_mon  = _BCD_decimal(buf[DS1307_ADDRESS_MONTH]);
        time_data->tm_year = _BCD_decimal(buf[DS1307_ADDRESS_YEAR]);
        break;
    }

    return 0;
}

struct dev_ops rtc = { ds1307_rtc_open, ds1307_rtc_close, ds1307_rtc_read, ds1307_rtc_write, ds1307_rtc_ioctl };
