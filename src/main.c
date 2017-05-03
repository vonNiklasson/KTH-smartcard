/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16

/* Include extended functions */
#include "initialize.c"
#include "str.c"


void wait_for_card_insert(void);
void wait_for_card_withdraw(void);
void delay(char millisec);
void putchar_eedata( char data, char EEPROMadress );
char getchar_eedata( char EEPROMadress );

void main(void) {
    /* Intialize some code */
    initialize();
    PORTC.2 = 1;
    delay(100);
	putchar_eedata(0x41, 0x10);
    /* Loop forever, program logic below */
    while (1) {
		char cd;
        /* Wait for card insertion */
        wait_for_card_insert();
        delay(150);

        /* wtf is this? */
        overrun_recover();
        /* Light up this shit */
        PORTC.2 = 1;
        delay(100);
		
		cd = getchar_eedata(0x10);
		cd = cd + 1;
		putchar_eedata(cd, 0x10); 
		delay(1000);
		put_char(cd);
		string_out("\r\n");
		cd = 0;

        wait_for_card_withdraw();
        delay(10);

        /* Turn off LED */
        PORTC.2 = 0;
        delay(100);
    }
}

/* Stall program til card is inserted */
void wait_for_card_insert(void) {
    while (PORTC.3 == 0);
}

/* Stall program til card is withdrawn */
void wait_for_card_withdraw(void) {
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

void putchar_eedata( char data, char EEPROMadress )
{
/* Put char in specific EEPROM-adress */
      /* Write EEPROM-data sequence                          */
      EEADR = EEPROMadress; /* EEPROM-data adress 0x00 => 0x40 */ 
      EEDATA = data;        /* data to be written              */
      WREN = 1;             /* write enable                    */
      EECON2 = 0x55;        /* first Byte in comandsequence    */
      EECON2 = 0xAA;        /* second Byte in comandsequence   */
      WR = 1;               /* write                           */
      while( EEIF == 0) ;   /* wait for done (EEIF=1)          */
      WR = 0;
      WREN = 0;             /* write disable - safety first    */
      EEIF = 0;             /* Reset EEIF bit in software      */
	  string_out("written\r\n");
	  delay(1000);
      /* End of write EEPROM-data sequence                     */
}

char getchar_eedata( char EEPROMadress )
{
/* Get char from specific EEPROM-adress */
      /* Start of read EEPROM-data sequence                */
      char temp;
      EEADR = EEPROMadress;  /* EEPROM-data adress 0x00 => 0x40  */
	  EEPGD = 0;
      RD = 1;                /* Read                             */
      temp = EEDATA;
      RD = 0;
	  string_out("read\r\n");
      return temp;          /* data to be read                   */
      /* End of read EEPROM-data sequence                        */  
} 