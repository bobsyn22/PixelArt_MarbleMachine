// config.h
#include <stdint.h>
#include <stdbool.h>
#include "imageVar.h"

#ifndef CONFIG_H
#define CONFIG_H

//Defintions for UART Queue 
# define QUEUE_SIZE 50
// Pin definitions for the stepper motor driver-----------------------------------------------------------
#define dispenserPin BIT4 //Pin 1.4
#define CWDirnPin BIT0 // P1.0
#define CCWDirnPin BIT1 // P1.1 
// Pin defitinions for solenoid
#define SolPin BIT0 //Pin 3.0
//Pin definitions for dispensero and hopper
#define selectorPin BIT4 //Pin 3.4
#define hopperPin BIT5  //Pin 3.5


// Declare global variables used across files
//process instructions Bytes
extern volatile unsigned char dataByte1;
extern volatile unsigned char dataByte2;
//Encoder Position bytes - y
extern volatile unsigned int currentDispenserPosByte;
extern volatile unsigned int currentSelectorPosByte;
extern volatile unsigned int currentHopperPosByte;
//Kp controller Bytes - r
extern volatile unsigned int inputDispenserPosByte;
extern volatile unsigned int inputSelectorPosByte;
extern volatile unsigned int inputHopperPosByte;
//Counters for motor hopper and seletor
extern volatile unsigned int selectorStepsCounter;
extern volatile unsigned int hopperStepsCounter;
//counter for solenoid timer
extern volatile unsigned int solenoidCounter;
extern volatile unsigned int hopperCounter;
// Flags for sequential processes
extern volatile bool hopperFlag;
extern volatile bool selectorFlag;
extern volatile bool dispenserFlag;
extern volatile bool solenoidFlag;
//Colour variables
extern volatile char receivedByte;
extern const char* receivedColour;
//Image comparison vairables
extern volatile int currentRow; // Start from the bottom row
extern volatile int currentCol; // Start from the last column (rightmost)
extern volatile bool colourMatchFlag;
//Video counter variable
extern volatile int counterSol;
extern volatile int targetSol;


// Struct to hold high and low byte for stepper positions
typedef struct {
    unsigned char highByte; // High byte (decimal value)
    unsigned char lowByte;  // Low byte (decimal value)
} StepperPosition;

// Declare the stepper table as an external variable
extern const StepperPosition stepperTable[50];

#endif // CONFIG_H
