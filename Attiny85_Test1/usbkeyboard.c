#include "usbkeyboard.h"/*
* usbkeyboard.c
*
* Created: 2017/4/27 17:19:55
*  Author: Administrator
*/
const PROGMEM char usbHidReportDescriptor[63] = { /* USB report descriptor */
	0x05, 0x01,          // Usage Page (Generic Desktop),
	0x09, 0x06,          // Usage (Keyboard),
	0xA1, 0x01,          // Collection (Application),
	0x75, 0x01,          //   Report Size (1),
	0x95, 0x08,          //   Report Count (8),
	0x05, 0x07,          //   Usage Page (Key Codes),
	0x19, 0xE0,          //   Usage Minimum (224),
	0x29, 0xE7,          //   Usage Maximum (231),
	0x15, 0x00,          //   Logical Minimum (0),
	0x25, 0x01,          //   Logical Maximum (1),
	0x81, 0x02,          //   Input (Data, Variable, Absolute), ;Modifier byte
	0x95, 0x01,          //   Report Count (1),
	0x75, 0x08,          //   Report Size (8),
	0x81, 0x03,          //   Input (Constant),                 ;Reserved byte
	0x95, 0x05,          //   Report Count (5),
	0x75, 0x01,          //   Report Size (1),
	0x05, 0x08,          //   Usage Page (LEDs),
	0x19, 0x01,          //   Usage Minimum (1),
	0x29, 0x05,          //   Usage Maximum (5),
	0x91, 0x02,          //   Output (Data, Variable, Absolute), ;LED report
	0x95, 0x01,          //   Report Count (1),
	0x75, 0x03,          //   Report Size (3),
	0x91, 0x03,          //   Output (Constant),                 ;LED report padding
	0x95, 0x06,          //   Report Count (6),
	0x75, 0x08,          //   Report Size (8),
	0x15, 0x00,          //   Logical Minimum (0),
	0x25, 0x68,          //   Logical Maximum(104),
	0x05, 0x07,          //   Usage Page (Key Codes),
	0x19, 0x00,          //   Usage Minimum (0),
	0x29, 0x68,          //   Usage Maximum (104),
	0x81, 0x00,          //   Input (Data, Array),
	0xc0                 // End Collection
};
uint8_t keyboard_modifier_keys=0;
uint8_t keyboard_keys[6]={0,0,0,0,0,0};
volatile uint8_t keyboard_leds=0;

static uint8_t reportBuffer[8]={0,0,0,0,0,0,0,0};
static uint8_t keyboard_idle_config=125;
static uint8_t keyboard_idle_count=0;

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
uint8_t releasekey(uint8_t key)
{
	uint8_t send_required=0;
	for (uint8_t i=0; i < 6; i++) {
		if (keyboard_keys[i] == key) {
			keyboard_keys[i] = 0;
			send_required=1;
			break;
		}
	}
	return send_required;
}
void releaseAll()
{
	for (uint8_t i=0; i < 6; i++) {
		keyboard_keys[i] = 0;
	}
	keyboard_modifier_keys=0;
}
uint8_t presskey(uint8_t key)
{
	for (uint8_t i=0; i < 6; i++) {
		if (keyboard_keys[i] == key) {
			return 1;
		}
	}
	for (uint8_t i=0; i < 6; i++) {
		if (keyboard_keys[i] == 0) {
			keyboard_keys[i] = key;
			return 1;
		}
	}
	return 0;
}
void pressModifierKeys(uint8_t key)
{
	keyboard_modifier_keys|=key;
}
void releaseModifierKeys(uint8_t key)
{
	keyboard_modifier_keys&=~key;
}
uint8_t usb_keyboard_send()
{
	uint8_t send_required=0;
	//主循环中必定需要在最后usbPoll去更新一下usb状态。否则PC会识别错误。
	if (usbInterruptIsReady()) {
		reportBuffer[0]=keyboard_modifier_keys;
		reportBuffer[2]=keyboard_keys[0];
		reportBuffer[3]=keyboard_keys[1];
		reportBuffer[4]=keyboard_keys[2];
		reportBuffer[5]=keyboard_keys[3];
		reportBuffer[6]=keyboard_keys[4];
		reportBuffer[7]=keyboard_keys[5];
		usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
		send_required=1;
	}
	usbPoll();
	return send_required;
}
usbMsgLen_t  usbFunctionSetup(uint8_t data[8]) {
	//一般功能设置,这个函数在usbPoll更新之后对于PC反馈的数据进行处理，比如键盘灯
	usbRequest_t    *rq = (usbRequest_t *)((void *)data);
	usbMsgPtr = reportBuffer;
	
	if (rq->bmRequestType == 0x21) {
		if (rq->bRequest == USBRQ_HID_SET_REPORT) {
			keyboard_leds = rq->wValue;		
			return 0;
		}
		if (rq->bRequest == USBRQ_HID_SET_IDLE) {
			keyboard_idle_config= rq->wValue.bytes[1];
			keyboard_idle_count=0;
			return 0;
		}		
	}
	return 0;
}
