/*
 * usbkeyboard.h
 *
 * Created: 2017/5/22 8:46:47
 *  Author: Administrator
 */ 


#ifndef USBKEYBOARD2_H_
#define USBKEYBOARD2_H_
//#define  ONEKEY 

#include "../usbdrv/usbdrv.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#ifdef ONEKEY

uint8_t    reportBuffer[2];
extern const  uint8_t  words2[] PROGMEM;

void usb_init();
int getwords2length();
void keyPrintWord2();
int setup();


#endif
#endif