/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16

/* Include extended functions */
#include "initialize.c"
#include "str.c"

void main(void) {
    /* Intialize some code */
    initialize();

    /* Loop forever, program logic below */
    while (1) {

    }
}

/* Stall program til card is inserted */
bit wait_for_card(void) {
    while (PORTC.3 == 0);
}
