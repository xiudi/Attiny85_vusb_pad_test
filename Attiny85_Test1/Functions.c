/*
* Functions.c
*
* Created: 2017/5/2 22:26:56
*  Author: Administrator
*/
#include "Functions.h"
#include <string.h>
#include "scancode-ascii-table.h"

void keyPrintWord(uchar * word)
{
uint8_t i=0;
uint8_t len=sizeof(word);
for(i=0;i<len;i++){
keyPrintChar(word[i]);
}
}
void keyPrintChar(uint8_t chr)
{
    releaseAll();usb_keyboard_send();
	  uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
	  keyboard_keys[0]=data & 0b01111111, 
	  keyboard_modifier_keys= (data >> 7) ? (1<<5) : 0;
	  usb_keyboard_send();
	releaseAll();usb_keyboard_send();
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