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

#include "circular_buffer.h"
#include "syscall.h"

#define _readable (cb->writes - cb->reads - cb->overflows)

void
cbuff_init(struct circular_buffer *cb, unsigned short size)
{
  cb->buffer 		= sys_sbrk(size * sizeof(union cb_element));
  cb->writes		= 0;
  cb->reads		= 0;
  cb->overflows		= 0;
  cb->start_valid	= 0;
  cb->end_valid		= 0;
  cb->size 		= size;
}

int
cbuff_is_full(struct circular_buffer *cb)
{
    return _readable == cb->size;
}

int
cbuff_is_overflowed(struct circular_buffer *cb)
{
    return cb->overflows;
}


struct cb_read
cbuff_read(struct circular_buffer *cb)
{
    struct cb_read ret;
    ret.value.i = 0;
    ret.error = 0;
    
    if (_readable == 0) {
	ret.error = 1;
	return ret;
    }
    
    ret.value =cb->buffer[cb->start_valid];
    
    cb->start_valid++;
    cb->reads++;
    if (cb->start_valid == cb->size) {
	cb->start_valid = 0;
    }
    
    return ret;
}

void
cbuff_write(struct circular_buffer *cb, union cb_element el)
{
    if (_readable == cb->size) {
      cb->overflows++;
    }
    
    cb->buffer[cb->end_valid] = el;
    
    cb->writes++;
    cb->end_valid++;
    if (cb->end_valid == cb->size) {
	cb->end_valid = 0;
    }
}

union
cb_element cb_element_int(int t) 
{
  union cb_element e;
  e.i = t;
  return e;
}

union
cb_element cb_element_float(float t) 
{
  union cb_element e;
  e.f = t;
  return e;
}

union
cb_element cb_element_char(char t) 
{
  union cb_element e;
  e.c = t;
  return e;
}
