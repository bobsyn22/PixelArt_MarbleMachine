//functions.h
#ifndef SETUPFUNCTIONS_H
#define SETUPFUNCTIONS_H

//Included Headers
#include "driverlib.h"
#include "in430.h"
#include "machine/_types.h"
#include "msp430fr5739.h"
#include "msp430fr57xxgeneric.h"
#include "setupFunctions.h"
#include "varConfig.h"
#include <msp430.h>
#include <stdbool.h>


// DECLARE FUNCTIONS ------------------------------------------------------------------------
void setupClocks();
void setupUART();
void setup8MhzUART();
void processInstructions();
void timerB0Setup(void);
void timerB1Setup(void);
void timerA0Setup(void);
void timerA1Setup(void);
void setupSystemPins(void);
void actuateSolenoid(void);
void actuateHopper(void);
void actuateSelector(void);
void actuateDispenser(void);
void delay250ms(void);
void setReceivedColour(char receivedByte);
void UARTTx(char TxByte);
char UARTRx(void);
void serialReset();


#endif // SETUPFUNCTIONS.h
