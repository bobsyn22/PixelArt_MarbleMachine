// setupfunctions.c
#include "intrinsics.h"
#include "msp430fr5739.h"
#include "varConfig.h"
#include "setupFunctions.h"
#include "imageVar.h"


//CLOCK Setup-----------------------------------------------------------------
void setupClocks(void){
    WDTCTL = WDTPW | WDTHOLD;   //turn off watchdog timer
    CSCTL0 = CSKEY;         //Input Clk key to change clk parameters.
    CSCTL1 |= DCOFSEL_3;    //Setup Digitally Controlled Oscilator Frequency to 8Mhz
    CSCTL2 |= (SELS__DCOCLK | SELA__DCOCLK | SELM__DCOCLK); //Setup SMClk & ACLK & MCLK using DCO
    CSCTL3 |= DIVA__8;      //Set frequency divider to 1 for 8Mhz for UART & Stepper PWMs
    CSCTL3 |= DIVS__1;      //Set frequency divider to 1 for 8MHZ for Master Clock
}

void setupUART(void) {
    // Configure UART with ACLK = 1MHz and Baud Rate = 9600
    UCA0MCTLW = 0;            // Clear previous settings

    // From Table 18-5 for ACLK = 1 MHz, Baud Rate = 9600
    UCA0MCTLW |= UCOS16;      // Enable oversampling mode (UCOS16 = 1)
    UCA0MCTLW |= (0x20 << 8); // Set UCBRSx = 0x20
    UCA0MCTLW |= (13 << 4);   // Set UCBRFx = 13
    UCA0BRW = 6;              // Set UCBR0 to 6
    UCA0CTLW0 |= UCSSEL__ACLK; // Select ACLK as the clock source
    UCA0CTLW0 &= ~UCSWRST;    // Reset UART system by clearing UCSWRST
    UCA0IE |= UCRXIE;         // Enable UART RX interrupt
    // Configure P2.0 and P2.1 for UART
    P2SEL1 |= (BIT0 + BIT1);
    P2SEL0 &= ~(BIT0 + BIT1);
}


//UART Setup--------------------------------------------------------------------------------------
void setup8MhzUART(void){

    //From Table 18-5, since Backgroung Clock 8MHz & Baud Rate 9600
    UCA0MCTLW |=UCOS16;     //Sets bit to 1 in register UCA0MCTLW for UCOS16
    UCA0MCTLW |=UCBRF0;     //Sets bits to 1 in register UCA0MCTLW for UCBRF0
    UCA0MCTLW |= 0x4900;    //Sets bits to 0x52 in register UCA0MCTLW for UCBRS0 (no bit shift)
    //(0x49<<8) Equivalent to this code
    UCA0BRW = 52;           //Set UCBR0 to 52. (All bits in register correspond to UCBR0;
    UCA0CTLW0|=UCSSEL__ACLK; //CLKsrc is SMCLK

    UCA0CTLW0&=~UCSWRST;    //Reset UART system by having USCWRST=1; 

    UCA0IE |= UCRXIE;   // Enable UART RX interrupt

    //Configure P2.0 & P2.1 For UART transmission
    P2SEL1 |=(BIT0+BIT1);
    P2SEL0 &=~(BIT0+BIT1);
}

//Timer B0 with Pins 1.4 used for stepper dispenser control at 125hz, 50% duty cycle.
void timerB0Setup(void) {
    TB0CTL = TBSSEL__ACLK | ID__1 | MC__UP | TBCLR; // SMCLK, no divider, Up mode, clear timer
    TB0CCR0 = 8000;                                 // 500 Hz PWM period (1 MHz SMCLK / 2000)

    // Configure TB0.1 for PWM output on P1.4
    TB0CCTL1 = OUTMOD_3;                            // Reset/Set output mode
    TB0CCR1 = 0;                                 // 50% duty cycle (1000 out of 2000)

    // Configure P1.4 for Timer B0 output
    P1DIR |= BIT4;                                  // Set P1.4 as output
    P1SEL1 &= ~BIT4;                                // Configure for Timer_B functionality
    P1SEL0 |= BIT4;
    
    // Enable Timer B0 CCR0 interrupt to count PWM cycles
    TB0CCTL0 |= CCIE; // Disable interrupt for CCR0
}


//Timer B1 with Pins 3.5 and 3.4 for stepper hopper and selector at 500Hz, 50% duty cycle.
void timerB1Setup(void) {
    TB1CTL = TBSSEL__ACLK | ID__1 | MC__UP |TBCLR; // Setup Timer B1 with SMCLK, Divider 1, up mode, clear timer

    // Configure TB1.1 for 50% duty cycle (500 Hz square wave on P3.4)
    TB1CCTL1 = OUTMOD_3; // Set output mode for TB1.1 to Reset/Set
    //TB1CCR1 = 4000;      // 50% duty cycle - Turns motor on
    TB1CCR1 = 0;
    TB1CCTL1 |= CCIE; // Turn off Interrupts

    //Configure CCR0 for period calculation. 1Mhz/500Hz = 2000;
    TB1CCR0 = 8000; // Set TB1CCR0 for 1000 Hz period (2ms)

    P3DIR |= BIT4;      // Set P3.4 as output
    P3SEL1 &= ~(BIT4);  // Configure for primary peripheral function
    P3SEL0 |= BIT4;     // Connect to Timer_B1

}

//Used to create a non-blocking timer for Solenoid actuation
void timerA0Setup(void) {
    TA0CTL = TASSEL__ACLK| ID__1 | MC__UP | TACLR; // SMCLK, no divider, Up mode, clear timer
    TA0CCTL0 |= OUTMOD_3 | CCIE;                         // Enable interrupt for CCR0
    TA0CCR1 = 0;                             // 0% duty cycle Off to start with
    TA0CCR0 = 8000;                          // Set CCR0 for 1ms at 8 MHz (8000 cycles)
}

// Timer A1 Setup for Hopper Motor (on P1.2) at 500Hz, 50% duty cycle
void timerA1Setup(void) {
    TA1CTL = TASSEL__ACLK | ID__1 | MC__UP | TACLR; // Setup Timer A1 with ACLK, Divider 1, up mode, clear timer

    // Configure TA1.1 for 50% duty cycle (500 Hz square wave on P1.2)
    TA1CCTL1 = OUTMOD_3; // Set output mode for TA1.1 to Reset/Set
    TA1CCR1 = 0;         // Initialize with 0 duty cycle (motor off)
    TA1CCTL1 |= CCIE;    // Enable interrupts if necessary (can disable later)

    // Configure CCR0 for period calculation. 1Mhz/500Hz = 2000
    TA1CCR0 = 8000; // Set TA1CCR0 for 500 Hz period (2ms)

    P1DIR |= BIT2;      // Set P1.2 as output
    P1SEL1 &= ~BIT2;    // Configure P1.2 for primary peripheral function
    P1SEL0 |= BIT2;     // Connect to Timer_A1
}



//Setup pins for dispenser direction
void setupSystemPins(){
    // Set P1.0 & 1.1 as output & Turn on CCW Direction DC Driver
    P1DIR |= (CCWDirnPin | CWDirnPin);
    P1OUT &= ~(CCWDirnPin | CWDirnPin); // Start with CCW Off
    
    //Set P2.6 for solenoid ouput
    P3DIR |= SolPin;
    P3OUT &= ~SolPin; //Start with off

}

// Transmit over UART A0--------------------------------------------------------------------------------------------------------------
void UARTTx(char TxByte){
    //UCTXIFG will set when system ready to send more data.
    //UC10IFG is the register holding several flags, including transmit and recieve.
    while (!(UCA0IFG & UCTXIFG)); // Wait until the previous Transmission is finished
    UCA0TXBUF = TxByte; // Transmit byte to register
}

// Receive data over UART A0----------------------------------------------------------------------
char UARTRx(void) {
    // Wait until data is received (UCA1RXIFG flag is set when data is ready to be read)
    while (!(UCA0IFG & UCRXIFG)); // Check the receive flag in the interrupt flag register
    return UCA0RXBUF; // Read the received byte from the buffer
}

void serialReset(){
    char receivedChar = 0;

    // Wait until 'a' is received
    while (receivedChar != 'a') {
        while (!(UCA0IFG & UCRXIFG)) {
            //UARTTx('W');  // Transmit 'W' while waiting for data
        }
        receivedChar = UCA0RXBUF;  // Read the received character
        UARTTx(receivedChar);      // Echo the received character for debugging
    }
}


void actuateDispenser() {
    if (!solenoidFlag && !hopperFlag && !selectorFlag && !dispenserFlag) {
        dispenserFlag = true;
        // Fetch the stepper position for the current column
        unsigned char dataByte1 = stepperTable[currentCol].highByte;
        unsigned char dataByte2 = stepperTable[currentCol].lowByte;

        // Combine high and low bytes into inputDispenserPosByte
        inputDispenserPosByte = ((dataByte1 << 8) | dataByte2) - 50 ;
    }
}

void actuateSolenoid() {
    if (!solenoidFlag && !hopperFlag && !selectorFlag && !dispenserFlag) {
        solenoidFlag = true;
        P3OUT |= SolPin;             // Open solenoid
        solenoidCounter = 0;         // Reset counter
        TA0CCR1 = 4000;              // Start Timer A0 in Up Mode
    } 
}

void actuateHopper() {
    if (!solenoidFlag && !hopperFlag && !selectorFlag && !dispenserFlag) {
        //hopperFlag = true;
        TA1CCR1 = 4000; //Actuate Hopper
        inputHopperPosByte = 115; //set target pos value
    } 
}

void actuateSelector() {
    if (!solenoidFlag && !hopperFlag && !selectorFlag && !dispenserFlag) {
        selectorFlag = true;
        TB1CCR1 = 4000; // Actuate Selector
        inputSelectorPosByte = 40;
    }
}

void delay250ms(void) {
    volatile unsigned long i;
    for (i = 0; i < 45000; i++) {
        __no_operation(); // Each iteration takes roughly 4 clock cycles
    }

}

void setReceivedColour(char receivedByte){
    switch (receivedByte) {
        case 1: // Color 1 (e.g., Black)
            receivedColour = "b";
            break;
        case 2: // Color 2 (e.g., White)
            receivedColour = "w";
            break;
        case 3: // Color 3 (e.g., Grey)
            receivedColour = "g";
            break;
        case 4:
            receivedColour = "n";
            break;
    }
}



