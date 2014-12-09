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

#define KSCFG_MODULE_EN	0x3

#define DXnCR_DXnA	0x0
#define DXnCR_DXnB	0x1
#define DXnCR_DXnC	0x2
#define DXnCR_DXnD	0x3
#define DXnCR_DXnE	0x4
#define DXnCR_DXnF	0x5
#define DXnCR_DXnG	0x6

#define FDR_DIV_OFF	(0x0 << 14)
#define FDR_DIV_NORM	(0x1 << 14)
#define FDR_DIV_FRAC	(0x2 << 14)

#define SCTR_PASSIVE_DATA_LVL_0	(0x0 << 1)
#define SCTR_PASSIVE_DATA_LVL_1 (0x1 << 1)

#define SCTR_DISABLE_TX	(0x0 << 8)
#define SCTR_ENABLE_TX	(0x1 << 8)

#define SCTR_FRAME_LEN(_a)	((_a) << 24)
#define SCTR_WORD_LEN(_a)	((_a) << 24)

// Define specific to UART
#define PCR_1_SAMPLE	(0x0 << 0)
#define PCR_3_SAMPLES	(0x1 << 0)
#define PCR_1_STOP	(0x0 << 1)
#define PCR_2_STOP	(0x1 << 1)
#define PCR_SAMPLE_POINT(_a)	((_a) << 8)

#ifndef DEV_UART_IFX_USIC_H
#define DEV_UART_IFX_USIC_H

#ifdef UART_USIC_CH0
  #define BRG    USIC0_CH0_BRG
  #define CCR    USIC0_CH0_CCR
  #define DX0CR  USIC0_CH0_DX0CR
  #define DX1CR  USIC0_CH0_DX1CR
  #define FDR    USIC0_CH0_FDR
  #define KSCFG  USIC0_CH0_KSCFG
  #define PCR    USIC0_CH0_PCR 
  #define PSR    USIC0_CH0_PSR
  #define RBCTR  USIC0_CH0_RBCTR
  #define SCTR   USIC0_CH0_SCTR
  #define TCSR   USIC0_CH0_TCSR   
  #define TRBSCR USIC0_CH0_TRBSCR
  #define TBCTR  USIC0_CH0_TBCTR
  #define TRBSR  USIC0_CH0_TRBSR
  #define IN     USIC0_CH0_IN
  #define OUTR   USIC0_CH0_OUTR
  #pragma message "No Pinmux defined for CH0"
#endif

#ifdef UART_USIC_CH1
  #define BRG    USIC0_CH1_BRG
  #define CCR    USIC0_CH1_CCR
  #define DX0CR  USIC0_CH1_DX0CR
  #define DX1CR  USIC0_CH1_DX1CR
  #define FDR    USIC0_CH1_FDR
  #define KSCFG  USIC0_CH1_KSCFG
  #define PCR    USIC0_CH1_PCR 
  #define PSR    USIC0_CH1_PSR
  #define RBCTR  USIC0_CH1_RBCTR
  #define SCTR   USIC0_CH1_SCTR
  #define TCSR   USIC0_CH1_TCSR   
  #define TRBSCR USIC0_CH1_TRBSCR
  #define TBCTR  USIC0_CH1_TBCTR
  #define TRBSR  USIC0_CH1_TRBSR
  #define IN     USIC0_CH1_IN
  #define OUTR   USIC0_CH1_OUTR

  #define RX_PIN   GPIO_PI6
  #define RX_PORT  P0_IOCR4
  #define TX_PIN   GPIO_PI7
  #define TX_PORT  P0_IOCR4
  #define TX_ALT   GPIO_OUTPUT_ALT7
#endif


// 19200 bd @8MHz
#ifdef UART_19200
  #define FDR_STEP 0x200
  #define BRG_PDIV 22
  #define BRG_DCTQ 8
  #define BRG_PCTQ 0
#endif


#endif