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
#define ROWS  1
#define COLS  2
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
void ResetMatrix(uint8_t mask,uint16_t address);
void ResetMatrixFormEEP();
int init_main();
#endif /* FUNCTIONS_H_ */