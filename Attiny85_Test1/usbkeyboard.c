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

typedef struct {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
} report_keyboard_t;
static report_keyboard_t keyboard_report;

uint8_t keyboard_modifier_keys=0;
uint8_t keyboard_keys[6]={0,0,0,0,0,0};
volatile uint8_t keyboard_leds=0;

#define KBUF_SIZE 16
static report_keyboard_t kbuf[KBUF_SIZE];
static uint8_t kbuf_head = 0;
static uint8_t kbuf_tail = 0;

static uint8_t vusb_idle_rate = 0;

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
static void vusb_transfer_keyboard()
{
	if (usbInterruptIsReady()) {
		if (kbuf_head != kbuf_tail) {
			usbSetInterrupt((void *)&kbuf[kbuf_tail], sizeof(report_keyboard_t));
			kbuf_tail = (kbuf_tail + 1) % KBUF_SIZE;
		}
	}
}
static void send_keyboard(report_keyboard_t *report){
	uint8_t next = (kbuf_head + 1) % KBUF_SIZE;
	if (next != kbuf_tail) {
		kbuf[kbuf_head] = *report;
		kbuf_head = next;
	}
	usbPoll();
	vusb_transfer_keyboard();
}
uint8_t usb_keyboard_send(){
	uint8_t send_required=0;
	usbPoll();
	if (usbConfiguration && usbInterruptIsReady()) {		
		keyboard_report.modifier = keyboard_modifier_keys;		
		keyboard_report.keycode[0]=keyboard_keys[0];
		keyboard_report.keycode[1]=keyboard_keys[1];
		keyboard_report.keycode[2]=keyboard_keys[2];
		keyboard_report.keycode[3]=keyboard_keys[3];
		keyboard_report.keycode[4]=keyboard_keys[4];
		keyboard_report.keycode[5]=keyboard_keys[5];
		send_keyboard(&keyboard_report);
		send_required=1;
	}
	vusb_transfer_keyboard();
	return send_required;
}

static struct {
	uint16_t len;
	enum {
		NONE,
		SET_LED
	} kind;
} last_req;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
		usbRequest_t *rq = (usbRequest_t *)((void *)data);
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
					 last_req.kind = SET_LED;
					 last_req.len = rq->wLength.word;
				 }
				 return USB_NO_MSG; 				
			 }		 
		 }	
		return 0;
}
uchar usbFunctionWrite(uchar *data, uchar len)
{
	//���ָʾ��
	if (last_req.len == 0) {
		return -1;
	}
	switch (last_req.kind) {
		case SET_LED:
		keyboard_leds = data[0];
		last_req.len = 0;
		return 1;
		break;
		case NONE:
		default:
		return -1;
		break;
	}
	return 1;
}



