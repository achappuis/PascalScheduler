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
#ifndef DEV_UART_IFX_USIC_H
#define DEV_UART_IFX_USIC_H

#ifdef UART_USIC_CH0
# define UART_CHANNEL       XMC_UART0_CH0
# define UART_INPUT_SOURCE  USIC0_C0_DX0_P2_0
# define UART_TX_PIN        P2_1
# define UART_RX_PIN        P2_0
# define UART_TX_ALT        XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6
# define UART_RX_ALT        XMC_GPIO_MODE_INPUT_TRISTATE
#endif

#ifdef UART_USIC_CH1
# define UART_CHANNEL       XMC_UART0_CH1
# define UART_INPUT_SOURCE  USIC0_C1_DX0_P1_3
# define UART_TX_PIN        P1_2
# define UART_RX_PIN        P1_3
# define UART_TX_ALT        XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7
# define UART_RX_ALT        XMC_GPIO_MODE_INPUT_TRISTATE
#endif

#ifdef UART_19200
# define UART_BAUD_RATE 19200
#endif

#ifdef UART_115200
# define UART_BAUD_RATE 115200
#endif

#endif
