/* main_key_template.c  question and answer, compare strings     */
/* This program is for 16F84 Gold Card                  */
/* All code in this file is from the smartkey.c template file                */

/*
SmartCard contact
             __ __ __
       +5V  |C1|   C5| Gnd
            |__|   __|
      MCLR  |C2|  |C6| 
            |__|  |__|
       OSC  |C3|  |C7| RB7/PGD I/O -><- Txd/Rxd half duplex 
            |__|  |__|
            |C4|  |C8|
            |__|__|__|

*/

/*
  SERIAL COMMUNICATION
  ============================
  One start bit, one stop bit, 8 data bit, no parity = 10 bit.
  Baudrate: 9600 baud => 104.167 usec. per bit.
  serial output PORTB.7  half duplex!
  serial input  PORTB.7  half duplex!
*/

#include "16F84.h"
#define MAX_STRING 16  /* string input max 15 characthers */
#pragma config |= 0x3ff1

/* Function prototypes                                   */
void initserial( void );
void putchar( char );
char getchar( void );
void string_out( const char * string ); 
void string_in( char * );
bit check_candidate( char * input_string, const char * candidate_string ); 
void delay( char );

void main( void)
{
   char i, c, d, charCount;
   char input_string[MAX_STRING]; /* 15 char buffer for input string */   
   bit compare;
   delay(50);  /* delay to stabilize power */

   initserial();

   string_in( &input_string[0]);
   
   delay(150);     /* give the lock time to get ready */
   
   string_out("1337666\r\n");  /* Change this to the ID of the card */

   while(1) nop(); /* end of communication */
}

/********************
     FUNCTIONS
     =========
*********************/


void initserial( void ) /* initialise serialcom port */
{
   PORTB.7 = 1;
   TRISB.7 = 1;  /* input mode */
}


void putchar( char d_out )  /* sends one char */
{
   char bitCount, ti;
   TRISB.7 = 0; /* output mode */
   PORTB.7 = 0; /* set startbit */
   for ( bitCount = 10; bitCount > 0 ; bitCount-- )
        {
         /* 104 usec at 3,58 MHz (5+27*3-1+9=104) */  
         // ti = 27; do ; while( --ti > 0);
         /* 104 usec at 4 MHz (5+30*3-1+1+9=104)  */         
          ti = 30; do ; while( --ti > 0); nop();   
          Carry = 1;           /* stopbit                        */
          d_out = rr( d_out ); /* Rotate Right through Carry     */
          PORTB.7 = Carry;
        }
        nop2(); nop2();
   return; /* all done */
}


char getchar( void )  /* recieves one char */
{
   /* One start bit, one stop bit, 8 data bit, no parity = 10 bit. */
   /* Baudrate: 9600 baud => 104.167 usec. per bit.                */
   TRISB.7 = 1; /* set input mode */
   char d_in, bitCount, ti;
   while( PORTB.7 == 1 )  /* wait for startbit */ ;
   /* delay 1,5 bit is 156 usec                         */ 
   /* 156 usec is 156 op @ 4 MHz ( 5+47*3-1+2+9=156)    */
   ti = 47; do ; while( --ti > 0); nop2();  
   for( bitCount = 8; bitCount > 0 ; bitCount--)
       {
        Carry = PORTB.7;
        d_in = rr( d_in);  /* rotate carry */
        /* delay 1 bit is 104 usec                       */ 
        /* 104 usec is 104 op @ 4 MHz (5+30*3-1+1+9=104) */  
        ti = 30; do ; while( --ti > 0); nop();  
        }
   return d_in;
}

void string_in( char * input_string )
{
   char charCount, c;
   for( charCount = 0; ; charCount++ )
       {
         c = getchar( );     /* input 1 character             */
         input_string[charCount] = c;  /* store the character */
         //putchar( c );     /* don't echo the character      */
         if( (charCount == (MAX_STRING-1))||(c=='\r' )) /* end of input   */
           {
             input_string[charCount] = '\0';  /* add "end of string"      */
             return;
           }
       }
}


void string_out(const char * string)
{
  char i, k;
  for(i = 0 ; ; i++)
   {
     k = string[i];
     if( k == '\0') return;   /* found end of string */
     putchar(k); 
   }
  return;
}


bit check_candidate( char * input_string, const char * candidate_string )
{
   /* compares input buffer with the candidate string */
   char i, c, d;
   for(i=0; ; i++)
     {
       c = input_string[i];
       d = candidate_string[i];
       if(d != c ) return 0;       /* no match    */
         if( d == '\0' ) return 1; /* exact match */
     }
}

void delay( char millisec)
/* 
  Delays a multiple of 1 milliseconds at 4 MHz
  using the TMR0 timer 
*/
{
    OPTION = 2;  /* prescaler divide by 8        */
    do  {
        TMR0 = 0;
        while ( TMR0 < 125)   /* 125 * 8 = 1000  */
            ;
    } while ( -- millisec > 0);
}

