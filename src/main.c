/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16

/* Include extended functions */
#include "initialize.c"
#include "str.c"

bit wait_for_card_insert(void);
bit wait_for_card_withdraw(void);
void delay(char millisec);

void main(void) {
    /* Intialize some code */
    initialize();
    PORTC.2 = 1;
    delay(100);

    /* Loop forever, program logic below */
    while (1) {
        /* Wait for card insertion */
        wait_for_card_insert();
        delay(150);

        /* wtf is this? */
        overrun_recover();

        /* Light up this shit */
        PORTC.2 = 1;
        delay(100);

        wait_for_card_withdraw();
        delay(10);

        /* Turn off LED */
        PORTC.2 = 0;
        delay(100);
    }
}

/* Stall program til card is inserted */
bit wait_for_card_insert(void) {
    while (PORTC.3 == 0);
}

/* Stall program til card is withdrawn */
bit wait_for_card_withdraw(void) {
    while (PORTC.3 == 1);
}

/*  Delays a multiple of 1 milliseconds at 4 MHz
    using the TMR0 timer by B. Knudsen
*/

void delay(char millisec) {
    OPTION = 2;  /* prescaler divide by 8        */
    do {
        TMR0 = 0;
        while (TMR0 < 125);   /* 125 * 8 = 1000  */
    } while (--millisec > 0);
}