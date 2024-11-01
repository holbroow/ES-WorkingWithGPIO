// Will Holbrook CW1 for 'SCC369: Embedded Systems' at Lancaster University (November 2024)

#include "MicroBit.h"

#define ROW1 MICROBIT_PIN_ROW1
#define ROW2 MICROBIT_PIN_ROW2
#define ROW3 MICROBIT_PIN_ROW3
#define ROW4 MICROBIT_PIN_ROW4
#define ROW5 MICROBIT_PIN_ROW5

#define COL1 MICROBIT_PIN_COL1
#define COL2 MICROBIT_PIN_COL2
#define COL3 MICROBIT_PIN_COL3
#define COL4 MICROBIT_PIN_COL4-32 // Subtract 32 because we dont require the bit that signifies the requirement for PORT 1
#define COL5 MICROBIT_PIN_COL5

#define BTNA MICROBIT_PIN_BUTTON_A
#define BTNB MICROBIT_PIN_BUTTON_B

#define REDLED MICROBIT_PIN_P1
#define GREENLED MICROBIT_PIN_P9
#define BLUELED MICROBIT_PIN_P8

extern NRF52Serial serial;   // Serial object may be useful for debugging


//###################################################################################
// Write your code for Subtask 1 here (and leave these separators in your code!):

void wait_ms(uint8_t ms) {
    for (volatile int i = 0; i < 7200 * ms; i++);
}

void wait_us(uint8_t us) {
    for (volatile int i = 0; i < 7.2 * us; i++);
}

//
void displayBinary(uint8_t value) {
    
    static int init = 1;

    if(init){
        // set all required pins to Output and initialise ROW1 for use (We do this once)
        NRF_P0->DIRSET = (GPIO_DIR_PIN21_Output << ROW1) |
                    (GPIO_DIR_PIN28_Output << COL1) |
                    (GPIO_DIR_PIN11_Output << COL2) |
                    (GPIO_DIR_PIN31_Output << COL3) |
                    (GPIO_DIR_PIN30_Output << COL5);
        NRF_P1->DIRSET = (GPIO_DIR_PIN5_Output << COL4);
        serial.printf("INIT!");
        init = 0;
    }

    // Set the Row to High for potential difference, setting colunmns to Low to light them up if their corresponding bit is 1 in the value parameter
    NRF_P0->OUTSET = (GPIO_OUT_PIN21_High << ROW1);

    if (value & (1 << 4)) {
        NRF_P0->OUTCLR = 1 << COL1;
        serial.printf("1");
    } else {
        NRF_P0->OUTSET = 1 << COL1;
        serial.printf("0");
    }
    if (value & (1 << 3)) {
        NRF_P0->OUTCLR = 1 << COL2;
        serial.printf("1");
    } else {
        NRF_P0->OUTSET = 1 << COL2;
        serial.printf("0");
    }
    if (value & (1 << 2)) {
        NRF_P0->OUTCLR = 1 << COL3;
        serial.printf("1");
    } else {
        NRF_P0->OUTSET = 1 << COL3;
        serial.printf("0");
    }
    if (value & (1 << 1)) {
        NRF_P1->OUTCLR = 1 << COL4;
        serial.printf("1");
    } else {
        NRF_P1->OUTSET = 1 << COL4;
        serial.printf("0");
    }
    if (value & (1 << 0)) {
        NRF_P0->OUTCLR = 1 << COL5;
        serial.printf("1\n");
    } else {
        NRF_P0->OUTSET = 1 << COL5;
        serial.printf("0\n");
    }
}

// 
void countUpBinary(uint8_t initialValue) {
    serial.printf(" ... Subtask 1 running ... \n");

    // Copy the parameter 'initialValue' and display it while incrementing it indefinitely, it wraps around at 11111
    uint8_t value = initialValue;
    while (1) {
        displayBinary(value);
        for (volatile int i = 0; i < 1414500; i++);
        value = (value + 1)%32;
    } // NB this function never returns
}

//###################################################################################
// Write your additional code for Subtask 2 here:

//
void countWithButtonsBinary(uint8_t initialValue) {
    serial.printf(" ... Subtask 2 running ... \n"); 
    uint8_t value = initialValue;

    // Set buttons A and B as inputs
    NRF_P0->DIR = (GPIO_DIR_PIN14_Input << BTNA) |
                  (GPIO_DIR_PIN23_Input << BTNB);

    // Set the pin configuration for both pin 14 (Button A) and pin32 (Button B)
    NRF_GPIO->PIN_CNF[BTNA] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                               (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                               (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
    
    NRF_GPIO->PIN_CNF[BTNB] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                               (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                               (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);

    // display the initial value (00000)
    displayBinary(value);

    while (1) {
        // check if Button A is pressed
        if (!(NRF_P0->IN & (1 << BTNA))) { // active low check
            serial.printf("Button A pressed\n");
            value--;  // decrement value
            displayBinary(value); //display value
            while (!(NRF_P0->IN & (1 << BTNA))); // wait for button release
            for (volatile int i = 0; i < 10000; i++); // delay for debounce
        }

        // check if Button B is pressed
        if (!(NRF_P0->IN & (1 << BTNB))) { // active low check
            serial.printf("Button B pressed\n");
            value++;  // increment value
            displayBinary(value); // display value
            while (!(NRF_P0->IN & (1 << BTNB))); // wait for button release
            for (volatile int i = 0; i < 10000; i++); // delay for debounce
        }
    }
}

//###################################################################################
// Write your additional code for Subtask 3 here:

//
uint8_t sampleVoltage() {
    // Set the durection to input for Pin 2
    NRF_P0->DIRCLR = (1 << MICROBIT_PIN_P2);

    // Configure said pin
    NRF_GPIO->PIN_CNF[MICROBIT_PIN_P2] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                         (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                         (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
    
    // Assign channel 0
    NRF_PWM0->PSEL.OUT[0] = (MICROBIT_PIN_P2);

    // Configure channel 0 for ADC (SAADC)
    NRF_SAADC->CH[0].CONFIG = (SAADC_RESOLUTION_VAL_8bit << SAADC_CH_CONFIG_RESN_Pos) |     // 8-bit resolution
                              (SAADC_CH_PSELN_PSELN_AnalogInput0 << SAADC_CH_PSELN_PSELN_Pos) | // P0.02 as positive input
                              (SAADC_CH_CONFIG_GAIN_Gain1_4 << SAADC_CH_CONFIG_GAIN_Pos); // Gain of 1/4
                              (SAADC_CH_CONFIG_REFSEL_VDD1_4 << SAADC_CH_CONFIG_REFSEL_Pos) | // Reference voltage Vdd/4
                              (SAADC_CH_CONFIG_RESN_Pulldown << SAADC_CH_PSELN_PSELN_Pos); // Set a pulldown on negative channel
    NRF_SAADC->CH[0].PSELP = (SAADC_CH_PSELP_PSELP_AnalogInput0 << SAADC_CH_PSELP_PSELP_Pos); // Connect positive channel
    NRF_SAADC->CH[0].PSELN = (SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos); // Don't connect negative, but place a pulldown
    NRF_SAADC->RESULT.MAXCNT = (1 << SAADC_RESULT_MAXCNT_MAXCNT_Pos); // Max cnt of 1
    NRF_SAADC->SAMPLERATE = (2047 << SAADC_SAMPLERATE_CC_Pos); // Sample rate of 2047
    NRF_SAADC->RESOLUTION = (SAADC_RESOLUTION_VAL_8bit << SAADC_RESOLUTION_VAL_Pos); // Set resolution to 8 bit

    static volatile int16_t AdcResult; // ADC result

    NRF_SAADC->RESULT.PTR = (uint32_t) &AdcResult; // Point to where the result is to be stored
    NRF_SAADC->ENABLE = 1; // Enable the ADC

    NRF_SAADC->TASKS_START = 1; // Start the ADC
    NRF_SAADC->TASKS_SAMPLE = 1; // Take one sample

    if (NRF_SAADC->STATUS == 0UL) {
        if (AdcResult < 1 || AdcResult > 255) { // This exists because my variable resistor, when set fully anti-clockwise, returns -1 from ADC not 0
            serial.printf("%d\n", 0);
            return 0; // Return 0 if AdcResult is out of bounds
        } else {
            serial.printf("%d\n", AdcResult);
            return AdcResult; // Otherwise, return the result normally
        }
        NRF_SAADC->TASKS_STOP = 1;
    }
}

//
void displayVoltageBinary() {
    serial.printf(" ... Subtask 3 running ... \n");
    uint16_t voltage;
    uint8_t top5BitsVoltage;

    while (1) {
        voltage = sampleVoltage(); // Take voltage sample with ADC
        top5BitsVoltage = voltage >> 3; // Do conversion to 5 most significant bits

        displayBinary(top5BitsVoltage); // Display the voltage value

        for (volatile int i = 0; i < 10000; i++);
    };  // NB this function never returns
}

//###################################################################################
// Write your additional code for Subtask 4 here:

// 
void driveRGB() {
    serial.printf(" ... Subtask 4 running ... \n"); 
    static uint16_t values[3]; // Create values array for Red Green and Blue values

    // Assign each PSEL channel to each LED pin
    NRF_PWM0->PSEL.OUT[1] = (REDLED << PWM_PSEL_OUT_PIN_Pos) |
                            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[2] = (GREENLED << PWM_PSEL_OUT_PIN_Pos) |
                            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[3] = (BLUELED << PWM_PSEL_OUT_PIN_Pos) |
                            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

    // Enable PWM
    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);

    NRF_PWM0-> COUNTERTOP = (16000 << PWM_COUNTERTOP_COUNTERTOP_Pos);

    // Initially LED 'values' are 0
    values[0] = 0; // Red
    values[1] = 0; // Green
    values[2] = 0; // Blue

    // Create i for use in both For loops
    uint16_t i;

    // Use PWM to set values to pins/channels.
    NRF_PWM0->SEQ[0].PTR = ((uint32_t)values << PWM_SEQ_PTR_PTR_Pos); // initially 0
    NRF_PWM0->SEQ[0].CNT = 3; // 3 channels

    while (1) {
        // Fade in gradually from 0 - 16000
        for (i = 0; i <= 16000; i += 20) {
            serial.printf("Fade in: %d\n", i);
        
            values[0] = i;
            values[1] = i;
            values[2] = i;

            NRF_PWM0->SEQ[0].PTR = ((uint32_t)values << PWM_SEQ_PTR_PTR_Pos);
            NRF_PWM0->TASKS_SEQSTART[0] = 1;
            NRF_PWM0->TASKS_STOP;

            //Delay
            for (volatile int j = 0; j < 5000; j++);
        }

        // Fade out gradually from 16000 - 0
        for (i = 16000; i > 0; i -= 20) {
            serial.printf("Fade out: %d\n", i);
        
            values[0] = i;
            values[1] = i;
            values[2] = i;

            NRF_PWM0->SEQ[0].PTR = ((uint32_t)values << PWM_SEQ_PTR_PTR_Pos);
            NRF_PWM0->TASKS_SEQSTART[0] = 1;
            NRF_PWM0->TASKS_STOP;

            // Delay
            for (volatile int j = 0; j < 5000; j++);
        }
    }
};  // NB this function never returns


//###################################################################################
// Write your additional code for Subtask 5 (stretch goal) here:

// 
void countWithTouchesBinary(uint8_t initialValue) {
    serial.printf(" ... Subtask 5 running ... \n"); 
    while (1) {
    };  // NB this function never returns
}