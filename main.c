#include <msp430.h> 
#include <string.h>
#include "bsp.h"

// Prototypes:
void COM_Init(void);


// Ancillary functions
void COM_Init(void)
{
    P3SEL |= 0x30;                  // P3.4,.5 = USCI_A0 TXD/RXD
    UCA0CTL1 = UCSSEL_2;

#if (BSP_CONFIG_CLOCK_MHZ_SELECT == 1)
    UCA0BR0 = 104;                  // 9600 from 1 MHz
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT ==2)
    UCA0BR0 = 0xDA;                           // 9600 from 2Mhz
    UCA0BR1 = 0x0;
    UCA0MCTL = UCBRS_6;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 4)
    UCA0BR0 = 0xA0;                           // 9600 from 4Mhz
    UCA0BR1 = 0x1;
    UCA0MCTL = UCBRS_6;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 6)
    UCA0BR0 = 0x7B;                           // 9600 from 6Mhz
    UCA0BR1 = 0x2;
    UCA0MCTL = UCBRS_3;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 8)
    UCA0BR0 = 0x41;                           // 9600 from 8Mhz
    UCA0BR1 = 0x3;
    UCA0MCTL = UCBRS_2;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 10)
    UCA0BR0 = 0x79;                           // 9600 from 10Mhz
    UCA0BR1 = 0x4;
    UCA0MCTL = UCBRS_7;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 12)
    UCA0BR0 = 0xE2;                           // 9600 from 12Mhz
    UCA0BR1 = 0x4;
    UCA0MCTL = 0;
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 16)
    UCA0BR0 = 0x82;                           // 9600 from 16Mhz
    UCA0BR1 = 0x6;
    UCA0MCTL = UCBRS_6;
#else
#error "ERROR: Unsupported clock speed.  Custom clock speeds are possible. See comments in code."
#endif

    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    __enable_interrupt();
}

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    
    return 0;
}
