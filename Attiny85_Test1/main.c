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


#include "1key/usbkeyboard2.h"


int main(){
usb_init();
while (1)
{

}
return 0;

}