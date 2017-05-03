/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16

/* Include extended functions */
#include "initialize.c"
#include "str.c"
#include "hardware.c"

void main(void) {
    /* Intialize some code */
    initialize();

    /* Loop forever, program logic below */
    while (1) {
        /* Wait for card insertion */
        wait_for_card_insert();
        delay(150);

        /* wtf is this? */
        overrun_recover();

        /* Light up this shit */
        set_led(1);
        delay(100);

        wait_for_card_withdraw();
        delay(10);

        /* Turn off LED */
        set_led(0);
        delay(100);
    }
}