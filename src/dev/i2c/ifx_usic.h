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

#ifndef DEV_I2C_IFX_USIC_H
#define DEV_I2C_IFX_USIC_H

#ifdef I2C_USIC_CH0
  #define BRG   USIC0_CH0_BRG
  #define CCR   USIC0_CH0_CCR
  #define DX0CR USIC0_CH0_DX0CR
  #define DX1CR USIC0_CH0_DX1CR
  #define FDR   USIC0_CH0_FDR
  #define INPR  USIC0_CH0_INPR
  #define KSCFG USIC0_CH0_KSCFG
  #define PCR   USIC0_CH0_PCR
  #define PSCR  USIC0_CH0_PSCR
  #define PSR   USIC0_CH0_PSR
  #define SCTR  USIC0_CH0_SCTR
  #define TCSR  USIC0_CH0_TCSR
  #define RBUF  USIC0_CH0_RBUF
  #define TBUF  USIC0_CH0_TBUF
  
  #define SCL_PORT  P0_IOCR8
  #define SDA_PORT  P0_IOCR12  
  #define SCL_PIN   GPIO_PI8
  #define SDA_PIN   GPIO_PI14
  #define SCL_ALT   GPIO_OUTPUT_ALT6
  #define SDA_ALT   GPIO_OUTPUT_ALT6
#endif

#ifdef I2C_USIC_CH1
  #define BRG   USIC0_CH1_BRG
  #define CCR   USIC0_CH1_CCR
  #define DX0CR USIC0_CH1_DX0CR
  #define DX1CR USIC0_CH1_DX1CR
  #define FDR   USIC0_CH1_FDR
  #define INPR  USIC0_CH1_INPR
  #define KSCFG USIC0_CH1_KSCFG
  #define PCR   USIC0_CH1_PCR
  #define PSCR  USIC0_CH1_PSCR
  #define PSR   USIC0_CH1_PSR
  #define SCTR  USIC0_CH1_SCTR
  #define TCSR  USIC0_CH1_TCSR
  #define RBUF  USIC0_CH1_RBUF
  #define TBUF  USIC0_CH1_TBUF
  
  #pragma message "No Pinmux defined for CH1"
#endif

#endif