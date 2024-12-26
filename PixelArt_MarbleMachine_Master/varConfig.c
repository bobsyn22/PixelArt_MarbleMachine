// varConfig.c
#include "varConfig.h"
#include "imageVar.h"
#include <stdint.h>

// Define and initialize global variables
// Store instructions
volatile unsigned char dataByte1 = 0;
volatile unsigned char dataByte2 = 0;
//Current Y Position Values
volatile unsigned int currentDispenserPosByte = 0;
volatile unsigned int currentSelectorPosByte = 0;
volatile unsigned int currentHopperPosByte = 0;
//r - Input Values
volatile unsigned int inputDispenserPosByte = 0; 
volatile unsigned int inputSelectorPosByte = 0; 
volatile unsigned int inputHopperPosByte = 0; 
//Steps counter
volatile unsigned int selectorStepsCounter = 0;
volatile unsigned int hopperStepsCounter = 0;
// Counters for solenoid timer
volatile unsigned int solenoidCounter = 0;
volatile unsigned int hopperCounter = 0;
// Flags for sequential processes
volatile bool hopperFlag = false;
volatile bool selectorFlag = false;
volatile bool dispenserFlag = false;
volatile bool solenoidFlag = false;
//Colour variables
volatile char receivedByte = 0;
const char* receivedColour = "black";
//Image comparison vairables
extern volatile int currentRow = IMAGE_SIZE - 1; // Start from the bottom row
//extern volatile int currentRow = IMAGE_SIZE - 9;
extern volatile int currentCol = 1; // Start from the last column (rightmost)
//extern volatile int currentCol = 5; // Start from the last column (rightmost)
extern volatile bool colourMatchFlag = false;

// Stepper table mapping row/column positions to encoder values
const StepperPosition stepperTable[50] = {
    {2, 50}, {3, 50}, {4, 0}, {4, 200}, {5, 150}, {6, 85}, {6, 245}, {7, 150}, {8, 125}, {9, 50}, 
    {9, 235}, {10, 210}, {11, 125}, {12, 100}, {13, 15}, {13, 180}, {14, 85}, {15, 15}, {15, 155}, {16, 85}, 
    {17, 0}, {17, 160}, {18, 100}, {19, 0}, {19, 160}, {20, 60}, {20, 205}, {21, 110}, {22, 35}, {22, 225}, 
    {23, 150}, {24, 50}, {24, 200}, {25, 175}, {26, 100}, {27, 65}, {28, 0}, {28, 160}, {29, 100}, {30, 60},
    {30, 240}, {31, 200}, {32, 180}, {33, 150}, {34, 100}, {35, 50}, {35, 240}, {36, 200}, {37, 180}, {39, 0} 
};
