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


}

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    
    return 0;
}
