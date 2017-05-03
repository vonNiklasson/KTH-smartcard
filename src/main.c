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

bit card_exist(char * card_id);
bit card_id_is_valid(char * card_id);
char get_stored_card_access(char * card_id);
void add_card(char * card_id);
void save_card_info(char * card_id, char access_count);

void main(void) {
    /* Initialize some code */
    initialize();

    /* String to store text from card */
    char card_str[MAX_STRING];
    bit card_exist;
    char access_count;
    bit access = 0;

    /* Loop forever, program logic below */
    while (1) {
        /* Wait for card insertion */
        wait_for_card_insert();
        delay(150);
        string_out("Card inserted\r\n");

        overrun_recover();

        /* Get id from card, stored in card_str */
        string_in(&card_str[0]);

        /* Maybe make sure the id follows a 
         * certain format, all numbers or something */

        /* Check if card id exists since earlier */
        card_exist = card_exist(&card_str[0]);
        if (card_exist == 0) {
            /* Add the card id to the array and store a new value */
            add_card(&card_str[0]);
            save_card_info(&card_str[0], NEW_ACCESS_COUNT);
        }

        /* Get the number or tries the card has left */
        access_count = get_stored_card_value(&card_str[0]);
        if (access_count > 0) {
            /* If it's more than 0, decrease the number and give access */
            access_count--;
            access = 1;
        } else {
            /* If it's 0, give no access */
            access = 0;
        }

        /* If the user has access, turn on the led */
        if (access) {
            set_led(1);
            delay(10);

            save_card_info(&card_str[0], access_count);
        }

        /* Print the number of accesses the user have left */
        print_to_display(access_count);

        wait_for_card_withdraw();
        delay(10);
        set_led(0);
        string_out("Card widthdrawn\r\n");

        overrun_recover();
    }
}

bit card_exist(char * card_id) {
    return 0;
}

bit card_id_is_valid(char * card_id) {
    return 1;
}

char get_stored_card_access(char * card_id) {
    return 5;
}

void add_card(char * card_id) {
    /* Add the card to some array */
}

void save_card_info(char * card_id, char access_count) {
    /* Store the access_count to the corresponding card */
}
