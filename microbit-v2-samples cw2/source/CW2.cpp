#include "MicroBit.h"

#define BBPIN1 MICROBIT_PIN_UART_TX    // Pin 6 on Port 0
#define BBPIN2 MICROBIT_PIN_P2         // Edge Pin 2


//###################################################################################
// Write your code for Subtask 1 here (and leave these separators in your code!):

// Initialise timer for use
void initTimer() {
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER1->PRESCALER = 4;
    NRF_TIMER1->TASKS_CLEAR = 1;
}

// Initialise serial for use
void initSerial() {
    // Set direction and both to high
        NRF_P0->DIRSET = (1UL << BBPIN1)|
                         (1UL << BBPIN2);
        NRF_P0->OUTSET = (1UL << BBPIN1)|
                         (1UL << BBPIN2);
}

//
void bitBangSerial(char *string) {
    static bool init = false;
    if (!init) {
        // Initialise serial
        initSerial();
        // Initialize timer
        initTimer();
        // Ensure the above happens once
        init = true;
    }

    while (*string) {
        char character = *string++;
        uint8_t bitsToTransmit = 0; // 1 start + 8 data + 1 stop

        while(bitsToTransmit != 10) {
            // Clear and start the timer
            NRF_TIMER1->TASKS_CLEAR = 1;
            NRF_TIMER1->CC[0] = (1000000/115200);
            NRF_TIMER1->EVENTS_COMPARE[0] = 0;
            NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Configure the timer to clear upon matching compare
            NRF_TIMER1->TASKS_START = 1;

            // Determine and output the next bit
            if (bitsToTransmit == 0) {
                // Start bit
                NRF_P0->OUTCLR = (1 << BBPIN1)|
                                 (1 << BBPIN2);
            } else if (bitsToTransmit >= 1 && bitsToTransmit <= 8) {
                // Character bit (set bit = OUTSET, unset bit = OUTCLR)
                uint8_t data_bit = (character >> (bitsToTransmit - 1)) & 0x01;
                if (data_bit) {
                    NRF_P0->OUTSET = (1 << BBPIN1)|
                                     (1 << BBPIN2);
                } else {
                    NRF_P0->OUTCLR = (1 << BBPIN1)|
                                     (1 << BBPIN2);
                }
            } else if (bitsToTransmit == 9) {
                // Stop bit
                NRF_P0->OUTSET = (1 << BBPIN1)|
                                 (1 << BBPIN2);
            }

            // Wait for the timer to complete
            while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);
            NRF_TIMER1->TASKS_STOP = 1;
            
            // We're 1 bit closer to the end of the character transmission
            bitsToTransmit++;
        }
    }
}

//
void voteForChocolate(void) {
    bitBangSerial(" ... Subtask 1 running ... \n");
    initTimer();

    while (1) {
        // Output 'Twix' to serial
        bitBangSerial("Twix\n");

        // Clear and start the timer
        NRF_TIMER1->TASKS_CLEAR = 1;
        NRF_TIMER1->CC[0] = (240 * 1000);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Configure the timer to clear upon matching compare
        NRF_TIMER1->TASKS_START = 1;
        // Wait for the timer to complete
        while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);
        // Stop and clear the timer
        NRF_TIMER1->TASKS_STOP = 1;
    };  // NB this function never returns
}

//###################################################################################
// Write your additional code for Subtask 2 here:

// I2C (TWI) address of the accelerometer (7-bit address)
#define ACCEL_ADDR        0x19

// Accelerometer register addresses
#define CTRL_REG1_A       0x20
#define OUT_X_L_A         0x28
#define OUT_Y_L_A         0x2A
#define OUT_Z_L_A         0x2C

// Initialise the accelerometer
void initAccel() {

}

//
int getAccelerometerSample(char axis) {
    static bool init = false;

    if (!init) {
        // Initialise the accelerometer
        initAccel();
        init = true;
    }
    
}

//
void showAccelerometerSamples(void) {
    bitBangSerial(" ... Subtask 2 running ... \n"); 
    char string[50];
    char buffer[25];

    while (1) {
        // int x = getAccelerometerSample('X');
        int y = getAccelerometerSample('Y');
        // int z = getAccelerometerSample('Z');

        int x = 0;
        // int y = 0;
        int z = 8;

        // Start constructing the message
        int index = 0;

        // Add "[X: "
        const char* prefixX = "[X: ";
        for (int i = 0; prefixX[i] != '\0'; i++) {
            string[index++] = prefixX[i];
        }

        // Convert x to a string and add it
        itoa(x, buffer); // Custom function to convert integer to string
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

        // /r/n
        string[index++] = '\r';
        string[index++] = '\n';

        // Null-terminate the string
        string[index] = '\0';

        // Output the string
        bitBangSerial(string);

        // Pause for 200ms
        initTimer();
        NRF_TIMER1->TASKS_CLEAR = 1;
        NRF_TIMER1->CC[0] = (200 * 1000);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; // Configure the timer to clear upon matching compare
        NRF_TIMER1->TASKS_START = 1;
        while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);
        NRF_TIMER1->TASKS_STOP = 1;

    };  // NB this function never returns
}

//###################################################################################
// Write your additional code for Subtask 3 here:

// TODO: NEED TO SORT OUT, base idea code is present
void makeNoise(void) {
    bitBangSerial(" ... Subtask 3 running ... \n"); 

    // Assign a PSEL channel to the Speaker pin/port
    NRF_PWM0->PSEL.OUT[1] = (MICROBIT_PIN_SPEAKER << PWM_PSEL_OUT_PIN_Pos) |
                            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

    // Enable PWM
    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM0-> COUNTERTOP = (16000 << PWM_COUNTERTOP_COUNTERTOP_Pos);

    // Use PWM to set value to pin.
    NRF_PWM0->SEQ[0].PTR = (16000 << PWM_SEQ_PTR_PTR_Pos); // initially 0
    NRF_PWM0->SEQ[0].CNT = 1; // 1 channel

    while (1) {
            // Use PWM to set value to pin.
            NRF_PWM0->SEQ[0].PTR = (16000 << PWM_SEQ_PTR_PTR_Pos); // initially 0
            NRF_PWM0->SEQ[0].CNT = 1; // 1 channel
            NRF_PWM0->TASKS_SEQSTART[0] = 1;
            NRF_PWM0->TASKS_STOP;
    }
}
