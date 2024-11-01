#include "MicroBit.h"

// Get a serial port running over USB back to the host PC
NRF52Pin    usbTx(ID_PIN_USBTX, MICROBIT_PIN_UART_TX, PIN_CAPABILITY_DIGITAL);
NRF52Pin    usbRx(ID_PIN_USBRX, MICROBIT_PIN_UART_RX, PIN_CAPABILITY_DIGITAL);
NRF52Serial serial(usbTx, usbRx, NRF_UARTE0);

// Declare the Subtasks in CW1.cpp so we can call them 
extern void    displayBinary(uint8_t);
extern void    countUpBinary(uint8_t);
extern void    countWithButtonsBinary(uint8_t);
extern uint8_t sampleVoltage();
extern void    displayVoltageBinary();
extern void    driveRGB();
extern void    countWithTouchesBinary(uint8_t);

// Entry point is a menu that allows any Subtask to be run
int main() {

    // Display instructions
    serial.printf("\nEnter a number 1-5 to run a CW1 subtask: ");
    
    while (1) {
        int in = serial.getChar(SYNC_SPINWAIT);   // get a character from serial
        serial.printf("%c", in);                  // echo it

        switch (in) {            // call a Subtask based on the character typed
            case '1':
                countUpBinary(0);
                break;
            case '2':
                countWithButtonsBinary(0);
                break;
            case '3':
                displayVoltageBinary();
                break;
            case '4':
                driveRGB();
                break;
            case '5':
                countWithTouchesBinary(0);
                break;
        }
        
    }
}