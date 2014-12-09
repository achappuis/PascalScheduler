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

#ifndef DEV_GPIO_IFX_GPIO_H
#define DEV_GPIO_IFX_GPIO_H

#define GPIO_SHIFT		3

// Input Characteristics
#define GPIO_INPUT_INV		0x4  << GPIO_SHIFT
// Input Function
#define GPIO_INPUT_NO		0x0  << GPIO_SHIFT
#define GPIO_INPUT_PD		0x1  << GPIO_SHIFT
#define GPIO_INPUT_PU		0x2  << GPIO_SHIFT

// Output Characteristics
#define GPIO_OUTPUT_PP		0x10 << GPIO_SHIFT
#define GPIO_OUTPUT_OD		0x18 << GPIO_SHIFT
// Output Function
#define GPIO_OUTPUT_ALT0	0x0  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT1	0x1  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT2	0x2  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT3	0x3  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT4	0x4  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT5	0x5  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT6	0x6  << GPIO_SHIFT
#define GPIO_OUTPUT_ALT7	0x7  << GPIO_SHIFT

#define GPIO_PI0	0 
#define GPIO_PI4	0 
#define GPIO_PI8	0 
#define GPIO_PI12	0 

#define GPIO_PI1	8 
#define GPIO_PI5	8
#define GPIO_PI9	8 
#define GPIO_PI13	8 

#define GPIO_PI2	16 
#define GPIO_PI6	16 
#define GPIO_PI10	16 
#define GPIO_PI14	16 

#define GPIO_PI3	24 
#define GPIO_PI7	24
#define GPIO_PI11	24
#define GPIO_PI15	24

#define GPIO_PIN_MUX(_port,_conf,_pin)\
		_port &= 0xFF<<(_pin);\
		_port |= (_conf)<<(_pin)
		
#endif