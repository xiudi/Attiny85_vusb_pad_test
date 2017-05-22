#include "usbkeyboard2.h"

const PROGMEM char usbHidReportDescriptor[] = { /* USB report descriptor */
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
uchar  reportBuffer[2];

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
static uint8_t    idleRate
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t    *rq = (usbRequest_t *)((void *)data);
	usbMsgPtr = reportBuffer; //
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {return 0;}
		else if (rq->bRequest == USBRQ_HID_GET_IDLE) {return 0;}
		else if (rq->bRequest == USBRQ_HID_SET_IDLE) {idleRate = rq->wValue.bytes[1];}				
	}
	return 0;
}