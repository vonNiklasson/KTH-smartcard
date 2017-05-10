/* This file contains basic and initialize functions */

/* Prototypes */
void initialize(void);
void overrun_recover(void);

void initialize(void) {
    TRISA.0 = 1; /* RA0 not to disturb PK2 UART Tool */
    ANSEL.0 = 0; /* RA0 digital input */
    TRISA.1 = 1; /* RA1 not to disturb PK2 UART Tool */
    ANSEL.1 = 0; /* RA1 digital input */

    /* Initialize PIC16F690 serialcom port */
    /* One start bit, one stop bit, 8 data bit, no parity. 9600 Baud. */

    TXEN = 1;      /* transmit enable                   */
    SYNC = 0;      /* asynchronous operation            */
    TX9  = 0;      /* 8 bit transmission                */
    SPEN = 1;

    BRGH  = 0;     /* settings for 6800 Baud            */
    BRG16 = 1;     /* @ 4 MHz-clock frequency           */
    SPBRG = 25;

    CREN = 1;      /* Continuous receive                */
    RX9  = 0;      /* 8 bit reception                   */
    ANSELH.3 = 0;  /* RB5 digital input for serial_in   */

    /* More init */
    TRISC.3 = 1;  /* RC3 card contact is input       */
    ANSEL.7 = 0;  /* RC3 digital input               */
    TRISC.2 = 0;  /* RC2 lock (lightdiode) is output */
    PORTC.2 = 0;  /* RC2 initially locked            */

    memory_cards[0] = 0;
    memory_card_count = 0;

    get_data_from_memory();
}

void overrun_recover(void) {
    char trash;
    trash = RCREG;
    trash = RCREG;
    CREN = 0;
    CREN = 1;
}

