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
#include <stdarg.h>
#include "syscall.h"

/*
 * %[flags][width][length]specifier 
 */

#define FMT_FLAG_PLUS	1
#define FMT_FLAG_MINUS	2
#define FMT_FLAG_SPACE	4
#define FMT_FLAG_SHARP	8
#define FMT_FLAG_ZERO	16

#define FMT_LEN_HH	1
#define FMT_LEN_H	2
#define FMT_LEN_L	4
#define FMT_LEN_LL	8

#define FMT_SPEC_I	1
#define FMT_SPEC_U	2
#define FMT_SPEC_C	4
#define FMT_SPEC_S	8

struct s_fmt {
    char *p;
    int  flags;
    int  width;
    int  length;
    int  specifier;
};

void
fmt_flags(struct s_fmt *fmt)
{
    switch (*(fmt->p)) {
    case '+':
	fmt->flags += FMT_FLAG_PLUS;
	fmt->p++;
	break;
    case '-':
	fmt->flags += FMT_FLAG_MINUS;
	fmt->p++;
	break;
    case ' ':
	fmt->flags += FMT_FLAG_SPACE;
	fmt->p++;
	break;
    case '#':
	fmt->flags += FMT_FLAG_SHARP;
	fmt->p++;
	break;
    case '0':
	fmt->flags += FMT_FLAG_ZERO;
	fmt->p++;
	break;
    }
    
    return;
}

void
fmt_width(struct s_fmt *fmt)
{
    fmt->width = 0;

    while (*(fmt->p) <= '9' && *(fmt->p) >= '0') {
	fmt->width = fmt->width * 10;
	switch (*(fmt->p)) {
	case '0': fmt->width += 0; break;
	case '1': fmt->width += 1; break;
	case '2': fmt->width += 2; break;
	case '3': fmt->width += 3; break;
	case '4': fmt->width += 4; break;
	case '5': fmt->width += 5; break;
	case '6': fmt->width += 6; break;
	case '7': fmt->width += 7; break;
	case '8': fmt->width += 8; break;
	case '9': fmt->width += 9; break;
	}
	fmt->p++;
    }
    
    return;
}

void
fmt_length(struct s_fmt *fmt)
{
    if (*(fmt->p) == 'h') {
	fmt->p++;
	if (*(fmt->p) == 'h') {
	    fmt->p++;
	    fmt->length += FMT_LEN_HH;
	} else {
	    fmt->length += FMT_LEN_H;
	}
    } else if (*(fmt->p) == 'l') {
	fmt->p++;
	if (*(fmt->p) == 'l') {
	    fmt->p++;
	    fmt->length += FMT_LEN_LL;
	} else {
	    fmt->length += FMT_LEN_L;
	}
    }
    
    return;
}

void
fmt_specifier(struct s_fmt *fmt)
{
    switch (*(fmt->p)) {
    case 'i':
    case 'd':
	fmt->p++;
	fmt->specifier += FMT_SPEC_I;
	break;
    case 'u':
	fmt->p++;
	fmt->specifier += FMT_SPEC_U;
	break;
    case 'c':
	fmt->p++;
	fmt->specifier += FMT_SPEC_C;
	break;
    case 's':
	fmt->p++;
	fmt->specifier += FMT_SPEC_S;
	break;
    }
    
    return;
}

void
ui2a(unsigned long long int num, unsigned int base,char * bf)
{
    int n=0;
    unsigned int d=1;
    while (num/d >= base)
	d*=base;
    while (d!=0) {
	int dgt = num / d;
	num-=dgt*d;
	d/=base;
	if (n || dgt>0|| d==0) {
	    *bf++ = dgt+(dgt<10 ? '0' : 'a'-10);
	    ++n;
	}
    }
    *bf='\0';
}

void
i2a(long long int num, unsigned int base,char * bf)
{
    if (num<0) {
	num=-num;
	*bf++ = '-';
    }
    ui2a(num,base,bf);
}

int
cnprintf(void(*cb)(char), int size,char *fmt, va_list va)
{
    char bf[12];
    char *p = fmt;
    struct s_fmt s_fmt = {.p = p, .flags = 0, .width = 0, .length = 0, .specifier = 0};

    while (*p != '\0' && (p-fmt) < size) {
	if (*p != '%') {
	    cb(*p);
	    p++;
	} else {
	    s_fmt.p = ++p;
	    fmt_flags    (&s_fmt);
	    fmt_width    (&s_fmt);
	    fmt_length   (&s_fmt);
	    fmt_specifier(&s_fmt);
	    p = s_fmt.p;
	    switch (s_fmt.specifier) {
	      case (FMT_SPEC_I):
		  if (s_fmt.length <= FMT_LEN_H)
		      i2a(va_arg(va, int), 10, bf);
		  else if (s_fmt.length == FMT_LEN_L)
		      i2a(va_arg(va, long int), 10, bf);
		  else if (s_fmt.length == FMT_LEN_LL) {
		      bf[0] = 'N';bf[1] = 'a';bf[2] = 'N';bf[3] = '\0';}
		  break;
	      case (FMT_SPEC_U):
		  if (s_fmt.length <= FMT_LEN_H)
		      ui2a(va_arg(va, unsigned int), 10, bf);
		  else if (s_fmt.length == FMT_LEN_L)
		      ui2a(va_arg(va, unsigned long int), 10, bf);
		  else if (s_fmt.length == FMT_LEN_LL) {
		      bf[0] = 'N';bf[1] = 'a';bf[2] = 'N';bf[3] = '\0';}
		  break;
	      case (FMT_SPEC_C):
		  bf[0] = va_arg(va, int);
		  bf[1] = '\0';
		  break;
	      case (FMT_SPEC_S):
		  bf[0] = 'N';bf[1] = 'a';bf[2] = 'S';bf[3] = '\0';
		  break;
	    }
	    char *b = bf;
	    while (*b != '\0') {
		cb(*b);
		b++;
	    }
	}
    }
    
    return 0;
}

void
cb_printf(char c)
{
    sys_write(1, &c, 1);
}

int
printf(char *fmt, ...)
{
    va_list va;
    va_start(va,fmt);
    int r = cnprintf(cb_printf, 128,fmt, va);
    va_end(va);
    
    return r;
}
