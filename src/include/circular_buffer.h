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

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

union cb_element {
  int i;	// 4
  float f;	// 4
  char c;	// 1
};// = 4

struct cb_read {
  union cb_element value;
  int error;
};

struct circular_buffer {
  union cb_element *buffer;	//4 -> aligné
  uint32_t writes;		//4 -> aligné
  uint32_t reads;		//4 -> aligné
  uint32_t overflows;		//4 -> aligné
  unsigned short start_valid;	//2
  unsigned short end_valid;	//2 -> aligné
  unsigned short size;		//2
};// = 22

void cbuff_init(struct circular_buffer*, unsigned short);
int cbuff_is_full(struct circular_buffer*);
int cbuff_is_overflowed(struct circular_buffer*);

struct cb_read cbuff_read(struct circular_buffer*);
void cbuff_write(struct circular_buffer*, union cb_element);

union cb_element cb_element_int(int t);
union cb_element cb_element_float(float t);
union cb_element cb_element_char(char t);

#endif