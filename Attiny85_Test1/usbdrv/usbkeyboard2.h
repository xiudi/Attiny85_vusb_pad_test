#ifndef USBKEYBOARD2_H_
#define USBKEYBOARD2_H_


#include "usbdrv.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


uint8_t    reportBuffer[2];
void usb_init();
void keyPrintWord2();
int setup();

#endif