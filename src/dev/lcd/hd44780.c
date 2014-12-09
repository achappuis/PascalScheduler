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

#include "lcd.h"
#include "kern.h"
#include "syscall.h"
#include "sys/dev.h"

#include "../i2c/i2c.h"

#pragma message "LCD-HD44780 module never tested!"

#define HD44780_4_BITS_OPS	0x2<<4

#define HD44780_CLR_DISPLAY	0x1
#define HD44780_RETURN_HOME	0x2

struct vnode node;
    
void
hd44780_check_busy()
{
    //Longest operation is 1.6ms, we don't want to wire R/W so we just wait ...
    sys_sleep(2);
}
void
hd44780_write_bus(char data, char rs, char e)
{
    char code = (e & 0x1) << 4;
    code |= (rs & 0x1) << 5;
    code |= (data & 0xF);
   
#ifdef LCD_I2C_BUS
    dev_write(&code, 0, 1, &node);
#endif
}

void
hd44780_send_4(char data, char rs)
{
    if (rs != 0 && (data < ' ' || data > 'z'))
	return;

    hd44780_check_busy();// Check busy    
    hd44780_write_bus((data & 0xF0) >> 4, rs, 1);// RS and E asserted and MSB
    hd44780_write_bus((data & 0xF0) >> 4, rs, 0);// E not asserted    
    hd44780_write_bus(data & 0x0F, rs, 1);       // E asserted and LSB
    hd44780_write_bus(data & 0x0F, 0,  0);      // E not asserted
}

void
hd44780_send_8(char data, char rs)
{
    hd44780_check_busy();// Check busy
    
    hd44780_write_bus(data, rs, 1);// RS and E asserted and MSB
    hd44780_write_bus(data, rs, 0);// E not asserted
}

int
hd44780_open(struct vnode *vnode)
{
    __UNUSED(vnode);
   
#ifdef LCD_I2C_BUS
    node.dev_type	= I2C;
    node.dev_id		= DEV_I2C;
    int add = LCD_ADDRESS;
    dev_ioctl(&node, I2C_IOCTL_SET_PEER, &add);
    dev_open(&node); 
#endif
    
    hd44780_send_8(HD44780_4_BITS_OPS, 0); // Set 4 bits operations
    hd44780_send_4(0x28, 0); // Set display layout (8*2) and small font DL = 4bits
    hd44780_send_4(0x0C, 0); // Turn on display
    hd44780_send_4(0x06, 0); // Set Inc mode
    
    hd44780_send_4('H', 1);// Write H
    hd44780_send_4('e', 1);// Write e
    hd44780_send_4('l', 1);// Write l
    hd44780_send_4('l', 1);// Write l
    hd44780_send_4('o', 1);// Write o
    
    
    return 0;
}

int
hd44780_close(struct vnode *vnode)
{
    __UNUSED(vnode);
    return 0;
}

int
hd44780_write(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(vnode);
    __UNUSED(uio);
    
    return 0;
}

int
hd44780_read(struct vnode *vnode, struct uio *uio)
{
    __UNUSED(vnode);
    __UNUSED(uio);

    return 0;
}

int
hd44780_ioctl(struct vnode *vnode, uint8_t code, void *data)
{
    __UNUSED(vnode);
    __UNUSED(code);
    __UNUSED(data);
        
    return 0;
} 

struct dev_ops lcd = { hd44780_open, hd44780_close, hd44780_read, hd44780_write, hd44780_ioctl };
