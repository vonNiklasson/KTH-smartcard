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

void reg_put_char(char data, char EEPROMadress);
char reg_get_char(char EEPROMadress);

void reg_put_word(const char * word, char reg_offset);
void reg_get_word(char * word, char reg_offset);

char get_card_offset_id(char * card_id);
bit card_id_is_valid(char * card_id);
char get_stored_card_access(char * card_id);
void add_card(char * card_id);
void save_card_info(char * card_id, char access_count);

void get_data_from_memory(void);
void set_data_to_memory(void);

/* Allocate space for 7 cards */
char memory_cards[8 * 7];
char memory_card_count = 0;

void main(void) {
    /* Initialize some code */
    initialize();

    /* String to store text from card */
    char card_str[8];
    char card_offset_id;
    char access_count;
    bit access = 0;

    const char * test_str1 = "test0000";
    const char * test_str2 = "test1111";
    reg_put_word(&test_str1[0], 0);
    reg_put_word(&test_str2[0], 1);
    char str[8];
    /* Loop forever, program logic below */
    while (1) {
        /* Wait for card insertion */
        wait_for_card_insert();
        delay(100); /* card debounce */
        delay(50);  /* extra delay   */

        string_out("Inserted\r\n");

        delay(100); /* USART is buffered, so wait until all chars sent  */

        overrun_recover();

        /* Get id from card, stored in card_str */
        //string_in(&card_str[0]);

        /* Maybe make sure the id follows a 
         * certain format, all numbers or something */

        /* Check if card id exists since earlier */
        //card_offset_id = get_card_offset_id(&card_str[0]);
        //if (card_offset_id == -1) {
        //    /* Add the card id to the array and store a new value */
        //    add_card(&card_str[0]);
        //    save_card_info(&card_str[0], NEW_ACCESS_COUNT);
        //}

        /* Get the number or tries the card has left */
        //access_count = get_stored_card_access(&card_str[0]);
        //if (access_count > 0) {
            /* If it's more than 0, decrease the number and give access */
        //    access_count--;
        //    access = 1;
        //} else {
        //    /* If it's 0, give no access */
        //    access = 0;
        //}

        /* If the user has access, turn on the led 
        if (access) {
            set_led(1);
            delay(10);

            save_card_info(&card_str[0], access_count);
        }
        */
        set_led(1);

        /* Print the number of accesses the user have left */
        //print_to_display(access_count);

        delay(100);
        wait_for_card_withdraw();
        delay(10);

        set_led(0);
        string_out("Widthdrawn\r\n");
        delay(100); /* USART is buffered, so wait until all chars sent  */

        /* Print test data 1 */
        reg_get_word(&str[0], 0);
        str[7] = 0;
        string_out(str);
        string_out("\r\n");

        /* Print test data 2 */
        reg_get_word(&str[0], 1);
        str[7] = 0;
        string_out(str);
        string_out("\r\n");

        delay(100);
    }
}

/* Check if the card exist in the array */
bit get_card_offset_id(char * card_id) {
    return 0;
}

/* Check if the card id _format_ is valid, 
   base it on length or something */
bit card_id_is_valid(char * card_id) {
    return 1;
}

/* Get the card access value from the array */
char get_stored_card_access(char * card_id) {
    return 5;
}

/* Add a new card id to the array */
void add_card(char * card_id) {

}

/* Store the access_count to the corresponding card */
void save_card_info(char * card_id, char access_count) {
    
}

void get_data_from_memory(void) {
    /* Get how many cards that are saved in the memory */
    memory_card_count = reg_get_char(0);
    /* Temporay string for card data */
    char card[8];
    /* Initialize temp vars */
    char i, j, k;
    /* Count for how many cards that are stored */
    for (i = 0; i < memory_card_count; i++) {
        /* Get card i from memory */
        reg_get_word(&card[0], i);
        /* Get start offset for card in local string */
        k = i * 8;
        /* Loop through the next 8 bytes in the local string */
        for (j = 0; j < 8 j++) {
            memory_cards[k + j] = card[j];
        }
    }
}

void set_data_to_memory(void) {

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
