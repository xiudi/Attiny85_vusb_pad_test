/*
 * Functions.h
 *
 * Created: 2017/5/2 22:26:44
 *  Author: Administrator
 */ 
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <avr/io.h>
#include "usbdevice.h"

#define ROWS  1
#define COLS  2
#define WS2812_COUNT	2
//ws2812
#define WS2812_PORT		PORTB
#define WS2812_DDR		DDRB
#define WS2812_MASK		(1<<2)
#define WS2812_SAVE		1
#define Maxdelay 0x1000
////////

#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

void pinMode(uint8_t IO,uint8_t value);
void digitalWrite(uint8_t IO,uint8_t value);
uint8_t digitalRead(uint8_t IO);
void init_cols();
void init_rows();
void init_LED();
void Open_LED();
void Close_LED();
void LED();

#define _delay_after 0x90
#define _delay_before 0x40
uint8_t keymask[ROWS][COLS];
uint8_t hexaKeys1[ROWS][COLS];
uint8_t hexaKeys0[ROWS][COLS];
uint8_t rowPins[ROWS];
uint8_t colPins[COLS];
#define add1 10
#define add2 add1+ROWS //11
#define add3 add2+COLS //13
#define add4 add3+(ROWS*COLS) //15
#define add5 add4+(ROWS*COLS) //17
#define addRGB add5+(ROWS*COLS) //19
#define addPrint addRGB+(WS2812_COUNT*3) //25
uint8_t WS2812fix[(WS2812_COUNT * 3)];
uint8_t RGB_Type;// bit1-> 0 off 1 on ;bit0-> 0 fix£¬1 Rainbow
void ResetMatrix(uint8_t mask,uint16_t address);
void ResetMatrixFormEEP();
int init_main();
#endif /* FUNCTIONS_H_ */