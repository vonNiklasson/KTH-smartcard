#include "16F690.h"
#include "initialize.c"
#include "str.c"

#pragma config |= 0x00D4
#define MAX_STRING 16

void main(void) {
    initialize();
}

