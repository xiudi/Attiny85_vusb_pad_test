#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
//#include <string.h>

#include "usbdrv/usbconfig.h"
#ifdef ONEKEY
#include "1key/usbkeyboard2.h"
#endif
#ifdef SIXKEY
#include "6key/Functions.h"
#endif

int main(void)
{
return setup();
}
