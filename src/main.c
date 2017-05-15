/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16
#define NEW_ACCESS_COUNT 5

/* Allocate space for 7 cards */
char memory_cards[8 * 2];
char memory_card_count;

char create_card(char * card_id);

void get_data_from_memory(void);
void set_data_to_memory(void);

char get_card_offset(char * card_id);
char get_card_accesses(char card_offset);
void set_card_accesses(char card_offset, const char accesses);
char increase_card_accesses(char card_offset);
char decrease_card_accesses(char card_offset);

void reg_put_char(char data, char EEPROMadress);
char reg_get_char(char EEPROMadress);

void reg_put_word(const char * word, char reg_offset);
void reg_get_word(char * word, char reg_offset);

/*String related functions*/
void put_char(char d_out);
char get_char(void);
void string_in(char * string);
void string_out(const char * string);
void printf(const char *string, char variable);
bit compare_string(char * input_string, const char * candidate_string);

/*Hardware related functions*/
void wait_for_card_insert(void);
void wait_for_card_withdraw(void);
bit get_button_state(void);
void set_led_red(bit state);
void set_led_green(bit state);
void print_to_display(char val);
void delay(char millisec);

/*Initializing and registry clearing*/
void initialize(void);
void overrun_recover(void);

void main(void) {
	/* String to store text from card */
    char card_str[MAX_STRING];
    char card_offset;
    char card_access_count;
    bit has_access = 0;
	bit test1 = 0;
	bit test2 = 0;
    /* Initialize some code */
    initialize();

    /* Extended initialize */
    memory_cards[0] = 0;
    memory_card_count = 0;

    /* Loop forever, program logic below */
    while (1) {
        /* Reset the display */
        print_to_display(-1);
		get_data_from_memory();
        /* Wait for card insertion */
        while (PORTC.3 == 0);
        delay(100); /* card debounce */
        delay(50);  /* extra delay   */
		char test = get_card_accesses(0);
		/* ask the question */
		printf("Card accesses: %d\r\n", PORTC.1);
		
        delay(100); /* USART is buffered, so wait until all chars sent  */

        /* empty the reciever FIFO, it's now full with garbage */
		overrun_recover();

        /* Get id from card, stored in card_str */
        string_in(&card_str[0]);
        
        test1 = compare_string(&card_str[0], "joherik");
		test2 = compare_string(&card_str[0], "1337666");
		
        if (test1 == 1 || test2 == 1) {
            //set_led_red(1);
        }
		
        /* Get the card offset id (if it exists) */
        card_offset = get_card_offset(&card_str[0]);

        if (card_offset == -1) {
            //Add new card 
            card_offset = create_card(&card_str[0]);
        }
		
        card_access_count = get_card_accesses(card_offset);
        nop();

		//Check if any accesses are left on the card and makes you refill
        if (card_access_count > 0) {
            set_led_green(1);
            nop();
            // Decrease the number of accesses
            card_access_count = decrease_card_accesses(card_offset);
        }
		else if (card_access_count == 0) {
			set_led_red(1);
			nop();
		}

        print_to_display(card_access_count);

        //While card is inserted loop the 
        // ability to increase the number of access
        while (PORTC.3 == 1) {
            //Wait for button presses and add 1 access
            while (!get_button_state() && PORTC.3 == 1);
            /* If the button is pressed, increase the access count */
            if (get_button_state()) {
                card_access_count = increase_card_accesses(card_offset);
                nop();
                print_to_display(card_access_count);
            }
            /* Wait for debounce of the button */
            while (get_button_state() && PORTC.3 == 1);
        }

        delay(10);

        /* Reset the LED:s */
        set_led_red(0);
		set_led_green(0);
        delay(100);   /* card debounce */

        /* Write the new information to the memory */
		set_data_to_memory();
    }
}

/********************
     FUNCTIONS
     =========
*********************/

char get_card_offset(char * card_id) {
    int i, j, k;
    char tmpChar1, tmpChar2;
    /* Loops through the number of cards */
    for (i = 0; i < memory_card_count; i++) {
        /* Sets j to the card offset */
        j = i * 8;
        /* Iterate the chars in the card strings */
        for (k = 0; k < 7; k++) {
            /* Check if the string matches */
            tmpChar1 = memory_cards[j + k];
            tmpChar2 = card_id[k];
            if (tmpChar1 != tmpChar2) {
                break;
            }
        }
        /* If all chars matched, return the offset id */
        if (k == 7) {
            return i;
        }
    }

    /* Otherwise, return -1 */
    return -1;
}

//Get the number of accesses of a specific card 
char get_card_accesses(char card_offset) {
    char current_accesses = memory_cards[(card_offset * 8) + 7];
    return current_accesses;
}

//Get the number of accesses of a specific card 
void set_card_accesses(char card_offset, const char accesses) {
    memory_cards[(card_offset * 8) + 7] = accesses;
}

//Increase the number of accesses of a specific card by one 
char increase_card_accesses(char card_offset) {
    char current_access = get_card_accesses(card_offset);

    if (current_access < 9) { 
        current_access++; 
    }

	set_card_accesses(card_offset, current_access);
	return current_access;
}

//Decrease the number of accesses of a specific card by one 
char decrease_card_accesses(char card_offset) {
	char temp_offset = card_offset;
	memory_cards[(temp_offset * 8) + 7] = memory_cards[(temp_offset * 8) + 7] - 1;
	return memory_cards[(temp_offset * 8) + 7];
}

char create_card(char * card_id) {
    /* Get the new card offset */
    char card_offset = memory_card_count;
    /* Add 1 to the memory card count */
    memory_card_count = memory_card_count + 1;

    int i;
    char temp_char;
    for (i = 0; i < 7; i++) {
        temp_char = card_id[i];
        memory_cards[(8 * card_offset) + i] = temp_char;
    }
    /* Set the last byte to 0       (8 * card_offset) + i + 1   */
    memory_cards[(8 * card_offset) + i] = 0;

    /* Return the new card offset */
    return card_offset;
}

void get_data_from_memory(void) {
    /* Get how many cards that are saved in the memory */
    memory_card_count = reg_get_char(0);
    /* Temporay string for card data */
    char card[8];
    /* Initialize temp vars */
    int i, j, k;
    char temp_char;
    /* Count for how many cards that are stored */
    for (i = 0; i < memory_card_count; i++) {
        /* Get card i from memory */
        reg_get_word(&card[0], i);
        /* Get start offset for card in local string */
        k = i * 8;
        /* Loop through the next 8 bytes in the local string */
        for (j = 0; j < 8; j++) {
            temp_char = card[j];
            memory_cards[k + j] = temp_char;
        }
    }
}

void set_data_to_memory(void) {
    /* Store the number of saved cards */
    reg_put_char(memory_card_count, 0);
    /* Initialize temp vars */
    char i;
    /* Count for how many cards that are stored */
    for (i = 0; i < memory_card_count; i++) {
        reg_put_word(&memory_cards[0], i);
    }
}



void reg_put_word(const char * word, char reg_offset) {
    int offset = (reg_offset * 8) + 1;

    char c;
    int i;
    for (i = 0; i < 8; i++) {
        c = word[offset - 1 + i];
        reg_put_char(c, offset + i);
    }
}

void reg_get_word(char * word, char reg_offset) {
    int offset = (reg_offset * 8) + 1;

    char c;
    int i;
    for (i = 0; i < 8; i++) {
        c = reg_get_char(offset + i);
		word[i] = c;
    }
}

/* I got this online */
void reg_put_char(char data, char EEPROMadress) {
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
    /* End of write EEPROM-data sequence                     */
}

/* I got this online */
char reg_get_char(char EEPROMadress) {
    /* Get char from specific EEPROM-adress */
    /* Start of read EEPROM-data sequence                */
    char temp;
    EEADR = EEPROMadress;  /* EEPROM-data adress 0x00 => 0x40  */
    EEPGD = 0;
    RD = 1;                /* Read                             */
    temp = EEDATA;
    RD = 0;
    return temp;          /* data to be read                   */
    /* End of read EEPROM-data sequence                        */
}

/* Sends one char */
void put_char(char d_out) {
    while (!TXIF);  /* wait until previus character transmitted   */
    TXREG = d_out;
    return; /* done */
}

/* Recieves one char */
char get_char(void) {
    char d_in = '\r';
    while (!RCIF && PORTC.3);  /* wait for character or card removal */
    if(!RCIF) return d_in;
    d_in = RCREG;
    return d_in;
}

void string_in(char * string) {
    char charCount, c;
    for(charCount = 0; ; charCount++) {
        c = get_char();         /* input 1 character         */
        string[charCount] = c;  /* store the character       */
        // put_char( c );       /* don't echo the character  */
        /* end of input */
        if((charCount == (MAX_STRING-1)) || (c=='\r')) {
            string[charCount] = '\0'; /* add "end of string" */
            return;
        }
    }
}

void string_out(const char * string) {
    char i, k;
    for(i = 0 ; ; i++) {
        k = string[i];
        if( k == '\0') return;   /* found end of string */
        put_char(k);
    }
    return;
}

void printf(const char *string, char variable)
{
  char i, k, m, a, b;
  for(i = 0 ; ; i++)
   {
     k = string[i];
     if( k == '\0') break;   // at end of string
     if( k == '%')           // insert variable in string
      { 
        i++;
        k = string[i];
        switch(k)
         {
           case 'd':         // %d  signed 8bit
             if( variable.7 ==1) put_char('-');
             else put_char(' ');
             if( variable > 127) variable = -variable;  // no break!
           case 'b':         // %b BINARY 8bit
             for( m = 0 ; m < 8 ; m++ )
              {
                if (variable.7 == 1) put_char('1');
                else put_char('0');
                variable = rl(variable);
               }
              break;
           case 'c':         // %c  'char'
             put_char(variable); 
             break;
           case '%':
             put_char('%');
             break;
           default:          // not implemented 
             put_char('!');   
         }   
      }
      else put_char(k); 
   }
}


bit compare_string(char * input_string, const char * candidate_string) {
    /* compares input with the candidate string */
    char i, c, d;
    for(i=0; ; i++) {
        c = input_string[i];
        d = candidate_string[i];
        if(d != c )     return 0;   /* no match    */
        if(d == '\0')   return 1;   /* exact match */
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

bit get_button_state(void) {
	bit input;
	while(1) {
		input = PORTC.1;
		delay(10);
		if (input == PORTC.1) {
			return input;
		}
	}
}

void set_led_red(bit state) {
    PORTA.2 = state;
    nop();
}

void set_led_green(bit state) {
    PORTB.4 = state;
    nop();
}

void print_to_display(char val) {
    nop();
    /* Print hex-value to 7-segment display */
	char value = val;
	char i;
	if (value == -1) {
		PORTC.4 = 0;
        nop();
	}
	else if (value >= 0 && value < 10) {
		PORTC.4 = 1;
        nop();
		delay(1);
		PORTC.7 = 1;
        nop();
		delay(1);
		PORTC.7 = 0;
        nop();
		delay(1);
		for (i = 0; i < value; i++) {
			PORTC.6 = 1;
            nop();
			delay(1);
			PORTC.6 = 0;
            nop();
			delay(1);
		}
	}
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
    TRISA.2 = 0;  /* RC2 Red LED for no access */
    PORTA.2 = 0;  /* RC2 initially off            */
	TRISB.4 = 0;  /* RB4 Green LED for access */
	PORTB.4 = 0;  /* RB4 initially off            */
	TRISC.1 = 1;  /* RC1 Button is set to input */
	ANSEL.5 = 0;  /* RC1 digital input               */
	
	//Display initialize
	TRISC.7 = 0;  /* RC7 Output to reset pin on display */
	PORTC.7 = 0;  /* RC7 initially off */
	TRISC.6 = 0;  /* RC6 Output to display clock */
	PORTC.6 = 0;  /* RC6 initially off */
	TRISC.4 = 0;  /* RC4 Enable display */
	PORTC.4 = 1;  /* RC4 on */
}

void overrun_recover(void) {
    char trash;
    trash = RCREG;
    trash = RCREG;
    CREN = 0;
    CREN = 1;
}