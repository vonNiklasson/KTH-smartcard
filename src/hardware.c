void wait_for_card_insert(void);
void wait_for_card_withdraw(void);
void set_led(bit state);
void print_to_display(char val);
void delay(char millisec);

/* Stall program til card is inserted */
void wait_for_card_insert(void) {
    while (PORTC.3 == 0);
}

/* Stall program til card is withdrawn */
void wait_for_card_withdraw(void) {
    while (PORTC.3 == 1);
}

void set_led(bit state) {
    PORTC.2 = state;
    nop();
}

void print_to_display(char val) {
    /* Print hex-value to 7-segment display */
}

/*  Delays a multiple of 1 milliseconds at 4 MHz
    using the TMR0 timer by B. Knudsen */
void delay(char millisec) {
    OPTION = 2;  /* prescaler divide by 8        */
    do {
        TMR0 = 0;
        while (TMR0 < 125);   /* 125 * 8 = 1000  */
    } while (--millisec > 0);
}
