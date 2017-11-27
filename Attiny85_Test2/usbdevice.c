#include "usbdevice.h"
uint8_t usb_keyboard_send_required(){
	uint8_t send_required_t=0;
	if(keyboard_report.modifier!=keyboard_buffer.keyboard_modifier_keys)
	{keyboard_report.modifier = keyboard_buffer.keyboard_modifier_keys;send_required_t=1;}
	for(uint8_t i=0;i<KEY_CODE_NUMBER;i++){
		if(keyboard_report.keycode[i]!=keyboard_buffer.keyboard_keys[i])
		{keyboard_report.keycode[i]=keyboard_buffer.keyboard_keys[i];send_required_t=1;}
	}
	if(send_required_t)keyboard_buffer.Send_Required=send_required_t;
	return send_required_t;
}
uint8_t presskey(uint8_t key){
	uint8_t i;
	for ( i=0; i < KEY_CODE_NUMBER; i++) {
		if (keyboard_buffer.keyboard_keys[i] == key) {
			return 1;
		}
	}
	for ( i=0; i < KEY_CODE_NUMBER; i++) {
		if (keyboard_buffer.keyboard_keys[i] == 0) {
			keyboard_buffer.keyboard_keys[i] = key;
			return 1;
		}
	}
	return 0;
}
void usb_update(){
	usbPoll();
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
	ClearKeyboard();
	#if MOUSE_ENABLE
	ClearMouse();
	#endif
	#if RAW_ENABLE
	ClearRaw();
	#endif
}
uint8_t i2=0;
uint8_t usb_keyboard_send(){
	if(keyboard_buffer.Send_Required==0)return 0;
	i2=0;
	while(++i2<50){
		usbPoll();
		if (usbInterruptIsReady()){
			if(keyboard_buffer.Send_Required){
				usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
				keyboard_buffer.Send_Required=0;
			}
			return 1;
		}
	}
	return 0;
}
void releaseAllkeyboardkeys(){
	uint8_t i;
	for ( i=0; i < KEY_CODE_NUMBER; i++) {
		keyboard_buffer.keyboard_keys[i] = 0;
	}
	keyboard_buffer.keyboard_modifier_keys=0;
}
void pressModifierKeys(uint8_t key){
	keyboard_buffer.keyboard_modifier_keys|=key;
}
void ClearKeyboard(){
	memset( &keyboard_report, 0,sizeof(keyboard_report));
	memset( &keyboard_buffer, 0,sizeof(keyboard_buffer));
	keyboard_buffer.enable_pressing=1;
}
#if MOUSE_ENABLE
uint8_t i1=0;
void ClearMouse(){
	memset(&mouse_report, 0, sizeof(mouse_report));
	memset(&mouse_buffer,0,sizeof(mouse_buffer));
	mouse_report.mouse.report_id= REPORT_ID_MOUSE;
	mouse_report.system_keys.report_id= REPORT_ID_SYSTEM;
	mouse_report.consumer_keys.report_id= REPORT_ID_CONSUMER;
}
uint8_t usb_mouse_send(){
	if(mouse_buffer.Send_Required==0)return 0;
	i1=0;
	while(++i1<50){
		usbPoll();
		if (usbInterruptIsReady3()){
			if(mouse_buffer.Send_Required==REPORT_ID_MOUSE){
				usbSetInterrupt3((void *)&mouse_report.mouse, sizeof(report_mouse0_t));
				mouse_buffer.Send_Required=0;
			}
			else if(mouse_buffer.Send_Required==REPORT_ID_SYSTEM){
				usbSetInterrupt3((void *)&mouse_report.system_keys, sizeof(report_extra_t));
				mouse_buffer.Send_Required=0;
			}
			else if(mouse_buffer.Send_Required==REPORT_ID_CONSUMER){
				usbSetInterrupt3((void *)&mouse_report.consumer_keys, sizeof(report_extra_t));
				mouse_buffer.Send_Required=0;
			}
			return 1;
		}
	}
	return 0;
}
uint8_t usb_mouse_send_required(){
	uint8_t send_required_t=0;
	if(mouse_report.mouse.buttons!=mouse_buffer.mouse_keys)
	{
		mouse_report.mouse.buttons=mouse_buffer.mouse_keys;
		send_required_t=REPORT_ID_MOUSE;
	}
	if(mouse_report.system_keys.usage!=mouse_buffer.system_keys)
	{
		mouse_report.system_keys.usage=mouse_buffer.system_keys;
		send_required_t=REPORT_ID_SYSTEM;
	}
	if(mouse_report.consumer_keys.usage!=mouse_buffer.consumer_keys)
	{
		mouse_report.consumer_keys.usage=mouse_buffer.consumer_keys;
		send_required_t=REPORT_ID_CONSUMER;
	}
	if(send_required_t){
		mouse_buffer.Send_Required=send_required_t;
	}
	return send_required_t;
}
void releaseAllmousekeys(){
	mouse_buffer.mouse_keys=0;
	mouse_buffer.system_keys=0;
	mouse_buffer.consumer_keys=0;
}
void pressmousekey(uint8_t key){
	mouse_buffer.mouse_keys|=key;
}
void presssystemkey(uint8_t key){
	mouse_buffer.system_keys=(uint16_t)key;
}
void pressconsumerkey(uint8_t key){
	mouse_buffer.consumer_keys=(uint16_t)key;
}
void MousePrintMousekey(uint8_t data)
{
	if(data==0)return;
	report_mouse0_t MouseReporttemp;
	memset(&MouseReporttemp,0,6);
	MouseReporttemp.report_id=REPORT_ID_MOUSE;
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady3()){
			MouseReporttemp.buttons|=data;
			usbSetInterrupt3((void*)&MouseReporttemp, sizeof(report_mouse0_t));
			break;
		}
	}usbPoll();
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady3()){
			MouseReporttemp.buttons=0;
			usbSetInterrupt3((void*)&MouseReporttemp, sizeof(report_mouse0_t));
			break;
		}
	}usbPoll();
}
#endif
#if RAW_ENABLE
void ClearRaw(){
	memset(&raw_report_out, 0,sizeof(raw_report_out));
}
#endif
usbRequest_t* rq ;
uint8_t vusb_idle_rate = 0;
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	rq = (usbRequest_t *)((void *)data);
	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
		if(rq->bRequest == USBRQ_HID_GET_REPORT){
			usbMsgPtr = (void *)&keyboard_report;
			return sizeof(keyboard_report);
		}
		else if(rq->bRequest == USBRQ_HID_GET_IDLE){
			usbMsgPtr = &vusb_idle_rate;
			return 1;
		}
		else if(rq->bRequest == USBRQ_HID_SET_IDLE){
			vusb_idle_rate = rq->wValue.bytes[1];
		}
		else if(rq->bRequest == USBRQ_HID_SET_REPORT){
			if (rq->wValue.word == 0x0200 && rq->wIndex.word == 0) {
				//rq->wLength.word==0x0001
				keyboard_buffer.keyboard_leds|=(1<<7);//用最高位标注状态灯，赋值后自动还原0
			}
			return USB_NO_MSG;
		}
	}
	return 0;
}
uchar usbFunctionWrite(uchar *data, uchar len)
{
	if ((keyboard_buffer.keyboard_leds&(1<<7))==(1<<7)) {
		keyboard_buffer.keyboard_leds = data[0];
		return 1;
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////
void pressmacrokey(uint8_t key){
	if(key==MACRO2){
		if(keyboard_report.modifier){
			//??????keyboard buffer ???buffer???????????
			//report ?????????
			presskey(KEY_TILDE);
		}
		else{
			presskey(KEY_ESC);
		}
	}
	macrobuffer|=key;
}
uint8_t usb_macro_send_required(){
	if (macroreport!=macrobuffer){
		macroreport=macrobuffer;
	return 1;}
	return 0;
}
void keyPrintCtl(uint8_t data){
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier=KEY_CTRL;
			keyboard_report.keycode[0]=0;keyboard_report.keycode[1]=0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier=KEY_CTRL;
			keyboard_report.keycode[0]=data;keyboard_report.keycode[1]=0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier=0;
			keyboard_report.keycode[0]=0;keyboard_report.keycode[1]=0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
}
void keyPrintChinese(uint8_t data[5]){
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier = 0x40;
			keyboard_report.keycode[0] =0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
	uint8_t i=0;
	for( i=0;i<5;i++){
		while(1){usbPoll();
			if(usbConfiguration && usbInterruptIsReady()){
				keyboard_report.keycode[0]=98;
				if(data[i]>0){keyboard_report.keycode[0]=data[i]+88;}
				usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
				break;
			}
		}usbPoll();
		while(1){usbPoll();
			if(usbConfiguration && usbInterruptIsReady()){
				keyboard_report.keycode[0] =0;
				usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
				break;
			}
		}usbPoll();
	}

	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier = 0;
			keyboard_report.keycode[0] =0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
}
void keyPrintEnglish(uint8_t data)
{
	if(data==0)return;
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier = (data >> 7) ? 0x20 : 0x00;//shift加了128
			keyboard_report.keycode[0] =data & 0b01111111;//abs删除正负号
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			keyboard_report.modifier = 0;
			keyboard_report.keycode[0] =0;
			usbSetInterrupt((void *)&keyboard_report, sizeof(report_keyboard_t));
			break;
		}
	}usbPoll();
}
void keyPrintChar(usbWord_t data){
	keyboard_report.modifier = 0;
	memset(keyboard_report.keycode,0,6);
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
void keyPrintWordEEP(uint16_t address_t){
	keyboard_buffer.enable_pressing=0;
	uint16_t i=0;
	uint16_t len=eeprom_read_word((uint16_t *)address_t);
	for(i=0;i<len;i++){
		uint16_t address=address_t+i*2+2;
		if(address>maxEEP-1)break;
		while(1){
			if(usbConfiguration && usbInterruptIsReady()){
				uint16_t data = eeprom_read_word((uint16_t *)address);
				keyPrintChar((usbWord_t)data);
				break;
			}usbPoll();
		}
	}
	keyboard_buffer.enable_pressing=1;
}
