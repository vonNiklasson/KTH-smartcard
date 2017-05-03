/* This file contains string functions */

/* Prototypes */
void put_char(char d_out);
char get_char(void);
void string_in(char * string);
void string_out(const char * string);
bit compare_string(char * input_string, const char * candidate_string);

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
        c = getchar();          /* input 1 character         */
        string[charCount] = c;  /* store the character       */
        // putchar( c );        /* don't echo the character  */
        /* end of input */
        if((charCount == (MAX_STRING-1)) || (c=='\r' )) {
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
        putchar(k); 
    }
    return;
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
