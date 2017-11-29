#ifndef WS2812_h_
#define WS2812_h_

#include <avr/io.h>
#include <stdlib.h>
#include "avr/pgmspace.h"
#include "Functions.h"

#define nop() __asm__ __volatile__ ("nop")
//ÄÚÇ¶»ã±àÖ¸Áînop ÑÓÊ± 0.25us
uint8_t WS2812Buffer[(WS2812_COUNT * 3)];
#define WS2812ColorCount 0xFF

void WS2812Setup(void);
void WS2812Clear(void);
void WS2812SetRGB(uint16_t led, uint8_t red, uint8_t green, uint8_t blue);
void WS2812SetHSV(uint16_t led, uint16_t hue, uint16_t saturation, uint16_t value);
void WS2812Send(void);
void WS2812Send2(void);
extern const  uint8_t Rcolors[WS2812ColorCount] PROGMEM;
extern const  uint8_t Gcolors[WS2812ColorCount] PROGMEM;
extern const  uint8_t Bcolors[WS2812ColorCount] PROGMEM;
/////////////////////////////////////////////////////////////
#define w_zeropulse   350
#define w_onepulse    900
#define w_totalperiod 1250

// Fixed cycles used by the inner loop
#define w_fixedlow    2
#define w_fixedhigh   4
#define w_fixedtotal  8

// Insert NOPs to match the timing, if possible
#define w_zerocycles    (((F_CPU/1000)*w_zeropulse          )/1000000)
#define w_onecycles     (((F_CPU/1000)*w_onepulse    +500000)/1000000)
#define w_totalcycles   (((F_CPU/1000)*w_totalperiod +500000)/1000000)

// w1 - nops between rising edge and falling edge - low
#define w1 (w_zerocycles-w_fixedlow)
// w2   nops between fe low and fe high
#define w2 (w_onecycles-w_fixedhigh-w1)
// w3   nops to complete loop
#define w3 (w_totalcycles-w_fixedtotal-w1-w2)

#if w1>0
#define w1_nops w1
#else
#define w1_nops  0
#endif

#define w_lowtime ((w1_nops+w_fixedlow)*1000000)/(F_CPU/1000)
#if w_lowtime>550
#error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
#elif w_lowtime>450
#warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
#warning "Please consider a higher clockspeed, if possible"
#endif

#if w2>0
#define w2_nops w2
#else
#define w2_nops  0
#endif

#if w3>0
#define w3_nops w3
#else
#define w3_nops  0
#endif

#define w_nop1  "nop      \n\t"
#define w_nop2  "rjmp .+0 \n\t"
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

static inline void ledcontrol_led_sendbyte
(const uint8_t data, const uint8_t masklo,const uint8_t maskhi)
{
	uint8_t ctr;

	asm volatile("       ldi   %0,8  \n\t"
	"loop%=:            \n\t"
	"       out   %2,%3 \n\t"
	#if (w1_nops & 1)
	w_nop1
	#endif
	#if (w1_nops & 2)
	w_nop2
	#endif
	#if (w1_nops & 4)
	w_nop4
	#endif
	#if (w1_nops & 8)
	w_nop8
	#endif
	#if (w1_nops & 16)
	w_nop16
	#endif
	"       sbrs  %1,7  \n\t"
	"       out   %2,%4 \n\t"
	"       lsl   %1    \n\t"
	#if (w2_nops & 1)
	w_nop1
	#endif
	#if (w2_nops & 2)
	w_nop2
	#endif
	#if (w2_nops & 4)
	w_nop4
	#endif
	#if (w2_nops & 8)
	w_nop8
	#endif
	#if (w2_nops & 16)
	w_nop16
	#endif
	"       out   %2,%4 \n\t"
	#if (w3_nops & 1)
	w_nop1
	#endif
	#if (w3_nops & 2)
	w_nop2
	#endif
	#if (w3_nops & 4)
	w_nop4
	#endif
	#if (w3_nops & 8)
	w_nop8
	#endif
	#if (w3_nops & 16)
	w_nop16
	#endif

	"       dec   %0    \n\t"
	"       brne  loop%=\n\t"
	: "=&d"(ctr)
	: "r"(data), "I"(_SFR_IO_ADDR(WS2812_PORT)), "r"(maskhi),
	"r"(masklo));
}

#endif
