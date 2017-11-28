#include "usbkeyboard.h"

uchar  reportBuffer[2];
static uint8_t idleRate=0;
uint8_t enable_pressing=1;
 void _delay_n(uint8_t n)       //N ms精确延时nms
 {
	 uint8_t i=0;
	 for (i=0;i<n;i++)
	 {
		 _delay_ms(100);
	 }
 }
void usb_init()
{
	usbInit();
	/* enforce USB re-enumerate: */
	usbDeviceDisconnect();  /* do this while interrupts are disabled */
	//wdt_reset();
	/* fake USB disconnect for > 250 ms */
	_delay_ms(300);
	usbDeviceConnect();
	sei();
}

void keyPrintChinese(uint8_t data[5]){
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = 0x40;
			reportBuffer[1] =0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}usbPoll();
	uint8_t i=0;
	for( i=0;i<5;i++){

		while(1){usbPoll();
			if(usbConfiguration && usbInterruptIsReady()){
				reportBuffer[1]=98;
				if(data[i]>0){reportBuffer[1]=data[i]+88;}
				usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
				break;
			}
		}usbPoll();
		while(1){usbPoll();
			if(usbConfiguration && usbInterruptIsReady()){
				reportBuffer[1] =0;
				usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
				break;
			}
		}usbPoll();
	}

	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = 0;
			reportBuffer[1] =0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}usbPoll();
}
void keyPrintEnglish(uint8_t data)
{
	if(data==0)return;
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = (data >> 7) ? 0x20 : 0x00;//shift加了128
			reportBuffer[1] =data & 0b01111111;//abs删除正负号
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}usbPoll();
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = 0;
			reportBuffer[1] =0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}usbPoll();
}
void keyPrintChar(usbWord_t data){
	if(data.bytes[1]==0x00){
		keyPrintEnglish(data.bytes[0]);
		}else{
		uint16_t out=(uint16_t)data.word;
		//out|=0x8080;//汉字内码每个byte最高位为1
		uint8_t datachinese[5];
		datachinese[4]=out%10;out=out/10;
		datachinese[3]=out%10;out=out/10;
		datachinese[2]=out%10;out=out/10;
		datachinese[1]=out%10;out=out/10;
		datachinese[0]=out;
		keyPrintChinese(datachinese);

	}
}
void keyPrintWordEEP(uint16_t address_t,uint16_t len){
//len表示字符个数
	enable_pressing=0;
	reportBuffer[0] = 0;
	reportBuffer[1] =0;
	uint16_t i=0;
	for(i=0;i<len;i++){
		uint16_t address=address_t+i*2;
		if(address>(uint16_t)511)break;
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint16_t data = eeprom_read_word((uint16_t *)address);
				keyPrintChar((usbWord_t)data);
				break;
			}usbPoll();
		}
	}
	enable_pressing=1;
}
void keyPrintMacro2XEEP(uint16_t address_t,uint16_t len){
	//len表示macro个数
	enable_pressing=0;
	reportBuffer[0] = 0;
	reportBuffer[1] =0;
	uint16_t i=0;
	for(i=0;i<len;i++){
		uint16_t address=address_t+i*3;
		if(address>(uint16_t)511)break;
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint8_t time_t=eeprom_read_byte((uint8_t *)address);
				reportBuffer[0] = eeprom_read_byte((uint8_t *)(address+1));
				reportBuffer[1] = eeprom_read_byte((uint8_t *)(address+2));
				usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
				_delay_n(time_t);
				break;
			}usbPoll();
		}		
	}
	enable_pressing=1;
}
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t    *rq = (usbRequest_t *)((void *)data);
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {
		usbMsgPtr = reportBuffer;return sizeof(reportBuffer);}
		else if(rq->bRequest == USBRQ_HID_SET_REPORT){return USB_NO_MSG;}
		else if (rq->bRequest == USBRQ_HID_GET_IDLE) {usbMsgPtr=&idleRate;}
		else if (rq->bRequest == USBRQ_HID_SET_IDLE) {idleRate = rq->wValue.bytes[1];}
	}
	return 0;
}
void usbFunctionWriteOut(uchar *data, uchar len){
	if(len==8){
		if(data[0]==0xFF && data[1]==0xF1){
			enable_pressing=0;
			return;
		}
		if(data[0]==0xFF && data[1]==0xF2){
			enable_pressing=1;
			return;
		}
		PORTB|=(1<<1);
		usbWord_t data1,data2,data3,data4;
		data1.bytes[0]=data[0];data1.bytes[1]=data[1];
		data2.bytes[0]=data[2];data2.bytes[1]=data[3];
		data3.bytes[0]=data[4];data3.bytes[1]=data[5];
		data4.bytes[0]=data[6];data4.bytes[1]=data[7];
		if(data1.word<510){
			eeprom_busy_wait();
			eeprom_write_word ((uint16_t *)data1.word,data2.word);
		}
		if(data1.word+2<510){
			eeprom_busy_wait();
			eeprom_write_word ((uint16_t *)(data1.word+2),data3.word);
		}
		if(data1.word+4<510){
			eeprom_busy_wait();
			eeprom_write_word ((uint16_t *)(data1.word+4),data4.word);
		}
		PORTB&= ~(1<<1);
	}
}
int setup(void){	
	DDRB &= ~(1<<0);
	DDRB &= ~(1<<1);
	PORTB|= (1<<0);
	PORTB|= (1<<1);	
	usb_init();
	uint8_t sign0=0;
	while (1)
	{
	usbPoll();
		if((PINB&(1<<0))==0)
		{
			if(sign0==0 && enable_pressing)
			//keyPrintWordEEP(0);
			sign0=0x20;
		}
		if(sign0>0)sign0--;	
	}
	return 0;
}
