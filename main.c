#include "driverlib.h"
#include "in430.h"
#include "intrinsics.h"
#include "machine/_types.h"
#include "msp430fr5739.h"
#include "msp430fr57xxgeneric.h"
#include <msp430.h>
#include <stdbool.h>
#include <string.h>

#include "setupFunctions.h"
#include "varConfig.h"
#include "imageVar.h"

// MAIN CODE LOGIC ---------------------------------------------------------------------------------------------
void main (void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    // Setup registers
    setupClocks(); //Setup clocks
    setupUART(); //Setup UART
    //serialReset(); //Serial Reset to ensure board isn't bricked
    timerB0Setup(); //Sets up Motor dispenser Timer P1.4
    timerB1Setup(); //Sets up blocking motor hopper and motor selector P3.4 and 3.5
    timerA0Setup(); //Used to create a non-blocking timer for solenoid P3.0
    timerA1Setup(); //Used for parallel hopper motion P1.2
    setupSystemPins(); //Sets up P1.0 and P1.1 for dirn ctrl for dispenser & P3.0 for solenoid.
    _EINT(); // Enable Global interrupts

    while(1){
        //Step 1: Move dispenser to position
        //actuateDispenser();
        while(dispenserFlag ==  true);
        //currentCol++;   //for testing

        //Step 2: Actuate Hopper to add marbles in
        actuateHopper();
        while(hopperFlag == true);

        //Step 3: Actuate Selector a constant position
        actuateSelector();
        while(selectorFlag == true);

        //Step 4: Send UART to MATLAB For Camera Vision & Wait for response (unsure if working correctly)
        //UARTTx(1);
        //while (receivedByte != 1 && receivedByte != 2 && receivedByte != 3 && receivedByte !=4 );
        //setReceivedColour(receivedByte);
        //receivedByte = 0;

        //Step 5: compare recieved color against image array
        if (strcmp(receivedColour, imageArray[currentRow][currentCol]) == 0) {
            colourMatchFlag = true;
        } else {
            colourMatchFlag = false;
        }   
        delay250ms();
        
        //Step 6: Actuate hopper again
        //actuateHopper(); 

        //For video:
        if (counterSol != targetSol) counterSol++;
        else{
            //if (targetSol == 5) targetSol = 2;
            //else if (targetSol == 2) targetSol = 3;
            //else if (targetSol == 3) targetSol = 4;
            //else if (targetSol == 4) targetSol = 1;
            //else if (targetSol == 1) targetSol = 5;
            colourMatchFlag = true;
            counterSol = 0;
        }

        //Step 7: move up row and/or coloumn and open solenoid if match is true
        if (colourMatchFlag == true){
            actuateSolenoid();
            while(solenoidFlag==true);
            currentRow--; //decrease 1 row
            // Check if we have reached the top of the column
            if (currentRow < 0) {
                currentRow = IMAGE_SIZE - 1;  // Reset currentRow to bottom row
                currentCol++; // Move to the next column to the left
                delay250ms();
                delay250ms();
            }
            
        }
        //reset if complete
        if (currentCol==50) break;
    }
}

// INTERRUPT SERVICE ROUTINES ---------------------------------------------------------------------------------
// Interrupt if data is received by UART
#pragma vector = USCI_A0_VECTOR
__interrupt void queueData()
{
    // Check correct flag has been set
    if (UCA0IFG & UCRXIFG) {
        receivedByte = UCA0RXBUF; // Store received byte
        UCA0IFG &= ~UCRXIFG;     // Clear the flag
    }
}

//ISR For Dispenser Motor - 1 axis control
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {
    if (inputDispenserPosByte > currentDispenserPosByte) {
        // Move CW
        P1OUT |= CWDirnPin;
        P1OUT &= ~CCWDirnPin;
        currentDispenserPosByte++;
        TB0CCR1 = 4000; // Keep PWM active
    } else if (inputDispenserPosByte < currentDispenserPosByte) {
        // Move CCW
        P1OUT |= CCWDirnPin;
        P1OUT &= ~CWDirnPin;
        currentDispenserPosByte--;
        TB0CCR1 = 4000; // Keep PWM active
    } else {
        // Stop the motor when target is reached
        TB0CCR1 = 0; // Disable PWM output
        //TB0CCTL0 &= ~CCIE; // Disable interrupt for CCR0
        dispenserFlag = false;
    }
    TB0CCTL0 &= ~CCIFG; //might be wrong
}

//ISR for motor selector and motor hopper - Unchanging position rotation
//I should figure out a better way to do this later! since it'll rotate once always
#pragma vector = TIMER1_B1_VECTOR
__interrupt void Timer1_B1_ISR(void) {
    unsigned int tb1iv_temp = TB1IV;  // Read TB1IV once

    if (tb1iv_temp == 0x02) {          // Check TB1CCR1 compare interrupt flag
        if (inputSelectorPosByte > currentSelectorPosByte) {
            currentSelectorPosByte++;
            TB1CCR1 = 4000; // Keep PWM active
        } else if (inputSelectorPosByte < currentSelectorPosByte) {
            currentSelectorPosByte--;
            TB1CCR1 = 4000; // Keep PWM active
        } else {
            // Stop the motor when target is reached
            TB1CCR1 = 0; // Disable PWM output
            inputSelectorPosByte = 0;
            currentSelectorPosByte = 0;
            //TB1CCTL1 &=~ CCIE; // Turn off Interrupts
            selectorFlag = false;
        }
    }
    TB1CCTL2 &= ~CCIFG;          // Clear TB1CCR2 interrupt flag
    TB1CCTL1 &= ~CCIFG;         // Clear TB1CCR1 interrupt flag
    TB1CCTL0 &= ~CCIFG;         // Clear TimerB0 interrupt flag

}

//ISR for Solenoid actuation
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA0_ISR(void) {
    if(solenoidCounter < 70) solenoidCounter++; //0.5s delay
    else {
        P3OUT &= ~SolPin;        // Close solenoid
        solenoidCounter = 0;      // Mark solenoid as inactive
        TA0CCR1 = 0;          // Stop Timer A0 duty cycle
        //TA0CCTL0 &= ~CCIE;
        solenoidFlag = false;
    }

    TA0CCTL0 &= ~CCIFG; // Clear interrupt flag
}

// Timer A1 ISR
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TimerA1_ISR(void) {
    switch (TA1IV) { // Check the interrupt source
        case 2: // CCR1 interrupt
        if (inputHopperPosByte > currentHopperPosByte) {
            currentHopperPosByte++;
            TA1CCR1 = 4000; // Keep PWM active
        } else if (inputHopperPosByte < currentHopperPosByte) {
            currentHopperPosByte--;
            TA1CCR1 = 4000; // Keep PWM active
        } else {
            // Stop the motor when target is reached
           TA1CCR1 = 0; // Disable PWM output
           inputHopperPosByte = 0;
           currentHopperPosByte = 0;
           hopperFlag = false;
        }
    }
}

