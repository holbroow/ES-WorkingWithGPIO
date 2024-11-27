// Will Holbrook CW2 for 'SCC369: Embedded Systems' at Lancaster University (November 2024)
#include "MicroBit.h"

#define BBPIN1 MICROBIT_PIN_UART_TX    // BitBang Pin Serial - Pin 6 on Port 0
#define BBPIN2 MICROBIT_PIN_P2         // BitBang Pin Edge 2 - Edge Pin 2

#define ACCEL_X_L_REG 0x28    // OUT_X_L_A register address for X axis low byte
#define ACCEL_X_H_REG 0x29    // OUT_X_H_A register address for X axis high byte
#define ACCEL_Y_L_REG 0x2A    // OUT_Y_L_A register address for Y axis low byte
#define ACCEL_Y_H_REG 0x2B    // OUT_Y_H_A register address for Y axis high byte
#define ACCEL_Z_L_REG 0x2C    // OUT_Z_L_A register address for Z axis low byte
#define ACCEL_Z_H_REG 0x2D    // OUT_Z_H_A register address for Z axis high byte


//###################################################################################
// Write your code for Subtask 1 here (and leave these separators in your code!):

// Initialise timer for use
void initTimer(void) {
    NRF_TIMER1->TASKS_STOP = 1;                           // Ensure any tasks are finished
    NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;             // Set the timer to timer mode
    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;    // Set to 32 bit mode
    NRF_TIMER1->PRESCALER = 4;                            // Prescaler to 4
    NRF_TIMER1->TASKS_CLEAR = 1;                          // Clear tasks ready for timer use
}

// Initialise serial for use
void initSerial(void) {
    // Set both pins to output and set them high
    NRF_P0->DIRSET = (GPIO_DIR_PIN6_Output << BBPIN1)|
                     (GPIO_DIR_PIN4_Output << BBPIN2);
    NRF_P0->OUTSET = (1UL << BBPIN1)|
                     (1UL << BBPIN2);
}

// 'Bit Bang' a string to the serial bus
void bitBangSerial(char *string) {
    static bool init = false;
    if (!init) {
        initSerial();   // Initialise serial
        initTimer();    // Initialize timer
        init = true;    // Ensure the above happens once
    }

    while (*string) {
        // Set up 'character' and 'bitsToTransmit' variables to assist with data transmission
        char character = *string++;     // Grab next character from string parameter for transmission
        uint8_t bitsToTransmit = 0;     // We begin at 0 and count to 10 (1 start + 8 data + 1 stop)

        while(bitsToTransmit != 10) {                               // While we haven't yet transmitted all bits...
            // Clear and start the timer
            NRF_TIMER1->TASKS_CLEAR = 1;                            // Clear all current timer tasks
            NRF_TIMER1->CC[0] = (1000000/115200);                   // Set the compare register to ~9ms
            NRF_TIMER1->EVENTS_COMPARE[0] = 0;                      // Ensure that comparison register is essentially false
            NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;   // Configure the timer to clear upon matching compare
            NRF_TIMER1->TASKS_START = 1;                            // Start timer!

            // Determine and output the next bit
            if (bitsToTransmit == 0) {                                              // If on first bit...
                // Start bit
                NRF_P0->OUTCLR = (1 << BBPIN1)|                                     // Send the start bit
                                 (1 << BBPIN2);
            } else if (bitsToTransmit >= 1 && bitsToTransmit <= 8) {                // If on a data bit...
                // Character bit (set bit = OUTSET, unset bit = OUTCLR)
                uint8_t data_bit = (character >> (bitsToTransmit - 1)) & 0x01;      // Send said data bit
                if (data_bit) {
                    NRF_P0->OUTSET = (1 << BBPIN1)|
                                     (1 << BBPIN2);
                } else {
                    NRF_P0->OUTCLR = (1 << BBPIN1)|
                                     (1 << BBPIN2);
                }
            } else if (bitsToTransmit == 9) {                                       // If on last bit...
                // Stop bit
                NRF_P0->OUTSET = (1 << BBPIN1)|                                     // Send the stop bit
                                 (1 << BBPIN2);
            }

            // Wait for the timer to complete
            while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);     // Wait for out 9ms timer
            NRF_TIMER1->TASKS_STOP = 1;                     // Stop timer once finished
            
            // We're 1 bit closer to the end of the character transmission
            bitsToTransmit++;                               // Next bit!
        }
    }
}

// Repeatedly print a string to the bus using our 'Bit Bang' function
void voteForChocolate(void) {
    bitBangSerial(" ... Subtask 1 running ... \n");
    initTimer();        // Initialise the timer for use

    while (1) {         // Do forever...
        bitBangSerial("Twix\n");        // Output 'Twix' to serial (Unreal choice btw...)

        // Clear and start the timer
        NRF_TIMER1->TASKS_CLEAR = 1;                            // Clear all timer tasks
        NRF_TIMER1->CC[0] = (240 * 1000);                       // Set compare register to 240ms as requested in the spec
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;                      // Set compare check to false ready for comparison
        NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;   // Configure the timer to clear upon matching compare
        NRF_TIMER1->TASKS_START = 1;                            // Start timer!

        // Wait for the timer to complete
        while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);             // Wait for 240ms timer to finish...

        // Stop and clear the timer
        NRF_TIMER1->TASKS_STOP = 1;                             // Stop the timer
    };  // NB this function never returns
}


//###################################################################################
// Write your additional code for Subtask 2 here:

// Initialise the accelerometer
void initAccel(void) {
    NRF_TWI0->PSEL.SCL = MICROBIT_PIN_INT_SCL;                                          // Set TWI SCL pin to that of the microbit
    NRF_TWI0->PSEL.SDA = MICROBIT_PIN_INT_SDA;                                          // Same as above for the SDA pin
    NRF_TWI0->ADDRESS = 0x19;  // Set I2C address                                       // Give TWI the address of I2C
    NRF_TWI0->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;  // Set the I2C frequency to 100kHz
    NRF_TWI0->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;              // Enable I2C!

    // Send accelerometer control register
    NRF_TWI0->EVENTS_TXDSENT = 0;               // Set 'has been sent' check to 0
    NRF_TWI0->TXD = 0x20;                       // Set up 0x20 for transmisson
    NRF_TWI0->TASKS_STARTTX = 1;                // Start transmitting
    while (!NRF_TWI0->EVENTS_TXDSENT);          // Wait until finsihed transmitting

    // Send data
    NRF_TWI0->EVENTS_TXDSENT = 0;               // Set 'has been sent' check to 0
    NRF_TWI0->TXD = 0b01010111;                 // Set up I2C configuration for transmisson
    while (!NRF_TWI0->EVENTS_TXDSENT);          // Wait until finsihed transmitting

    // Finish
    NRF_TWI0->EVENTS_STOPPED = 0;               // Set 'has task stopped' check to 0
    NRF_TWI0->TASKS_STOP = 1;                   // Stop tasks as we are finished transmitting!
    while(!NRF_TWI0->EVENTS_STOPPED);           // Wait until so...

    // We are now initialised for Accelerometer use!
}

// Get an accelerometer sample using I2C and TWI
int getAccelerometerSample(char axis) {
    // Initialize the accelerometer if not already done
    static bool init = false;
    if (!init) {
        initAccel();                        // Initialise accelerometer
        init = true;                        // Ensure the above happens once
    }

    // Determine the register address based on the axis
    int regAddrHi;              // Define var for higher bit address
    int regAddrLo;              // Define var for lower bit address
    switch (axis) {             // Based on the given axis parameter...
        case 'X':
            regAddrLo = ACCEL_X_L_REG;   // OUT_X_L_A
            regAddrHi = ACCEL_X_H_REG;   // OUT_X_H_A
            break;
        case 'Y':
            regAddrLo = ACCEL_Y_L_REG;   // OUT_Y_L_A
            regAddrHi = ACCEL_Y_H_REG;   // OUT_Y_H_A
            break;
        case 'Z':
            regAddrLo = ACCEL_Z_L_REG;   // OUT_Z_L_A
            regAddrHi = ACCEL_Z_H_REG;   // OUT_Z_H_A
            break;
        default:
            break;
    }

    uint16_t data[2] = {0};  // Buffer for received data (2 bytes)

    // Write the chosen register address to the accelerometer to begin reading
    NRF_TWI0->EVENTS_TXDSENT = 0;         // Clear stop event
    NRF_TWI0->TXD = regAddrLo;            // Point to register address
    NRF_TWI0->TASKS_STARTTX = 1;          // Start the transmission
    while (!NRF_TWI0->EVENTS_TXDSENT);    // Wait until transmission completes

    // Read both the low and high bytes of data
    // Start reception and read first byte
    NRF_TWI0->EVENTS_RXDREADY = 0;        // Clear ready event
    NRF_TWI0->TASKS_STARTRX = 1;          // Start reception
    while (!NRF_TWI0->EVENTS_RXDREADY);   // Wait until reception completes
    NRF_TWI0->EVENTS_STOPPED = 0;         // Once completed, reset stopped check
    NRF_TWI0->TASKS_STOP = 1;             // Stop tasks
    while (!NRF_TWI0->EVENTS_STOPPED);    // Wait until so...

    data[0] = NRF_TWI0->RXD;              // Read first byte into our data array!

    NRF_TWI0->EVENTS_TXDSENT = 0;         // Clear sent event
    NRF_TWI0->TXD = regAddrHi;            // Point to axis 'Hi' register address
    NRF_TWI0->TASKS_STARTTX = 1;          // Start the transmission
    while (!NRF_TWI0->EVENTS_TXDSENT);    // Wait until transmission completes

    // Read second byte
    NRF_TWI0->EVENTS_RXDREADY = 0;        // Clear ready event
    NRF_TWI0->TASKS_STARTRX = 1;          // Start reception
    while (!NRF_TWI0->EVENTS_RXDREADY);   // Wait until reception completes
    NRF_TWI0->EVENTS_STOPPED = 0;         // Once completed, reset stopped check
    NRF_TWI0->TASKS_STOP = 1;             // Stop tasks
    while (!NRF_TWI0->EVENTS_STOPPED);    // Wait until so...

    NRF_TWI0->TASKS_STOP = 0;             // Reset stopped event

    data[1] = NRF_TWI0->RXD;              // Read second byte into our data array!

    // Combine the two bytes into a single signed integer
    int16_t result = ((int16_t)(data[1] << 8) | data[0]);   // Combine the two bytes into an Unsigned 16 bit integer using bit shifting
    result = result >> 6;                                   // Shift bits 6 places right due to being left-justified

    // Return result
    return result;
}

// Format and read-out accelerometer samples
void showAccelerometerSamples(void) {
    bitBangSerial(" ... Subtask 2 running ... \n");
    char string[50];    // Var for final string to be outputted
    char buffer[25];    // Buffer var to aid the creation of said string with included formatting

    while (1) {
        int x = getAccelerometerSample('X');    // Grab X accelerometer reading
        int y = getAccelerometerSample('Y');    // Grab Y accelerometer reading
        int z = getAccelerometerSample('Z');    // Grab Z accelerometer reading

        // Start constructing the message
        int index = 0;      // Index identifying which char of the string we're working with

        // Add "[X: "
        const char* prefixX = "[X: ";
        for (int i = 0; prefixX[i] != '\0'; i++) {
            string[index++] = prefixX[i];
        }

        // Convert x to a string and add it
        itoa(x, buffer);
        for (int i = 0; buffer[i] != '\0'; i++) {
            string[index++] = buffer[i];
        }

        // Add "] [Y: "
        const char* prefixY = "] [Y: ";
        for (int i = 0; prefixY[i] != '\0'; i++) {
            string[index++] = prefixY[i];
        }

        // Convert y to a string and add it
        itoa(y, buffer);
        for (int i = 0; buffer[i] != '\0'; i++) {
            string[index++] = buffer[i];
        }

        // Add "] [Z: "
        const char* prefixZ = "] [Z: ";
        for (int i = 0; prefixZ[i] != '\0'; i++) {
            string[index++] = prefixZ[i];
        }

        // Convert z to a string and add it
        itoa(z, buffer);
        for (int i = 0; buffer[i] != '\0'; i++) {
            string[index++] = buffer[i];
        }

        // Add "]\n" to complete the string
        const char* end = "]\n";
        for (int i = 0; end[i] != '\0'; i++) {
            string[index++] = end[i];
        }

        // Add /r/n as the spec calls for...
        string[index++] = '\r';
        string[index++] = '\n';

        // Null termination char
        string[index] = '\0';

        // Output the string
        bitBangSerial(string);

        // Pause for 200ms
        bool init = false;
        if (!init) {
            initTimer();        // Ensure that the timer is initialised
            init = true;        // Ensure this happens once
        }
        NRF_TIMER1->TASKS_CLEAR = 1;                            // Clear all tasks for the timer
        NRF_TIMER1->CC[0] = (200 * 1000);                       // Set the compare register to 200ms
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;                      // Clear the compare check
        NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;   // Configure the timer to clear upon matching compare
        NRF_TIMER1->TASKS_START = 1;                            // Start the timer...
        while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);             // Wait until value is met/counted to
        NRF_TIMER1->TASKS_STOP = 1;                             // Stop the timer!
    };  // NB this function never returns
}


//###################################################################################
// Write your additional code for Subtask 3 here:

// PWM frequency value declared here in-case we lose scope, PWM seems more reliable in my implementation this way...
uint16_t pwm_seq;

// Initialise PWM
void initPWM(void) {
    // Assign a PSEL channel to the Speaker pin/port
    NRF_GPIO->PIN_CNF[MICROBIT_PIN_SPEAKER] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);    // Set speaker pin to a GPIO output
    NRF_PWM0->PSEL.OUT[0] = (MICROBIT_PIN_SPEAKER << PWM_PSEL_OUT_PIN_Pos) |                    
                            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);           // Connect the speaker pin to PWM channel 0

    // Enable PWM
    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);                      // Enable PWM
    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);                                 // Set PWM mode to Up-Down
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);         // Prescaler to 1 to ensure a clock of 1KHz
    NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos);                        // Common decoder as we're using a single value
}

// Initialise buttons A and B
void initButtons(void) {
    // Set buttons A and B as inputs
    NRF_P0->DIR = (GPIO_DIR_PIN14_Input << MICROBIT_PIN_BUTTON_A) |
                  (GPIO_DIR_PIN23_Input << MICROBIT_PIN_BUTTON_B);

    // Set the pin configuration for both pin 14 (Button A) and pin32 (Button B)
    NRF_GPIO->PIN_CNF[MICROBIT_PIN_BUTTON_A] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                               (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                               (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
    
    NRF_GPIO->PIN_CNF[MICROBIT_PIN_BUTTON_B] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                               (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                               (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
}

// Check if either button is pressed
bool buttonPressed() {
    // If button A or B is pressed, return true
    if ((!(NRF_P0->IN & (1 << MICROBIT_PIN_BUTTON_A))) || (!(NRF_P0->IN & (1 << MICROBIT_PIN_BUTTON_B)))) { // active low check
        return true;
    }
    // Otherwise, return false
    return false;
}

// Sound the speaker using the accelerometer 'Y' value on button press
void makeNoise(void) {
    bitBangSerial(" ... Subtask 3 running ... \n");
    bool init = false;
    if (!init) {
        initPWM();
        initButtons();  //  Initialise buttons for GPIO input
        init = true;    //  Do this only once!
    }

    NRF_PWM0->SEQ[0].PTR = (uint32_t)&pwm_seq;      // Assign a pointer to the declared frequency value for PWM to use
    NRF_PWM0->SEQ[0].CNT = 1;                       // PWM is to expect only 1 value (the frequency)
    NRF_PWM0->TASKS_SEQSTART[0] = 1;                // Start the 'task'

    while (1) {
        // If either button A or B is pressed...
        if (buttonPressed()) {
            // Get 'Y' value from accelerometer
            int y = getAccelerometerSample('Y');
            int scaled_value = (y + 512); // value is scaled for (-512 - 511) to (0 - 1023)

            // Multiply the value to scale more appropriately with PWM
            int counter_top = scaled_value * 20;

            // Set countertop and frequency for PWM
            NRF_PWM0->COUNTERTOP = (counter_top << PWM_COUNTERTOP_COUNTERTOP_Pos);
            pwm_seq = (counter_top/2);
            NRF_PWM0->TASKS_SEQSTART[0] = 1;
        } else  {
            // If no button is pressed, End the task if currently running
            NRF_PWM0->TASKS_STOP = 1;
        }
    }
}
