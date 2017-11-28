#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "usbkeyboard.h"

int main(void)
{
return setup();
}
/*
1 eep:
macro_number+macro_head+macro
2 macro_head:
address address length length macro_type color
3 macro_type:
type+length:
0x01 English letters
0x02 Chinese characters
0x11 modifier+key[1]
0x12 modifier+key[2]
...
0x16 modifier+key[6]
0x21 mouse_buttons
0x25 mouse_buttons;x;y;v;h
0x31 system
0x41 consumer
__________________________
press->send keys
release->send blank
4 macro
if(type&0xF0!=0)
(uint8_t)time+ macro
*/