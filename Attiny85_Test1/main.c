/*
 * Attiny85_Test1.c
 *
 * Created: 2017/4/27 15:44:39
 * Author : zian1
 //Prototype lib is vusb-20121206
 //device need to fix osccal.h
 //usbdrv need to create usbconfig.h. And fix USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH=63
 //this two files can be copyed from digispark
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "usbkeyboard.h"
#include "Functions.h"
int main(void)
{
pinMode(0,INPUT);digitalWrite(0,HIGH);
pinMode(1,OUTPUT);digitalWrite(1,LOW);
pinMode(2,INPUT);digitalWrite(2,HIGH);
usb_init();
    /* Replace with your application code */
    while (1) 
    {		
			if(digitalRead(0)==LOW)
			{
				keyboard_modifier_keys=0;
				keyboard_keys[0]=KEY_A;
				keyboard_keys[1]=0;
				keyboard_keys[2]=0;
				keyboard_keys[3]=0;
				keyboard_keys[4]=0;
				keyboard_keys[5]=0;
			}
			else if(digitalRead(2)==LOW){
				keyboard_modifier_keys=KEY_CTRL;
				memset(keyboard_keys, 0, 6);
			}
			else{
				keyboard_modifier_keys=0;
				memset(keyboard_keys, 0, 6);
			}
			uint8_t i=0;
			for ( i=0; i<3; i++){
				if((keyboard_leds&(1<<i))==(1<<i))
				{ digitalWrite(1,HIGH);}
				else{ digitalWrite(1,LOW);}
			}

		
		usb_keyboard_send();
    }
	return 0;
}

