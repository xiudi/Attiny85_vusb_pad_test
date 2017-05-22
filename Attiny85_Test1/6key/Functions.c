
#include "Functions.h"
#include <string.h>
#include <avr/pgmspace.h>
#ifdef SIXKEY
int setup(){
	pinMode(0,INPUT);digitalWrite(0,HIGH);
	pinMode(1,OUTPUT);digitalWrite(1,LOW);
	pinMode(2,INPUT);digitalWrite(2,HIGH);
	usb_init();
	uint8_t sign0=0;
	while (1)
	{
		if(digitalRead(0)==LOW)
		{
			if(sign0==0)keyPrintWord2();
			sign0=0x20;
		}
		else{
			keyboard_modifier_keys=0;
			memset(keyboard_keys, 0, 6);
		}
		if(sign0>0)sign0-=1;
		uint8_t i=0;
		for ( i=0; i<3; i++){
			if((keyboard_leds&(1<<i))==(1<<i))
			{ digitalWrite(1,HIGH);break;}
			else{ digitalWrite(1,LOW);}
		}
		usb_keyboard_send();
	}
	return 0;

}

uint8_t releasekey(uint8_t key)
{uint8_t i;
	uint8_t send_required=0;
	for ( i=0; i < 6; i++) {
		if (keyboard_keys[i] == key) {
			keyboard_keys[i] = 0;
			send_required=1;
			break;
		}
	}
	return send_required;
}
void releaseAll()
{uint8_t i;
	for ( i=0; i < 6; i++) {
		keyboard_keys[i] = 0;
	}
	keyboard_modifier_keys=0;
}
uint8_t presskey(uint8_t key)
{uint8_t i;
	for ( i=0; i < 6; i++) {
		if (keyboard_keys[i] == key) {
			return 1;
		}
	}
	for ( i=0; i < 6; i++) {
		if (keyboard_keys[i] == 0) {
			keyboard_keys[i] = key;
			return 1;
		}
	}
	return 0;
}
void pressModifierKeys(uint8_t key)
{
	keyboard_modifier_keys|=key;
}
void releaseModifierKeys(uint8_t key)
{
	keyboard_modifier_keys&=~key;
}
#if defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
void pinMode(uint8_t IO,uint8_t value){
	switch(IO){
		case 0: if(value){DDRB|= (1<<0);}else{DDRB &= ~(1<<0);}break;
		case 1: if(value){DDRB|= (1<<1);}else{DDRB &= ~(1<<1);}break;
		case 2: if(value){DDRB|= (1<<2);}else{DDRB &= ~(1<<2);}break;
		case 3: if(value){DDRB|= (1<<3);}else{DDRB &= ~(1<<3);}break;
		case 4: if(value){DDRB|= (1<<4);}else{DDRB &= ~(1<<4);}break;
		case 5: if(value){DDRB|= (1<<5);}else{DDRB &= ~(1<<5);}break;
		case 6: if(value){DDRB|= (1<<6);}else{DDRB &= ~(1<<6);}break;
	}
}
void digitalWrite(uint8_t IO,uint8_t value){
	switch(IO){
		case 0: if(value){PORTB|= (1<<0);}else{PORTB &= ~(1<<0);}break;
		case 1: if(value){PORTB|= (1<<1);}else{PORTB &= ~(1<<1);}break;
		case 2: if(value){PORTB|= (1<<2);}else{PORTB &= ~(1<<2);}break;
		case 3: if(value){PORTB|= (1<<3);}else{PORTB &= ~(1<<3);}break;
		case 4: if(value){PORTB|= (1<<4);}else{PORTB &= ~(1<<4);}break;
		case 5: if(value){PORTB|= (1<<5);}else{PORTB &= ~(1<<5);}break;
		case 6: if(value){PORTB|= (1<<6);}else{PORTB &= ~(1<<6);}break;
	}
}
uint8_t digitalRead(uint8_t IO){
	uint8_t value=0;
	switch(IO){
		case 0:  value= PINB& (1<<0) ;break;
		case 1:  value= PINB& (1<<1) ;break;
		case 2:  value= PINB& (1<<2) ;break;
		case 3:  value= PINB& (1<<3) ;break;
		case 4:  value= PINB& (1<<4) ;break;
		case 5:  value= PINB& (1<<5) ;break;
		case 6:  value= PINB& (1<<6) ;break;
	}
	return value;
}
#endif

const uint8_t ascii_to_scan_code_table[] PROGMEM = {
	// /* ASCII:   0 */
	0,
	// /* ASCII:   1 */
	0,
	// /* ASCII:   2 */
	0,
	// /* ASCII:   3 */
	0,
	// /* ASCII:   4 */
	0,
	// /* ASCII:   5 */
	0,
	// /* ASCII:   6 */
	0,
	// /* ASCII:   7 */
	0,
	/* ASCII:   8 */ 42,
	/* ASCII:   9 */ 43,
	/* ASCII:  10 */ 40,
	/* ASCII:  11 */ 0,
	/* ASCII:  12 */ 0,
	/* ASCII:  13 */ 0,
	/* ASCII:  14 */ 0,
	/* ASCII:  15 */ 0,
	/* ASCII:  16 */ 0,
	/* ASCII:  17 */ 0,
	/* ASCII:  18 */ 0,
	/* ASCII:  19 */ 0,
	/* ASCII:  20 */ 0,
	/* ASCII:  21 */ 0,
	/* ASCII:  22 */ 0,
	/* ASCII:  23 */ 0,
	/* ASCII:  24 */ 0,
	/* ASCII:  25 */ 0,
	/* ASCII:  26 */ 0,
	/* ASCII:  27 */ 41,
	/* ASCII:  28 */ 0,
	/* ASCII:  29 */ 0,
	/* ASCII:  30 */ 0,
	/* ASCII:  31 */ 0,
	/* ASCII:  32 */ 44,
	/* ASCII:  33 */ 158,
	/* ASCII:  34 */ 180,
	/* ASCII:  35 */ 160,
	/* ASCII:  36 */ 161,
	/* ASCII:  37 */ 162,
	/* ASCII:  38 */ 164,
	/* ASCII:  39 */ 52,
	/* ASCII:  40 */ 166,
	/* ASCII:  41 */ 167,
	/* ASCII:  42 */ 165,
	/* ASCII:  43 */ 174,
	/* ASCII:  44 */ 54,
	/* ASCII:  45 */ 45,
	/* ASCII:  46 */ 55,
	/* ASCII:  47 */ 56,
	/* ASCII:  48 */ 39,
	/* ASCII:  49 */ 30,
	/* ASCII:  50 */ 31,
	/* ASCII:  51 */ 32,
	/* ASCII:  52 */ 33,
	/* ASCII:  53 */ 34,
	/* ASCII:  54 */ 35,
	/* ASCII:  55 */ 36,
	/* ASCII:  56 */ 37,
	/* ASCII:  57 */ 38,
	/* ASCII:  58 */ 179,
	/* ASCII:  59 */ 51,
	/* ASCII:  60 */ 182,
	/* ASCII:  61 */ 46,
	/* ASCII:  62 */ 183,
	/* ASCII:  63 */ 184,
	/* ASCII:  64 */ 159,
	/* ASCII: A 65 */ 132,
	/* ASCII:  66 */ 133,
	/* ASCII:  67 */ 134,
	/* ASCII:  68 */ 135,
	/* ASCII:  69 */ 136,
	/* ASCII:  70 */ 137,
	/* ASCII:  71 */ 138,
	/* ASCII:  72 */ 139,
	/* ASCII:  73 */ 140,
	/* ASCII:  74 */ 141,
	/* ASCII:  75 */ 142,
	/* ASCII:  76 */ 143,
	/* ASCII:  77 */ 144,
	/* ASCII:  78 */ 145,
	/* ASCII:  79 */ 146,
	/* ASCII:  80 */ 147,
	/* ASCII:  81 */ 148,
	/* ASCII:  82 */ 149,
	/* ASCII:  83 */ 150,
	/* ASCII:  84 */ 151,
	/* ASCII:  85 */ 152,
	/* ASCII:  86 */ 153,
	/* ASCII:  87 */ 154,
	/* ASCII:  88 */ 155,
	/* ASCII:  89 */ 156,
	/* ASCII: Z 90 */ 157,
	/* ASCII:  91 */ 47,
	/* ASCII:  92 */ 49,
	/* ASCII:  93 */ 48,
	/* ASCII:  94 */ 163,
	/* ASCII:  95 */ 173,
	/* ASCII:  96 */ 53,
	/* ASCII: a 97 */ 4,
	/* ASCII:  98 */ 5,
	/* ASCII:  99 */ 6,
	/* ASCII: 100 */ 7,
	/* ASCII: 101 */ 8,
	/* ASCII: 102 */ 9,
	/* ASCII: 103 */ 10,
	/* ASCII: 104 */ 11,
	/* ASCII: 105 */ 12,
	/* ASCII: 106 */ 13,
	/* ASCII: 107 */ 14,
	/* ASCII: 108 */ 15,
	/* ASCII: 109 */ 16,
	/* ASCII: 110 */ 17,
	/* ASCII: 111 */ 18,
	/* ASCII: 112 */ 19,
	/* ASCII: 113 */ 20,
	/* ASCII: 114 */ 21,
	/* ASCII: 115 */ 22,
	/* ASCII: 116 */ 23,
	/* ASCII: 117 */ 24,
	/* ASCII: 118 */ 25,
	/* ASCII: 119 */ 26,
	/* ASCII: 120 */ 27,
	/* ASCII: 121 */ 28,
	/* ASCII: 122 */ 29,
	/* ASCII: 123 */ 175,
	/* ASCII: 124 */ 177,
	/* ASCII: 125 */ 176,
	/* ASCII: 126 */ 181
};

void keyPrintWord(char * word)
{releaseAll();
	uint8_t i=0;
	uint8_t len=strlen(word);
	for(i=0;i<len;i++){
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + word[i]);
				keyPrintChar2(data);
				break;
			}usbPoll();
		}
	}
}
void keyPrintWord2()
{releaseAll();
	uint8_t i=0;
	uint8_t len=getwords2length();
	for(i=0;i<len;i++){
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint8_t data = pgm_read_byte_near(words2 + i);
				keyPrintChar2(data);
				break;
			}usbPoll();
		}
	}
}
#endif