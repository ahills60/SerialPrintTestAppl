#include <msp430.h> 
//#include <string.h>
//#include "bsp.h"


// Prototypes:
void COM_Init(void);
void TXString(char*, unsigned int);
void transmitData(unsigned int);
void lfsr_16_14(void);

__interrupt void Timer_Tick(void);

// Global variables
unsigned int LFSR_WORD = 0x0001;

// Ancillary functions

/*
    Serial port communication initialisation
*/
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
    Transmit a string
*/
void TXString(char *str, unsigned int len)
{
    unsigned int ptr;
    
    for (ptr = 0; ptr < len; ptr++)
    {
        UCA0TXBUF = str[ptr];        // Fill the buffer with this character
        while (!(IFG2 & UCA0TXIFG));        // Hold until ready
    }
}

/*
    Convert this data and transmit via serial
*/
void transmitData(unsigned int data)
{
    // Offset Idx:    012345678901234567890123
    char output[] = {"0000000000000000 (00000)"};
    unsigned int mask 0x8000;
    int n;
    
    // Binary sequence
    for (n = 0; n < 16; n++)
    {
        if (mask & data)
            output[n] = '1';
        mask = (mask >> 1) & 0x7FFF;
    }
    
    // Now translate the number to text
    output[22] = '0' + (data % 10);
    data /= 10;
    output[21] = '0' + (data % 10);
    data /= 10;
    output[20] = '0' + (data % 10);
    data /= 10;
    output[19] = '0' + (data % 10);
    data /= 10;
    output[18] = '0' + (data % 10);
    
    // Transmit via serial
    TXString(output, 24);
}

/*
    Apply a round of the 16-bit LFSR with a 14-bit sequence.
*/
void lfsr_16_14(void)
{
    unsigned int bit;
    
    // 14 bit sequence:
    bit = ((LFSR_WORD >> 0) ^ (LFSR_WORD >> 1) ^ (LFSR_WORD >> 3) ^ (LFSR_WORD >> 5)) & 0x0001;
    
    LFSR_WORD = (LFSR_WORD >> 1) | (bit << 13);
}

/*
    Interrupt timer clock tick
*/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_Tick(void)
{
    // Wake up:
    __bic_SR_register_on_exit(CPUOFF);
}


/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    
    // Initialise the board-specific hardware
    BSP_Init();
    
    // Initialise the timer
    BCSCTL3 |= LFXT1S_2;                        // LFXT1 = VLO
    TACCTL0 = CCIE;                             // TACCR0 interrupt enabled
    TACCR0 = 11628;                             // Approximately 1 sec
    TACTL = TASSEL_1 | MC_1;                    // ACLK, upmode;
    
    // Loop forever
    while (1)
    {
        // Go to sleep until a clock tick:
        __bis_SR_register(CPUOFF | GIE);
        
        // The unit is awake if here
        lfsr_16_14();
        transmitData(LFSR_WORD);
    }
    
    
    return 0;
}
