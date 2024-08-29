#include <xc.h>
#include "configbit_header.h"
#define _XTAL_FREQ 4000000

// Seven-segment display output for digits 0-9
char output[10] = {
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000, // 9
    0b11000000  // 0
};

// Global variables to control stopwatch state
volatile unsigned char running = 0;   // Flag for stopwatch running state
volatile unsigned char reset_flag = 0; // Flag to reset the display

void __interrupt() ISR(void) {
    // INT0 interrupt (RB0) for Start
    if (INT0IF) {
        INT0IF = 0;       // Clear INT0 interrupt flag
        running = 1;      // Start the stopwatch
    }
    
    // INT1 interrupt (RB3) for Stop
    if (INT1IF) {
        INT1IF = 0;       // Clear INT1 interrupt flag
        running = 0;      // Stop the stopwatch
    }
    
    // INT2 interrupt (RB6) for Reset
    if (INT2IF) {
        INT2IF = 0;       // Clear INT2 interrupt flag
        running = 0;      // Stop the stopwatch
        reset_flag = 1;   // Set reset flag
    }
}

void timer() {
    for (int i = 0; i < 10; i++) {
        if (!running) {    // Stop if not running
            return;
        }
        __delay_ms(500);
        PORTD = output[i]; // Update 7-segment display
        
        // Reset the display if the reset flag is set
        if (reset_flag) {
            PORTD = output[9]; // Reset display to '0'
            reset_flag = 0;    // Clear reset flag
            return;
        }
    }    
}

void main(void) {
    // Port configuration
    TRISD = 0b00000000; // PORTD as output
    TRISB = 0b01001001; // PORTB as input for RB0, RB3, RB6
    
    PORTD = output[9];  // Initialize display to '0'
    
    // Interrupt configuration
    INTEDG0 = 1;  // Interrupt on rising edge for RB0 (Start)
    INTEDG1 = 1;  // Interrupt on rising edge for RB3 (Stop)
    INTEDG2 = 1;  // Interrupt on rising edge for RB6 (Reset)
    
    INT0IF = 0;   // Clear INT0 flag
    INT0IE = 1;   // Enable INT0 external interrupt (Start)
    
    INT1IF = 0;   // Clear INT1 flag
    INT1IE = 1;   // Enable INT1 external interrupt (Stop)
    
    INT2IF = 0;   // Clear INT2 flag
    INT2IE = 1;   // Enable INT2 external interrupt (Reset)
    
    PEIE = 1;     // Enable peripheral interrupts
    GIE = 1;      // Enable global interrupts
    
    while (1) {
        if (running) {
            timer();  // Start the timer if running
        }
        
        if (reset_flag) {
            PORTD = output[9]; // Reset display to '0'
            reset_flag = 0;    // Clear reset flag
        }
    }
    
    return;
}
