#ifndef USBKEYBOARD_H_
#define USBKEYBOARD_H_


#include "usbdrv/usbdrv.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


uint8_t    reportBuffer[2];
void usb_init();
void keyPrintWordEEP(uint16_t address_t,uint16_t len);
void keyPrintMacro2XEEP(uint16_t address_t,uint16_t len);
int setup();

#endif