#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "ws2812.h"
#include "Functions.h"
uint8_t r,c,i,FN;
uint8_t delay_after=0;//backswing
uint8_t delay_before=0;//windup
uint8_t ledmacro=0;
uint16_t delayval;
uint8_t cindex[WS2812_COUNT]={0,0};
uint8_t rowPins[ROWS]={0xFF};
uint8_t colPins[COLS]={0,1};
uint8_t hexaKeys0[ROWS][COLS] = {{KEY_Z,KEY_X}};
uint8_t hexaKeys1[ROWS][COLS] ={{0,0}};
uint8_t keymask[ROWS][COLS] = {{0x10,0x10}};
uint8_t usb_macro_send(){
	ledmacro^=macroreport;
	if(macroreport&MACRO3){
		keyPrintWordEEP(addPrint+6);
		//exe program upload 6 bytes once 
		return 1;
	}
	if(macroreport&MACRO4){
		keyPrintCtl(KEY_A);
		return 1;
	}
	if(macroreport&MACRO5){
		keyPrintCtl(KEY_X);
		return 1;
	}
	if(macroreport&MACRO6){
		keyPrintCtl(KEY_C);
		return 1;
	}
	if(macroreport&MACRO7){
		keyPrintCtl(KEY_V);
		return 1;
	}
	return 0;
}
void init_LED(){
	WS2812Setup();delayval=Maxdelay;
	WS2812Clear();
	WS2812Send2();
	if((RGB_Type&0xF0)==0x10){ledmacro|=(1<<1);}else{ledmacro&= ~(1<<1);}
}
void LED(){
	if(delayval>=Maxdelay){
		if(ledmacro & (1<<1)){
			for(uint8_t i=0;i<WS2812_COUNT;i++){
				if((RGB_Type&0x0F)==0x01){
					if(cindex[i]>=WS2812ColorCount) cindex[i]=0;
					uint8_t r=pgm_read_byte(Rcolors+cindex[i]);
					uint8_t g=pgm_read_byte(Gcolors+cindex[i]);
					uint8_t b=pgm_read_byte(Bcolors+cindex[i]);
					WS2812SetRGB(i,r,g,b);
					cindex[i]++;
				}
				else if((RGB_Type&0x0F)==0x00){
					WS2812SetRGB(i,WS2812fix[i*3],WS2812fix[i*3+1],WS2812fix[i*3+2]);
				}
			}
		}else{WS2812Clear();}
		delayval--;
		WS2812Send2();
	}else{if(delayval){delayval--;}else {delayval=Maxdelay;}}
}
void Open_LED(){
for(uint8_t i=0;i<WS2812_COUNT;i++){
 WS2812SetRGB(i,0,0,255);
}
WS2812Send2();
}
void Close_LED(){
WS2812Clear();
WS2812Send2();
}
void init_IO(){
	DDRB&=~(1<<0)|(1<<1);
	PORTB|=(1<<0)|(1<<1);
}
void TinykeyMode(){
	FN=0xF0;
		for (c = 0; c < COLS; c++) {
			if (digitalRead(colPins[c])) {keymask[0][c]&= ~0x88;}
			else {keymask[0][c]|= 0x88;delay_after=_delay_after;}
			if(keymask[0][c]==0xEE )FN=0x0F;
		}
	releaseAllkeyboardkeys();
	#if MOUSE_ENABLE
	releaseAllmousekeys();
	#endif
	macrobuffer=0;
		for (c = 0; c < COLS; c++) {
			switch(keymask[0][c]&FN){
				case 0x90:
				presskey(hexaKeys0[0][c]);
				break;
				case 0xA0:
				pressModifierKeys(hexaKeys0[0][c]);
				break;
				case 0xF0:
				pressmacrokey(hexaKeys0[0][c]);
				break;
				case 0x09:
				presskey(hexaKeys1[0][c]);
				break;
				case 0x0A:
				pressModifierKeys(hexaKeys1[0][c]);
				break;
				case 0x0F:
				pressmacrokey(hexaKeys1[0][c]);
				break;
				#if MOUSE_ENABLE
				case 0xB0:
				pressmousekey(hexaKeys0[0][c]);
				break;
				case 0xC0:
				presssystemkey(hexaKeys0[0][c]);
				break;
				case 0xD0:
				pressconsumerkey(hexaKeys0[0][c]);
				break;
				case 0x0B:
				pressmousekey(hexaKeys1[0][c]);
				break;
				case 0x0C:
				presssystemkey(hexaKeys1[0][c]);
				break;
				case 0x0D:
				pressconsumerkey(hexaKeys1[0][c]);
				break;
				#endif
			}
	}
	if(usb_macro_send_required())delay_before=_delay_before;
	if(usb_keyboard_send_required())delay_before=_delay_before;
	#if MOUSE_ENABLE
	if(usb_mouse_send_required())delay_before=_delay_before;
	#endif
	if(delay_after==_delay_after && delay_before==1)
	{usb_macro_send();usb_keyboard_send();
		#if MOUSE_ENABLE
		usb_mouse_send();
		#endif
	}
	if(delay_after==1)
	{usb_macro_send();usb_keyboard_send();
		#if MOUSE_ENABLE
		usb_mouse_send();
		#endif
	}
	if(delay_after>0)delay_after-=1;
	if(delay_before>0)delay_before-=1;
}
int init_main(void) {
	usb_init();
	////////////////////////////////////////////////
	init_IO();
	while (1) {		
		keyboard_buffer.enable_pressing=1;
		releaseAllkeyboardkeys();
		releaseAllmousekeys();
		ClearMacro();
		ResetMatrixFormEEP();		
		_delay_ms(500);
		init_LED();
		usb_keyboard_send();
		while (1) {
			usbPoll();
			if(keyboard_buffer.enable_pressing==2){
				break;
			}
			else if(keyboard_buffer.enable_pressing==1){
				TinykeyMode();
				if (usbConfiguration && usbInterruptIsReady()){
					if(delay_before==0)LED();
				}
			}
		}
	}
	return 0;
}