/* This file contains the main 
 * function and the program logic */

/* First include of chipkit-header */
#include "16F690.h"

#pragma config |= 0x00D4
#define MAX_STRING 16
#define NEW_ACCESS_COUNT 5

/* Include extended functions */
#include "initialize.c"
#include "str.c"
#include "hardware.c"

/* Allocate space for 7 cards */
char memory_cards[8 * 2];
char memory_card_count;

char create_card(char * card_id);

void get_data_from_memory(void);
void set_data_to_memory(void);

char get_card_offset(char * card_id);

void reg_put_char(char data, char EEPROMadress);
char reg_get_char(char EEPROMadress);

void reg_put_word(const char * word, char reg_offset);
void reg_get_word(char * word, char reg_offset);

void main(void) {
    /* Initialize some code */
    initialize();

    /* Extended initialize */
    memory_cards[0] = 0;
    memory_card_count = 0;
    get_data_from_memory();

    /* String to store text from card */
    char card_str[8];
    char card_offset;
    char card_access_count;
    bit has_access = 0;

    /* Loop forever, program logic below */
    while (1) {
        /* Reset the display */
        print_to_display(0);

        /* Wait for card insertion */
        wait_for_card_insert();
        delay(100); /* card debounce */
        delay(50);  /* extra delay   */

        string_out("Inserted\r\n");

        delay(100); /* USART is buffered, so wait until all chars sent  */

        overrun_recover();

        /* Get id from card, stored in card_str */
        string_in(&card_str[0]);

        delay(100); /* Delay, because why not */

        bit test = compare_string(&card_str[0], "joherik");

        if (test) {
            set_led(1);
        }

        /* Get the card offset id (if it exists) */
        card_offset = get_card_offset(&card_str[0]);

        if (card_offset == -1) {
            /* Add new card */
            card_offset = create_card(&card_str[0]);
        }


        /* Print the number of accesses the user have left */
        print_to_display(card_access_count);

        delay(100);
        wait_for_card_withdraw();
        delay(10);

        set_led(0);
        string_out("Widthdrawn\r\n");
        delay(100); /* USART is buffered, so wait until all chars sent  */

        overrun_recover();

        delay(100);
    }
}

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

char create_card(char * card_id) {
    /* Get the new card offset */
    char card_offset = memory_card_count;
    /* Add 1 to the memory card count */
    memory_card_count = memory_card_count + 1;

    int i;
    char temp_char;
    for (i = 0; i < 7; i++) {
        temp_char = card_id[i];
        memory_cards[card_offset + i] = temp_char;
    }
    /* Set the last byte to 0 */
    memory_cards[card_offset + i + 1] = 0;

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
        reg_put_word(&memory_cards[i], i);
    }
}



void reg_put_word(const char * word, char reg_offset) {
    int offset = (reg_offset * 8) + 1;

    char c;
    int i;
    for (i = 0; i < 8; i++) {
        c = word[i];
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
