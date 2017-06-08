#include "usbkeyboard2.h"

uchar  reportBuffer[2];
static uint8_t idleRate=0;
uint8_t pressing=1;

const  char usbHidReportDescriptor[] PROGMEM= { /* USB report descriptor */
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,                    // USAGE (Keyboard)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x01,                    //   REPORT_COUNT 1(simultaneous keystrokes)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
	0xc0                           // END_COLLECTION
};
/*
const uint8_t ascii_to_chinese_table[] ={
	98,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97
};
*/
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
void keyPrintWord2(){
	pressing=0;
	reportBuffer[0] = 0;
	reportBuffer[1] =0;
	uint16_t i=0;
	uint16_t len=eeprom_read_word((uint16_t *)0);
	for(i=0;i<len;i++){
		uint16_t address=i*2+2;
		if(address>(uint16_t)511)break;
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint16_t data = eeprom_read_word((uint16_t *)address);
				keyPrintChar((usbWord_t)data);
				break;
			}usbPoll();
		}
	}
	pressing=1;
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t    *rq = (usbRequest_t *)((void *)data);
	usbMsgPtr = reportBuffer; //
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {return 0;}
		else if (rq->bRequest == USBRQ_HID_GET_IDLE) {usbMsgPtr=&idleRate;}
		else if (rq->bRequest == USBRQ_HID_SET_IDLE) {idleRate = rq->wValue.bytes[1];}
	}
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) ==USBRQ_TYPE_VENDOR) {
		if (rq->bRequest == 0x16) {
			eeprom_busy_wait();
			eeprom_write_word ((uint16_t *)rq->wIndex.word,rq->wValue.word);
			PORTB|=(1<<1);
		return 0;}
		if (rq->bRequest == 0x08) {
			eeprom_busy_wait();
			eeprom_write_byte ((uint8_t *)rq->wIndex.word,rq->wValue.bytes[0]);
			PORTB|=(1<<1);
		return 0;}
		if (rq->bRequest == 0x03) {
			//结束写
			eeprom_busy_wait();
			PORTB&= ~(1<<1);
			pressing=1;
		return 0;}
		if (rq->bRequest == 0x01) {
			//开始写
			pressing=0;
			eeprom_is_ready();
			PORTB&= ~(1<<1);
		return 0;}
	}
	return 0;
}
int setup(void){
	
	DDRB &= ~(1<<0);
	DDRB |= (1<<1);
	DDRB &= ~(1<<2);

	PORTB|= (1<<0);
	PORTB&= ~(1<<1);
	PORTB|= (1<<2);
	
	usb_init();
	uint8_t sign0=0;
	while (1)
	{
		if((PINB&(1<<0))==0)
		{
			if(sign0==0 && pressing)keyPrintWord2();
			sign0=0x20;
		}
		if(sign0>0)sign0--;
		usbPoll();
	}
	return 0;
}
