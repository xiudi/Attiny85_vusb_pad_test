/*
 * TypeCode.c
 *
 * Created: 2017/5/9 16:43:13
 *  Author: Administrator
 */ 
 #include "usbkeyboard2.h"
 #include <avr/pgmspace.h>
  #ifdef ONEKEY
 const  uint8_t  words2[] PROGMEM ={
	 11,8,15,15,18,44,26,18,21,15,7,158
	 //hello world£¡
 };
  int getwords2length(){
	  return sizeof(words2)/sizeof(words2[0]);
  }

  #endif