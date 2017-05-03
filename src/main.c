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
    initialize();
}

