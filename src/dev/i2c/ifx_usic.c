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
#include "xmc1100.h"

#define __UNUSED(x) (void)(x)

enum usic_tdf {
    I2C_WRITE         = 0x000,
    I2C_READ          = 0x200,
    I2C_READ_NACK     = 0x300,
    I2C_START         = 0x400,
    I2C_RESTART       = 0x500,
    I2C_STOP          = 0x600,
};

void
_usic_i2c_send(uint32_t uwTDF, uint32_t uwData)
{
    while( TCSR & 0x0080);
    
    PSCR |= 0x2000;
    // clear PSR_TBIF
    TBUF[0] = uwTDF | uwData; // load TBUF00
} 

uint32_t
_usic_i2c_read()
{
    while( PSR & 0x800);
    
    return (uint32_t)RBUF;
}

int
usic_i2c_open(struct vnode *vnode)
{
    __UNUSED(vnode);
    
    KSCFG = 0x3; // Enable module
    CCR = 0x000000000;
          
    DX0CR = 0x00000000; // Input Stage SDA -> P0.14
    DX1CR = 0x00000001; // Intput Stage SDC -> P0.8

    FDR = 0x00008317; // DM=2, STEP=0x317
    BRG = 0x00182500; // PCTQ=1, DCTQ=9, PDIV = 24 
    
    INPR = 0x00000000; // AINP=0, RINP=0, TBINP=0, TSINP=0 PINP=0
    SCTR = 0x073F0303; // TRM=3, PDL=1, SDIR=1, WLE=7, FLE=3F
    TCSR = 0x00000500; // TDEN=1, TDSSM=1
    PCR  = 0x30000000; // STIM=0, HDEL=12
    PCR  = 0x0001FFFF; // STIM=0, HDEL=12

    // Pin as opendrain. ALT6
    GPIO_PIN_MUX(SCL_PORT, GPIO_OUTPUT_OD | SCL_ALT , SCL_PIN); 
    GPIO_PIN_MUX(SDA_PORT, GPIO_OUTPUT_OD | SDA_ALT , SDA_PIN);  

    //CCR.MODE = 0x0100;
    CCR = 0x000000004;
    
    return 0;
}

int
usic_i2c_close(struct vnode *vnode)
{
    __UNUSED(vnode);
    return 0;
}

int
usic_i2c_write(struct vnode *vnode, struct uio *uio)
{
    uint32_t *ptr;
    int low_add;

#define	SEND_BYTE(a)	  _usic_i2c_send(0, ((*(ptr)) & (0xFF << 8*(a))) >> 8*(a) ); uio->uio_resid--
    
    if (uio->uio_rw != UIO_WRITE) {
	return -1;
    }
    
    _usic_i2c_send(I2C_START, vnode->endpt);
    
    // iov_base should be aligned
    ptr = uio->uio_iov->iov_base;
    low_add = (int)((uint32_t)ptr & 0x3); // lowest 2 bits
    ptr = (uint32_t*)((uint32_t)ptr & (~0x3));
    switch(low_add) {
    case 0:		// Aligned
	break;
    case 1:
	SEND_BYTE(1);
	if(uio->uio_resid > 0) { SEND_BYTE(2); }
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
	break;
    case 2:
	SEND_BYTE(2);
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
	break;
    case 3:
	SEND_BYTE(3);
	break;
    }
    if (uio->uio_resid == 0) {
	_usic_i2c_send(I2C_STOP, 0);
	return 0;
    }
    
    for(; uio->uio_resid > 0; ptr++) {	
	SEND_BYTE(0);
	if(uio->uio_resid > 0) { SEND_BYTE(1); }
	if(uio->uio_resid > 0) { SEND_BYTE(2); }
	if(uio->uio_resid > 0) { SEND_BYTE(3); }
    }
    
    _usic_i2c_send(I2C_STOP, 0);
    return 0;
}

int
usic_i2c_read(struct vnode *vnode, struct uio *uio)
{
    uint32_t *ptr;
    int low_add;
    int offset;
    uint32_t ans;

#define	CLEAR_ANS(__a) *ptr &= ~(0xFF << 8*(__a))
#define	WRITE_ANS(__a) *ptr |= ans << 8*(__a);offset++;offset%=4; uio->uio_resid--
#define READ_INNER(__a) 	if (uio->uio_resid == 1) {\
	    _usic_i2c_send(I2C_READ_NACK, 0);\
	} else {\
	    _usic_i2c_send(I2C_READ, 0);\
	}\
	ans = _usic_i2c_read();\
	CLEAR_ANS(__a);\
	WRITE_ANS(__a);
    
    if (uio->uio_rw != UIO_READ) {
	return -1;
    }

    _usic_i2c_send(I2C_START, vnode->endpt | 0x1);
    
    // iov_base should be aligned
    ptr = uio->uio_iov->iov_base;
    offset = 0;
    low_add = (int)((uint32_t)ptr & 0x3); // lowest 2 bits
    ptr = (uint32_t*)((uint32_t)ptr & (~0x3));
    switch(low_add) {
    case 0:		// Aligned
	break;
    case 1:
	READ_INNER(1);
	if(uio->uio_resid > 0) { READ_INNER(2); }
	if(uio->uio_resid > 0) { READ_INNER(3); }
	break;
    case 2:
	READ_INNER(2);
	if(uio->uio_resid > 0) { READ_INNER(3); }
	break;
    case 3:
	READ_INNER(3);
	break;
    }
    if (uio->uio_resid == 0) {
	_usic_i2c_send(I2C_STOP, 0);
	return 0;
    }
    
    for(; uio->uio_resid > 0;uio->uio_resid--, ptr++) {
	READ_INNER(0);
	if(uio->uio_resid > 0) { READ_INNER(1); }
	if(uio->uio_resid > 0) { READ_INNER(2); }
	if(uio->uio_resid > 0) { READ_INNER(3); }
    }
    
    _usic_i2c_send(I2C_STOP, 0);
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