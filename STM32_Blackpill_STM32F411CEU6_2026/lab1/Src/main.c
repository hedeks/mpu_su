#include <stdint.h>
#include "stm32f401xc.h"
#include "leds.c"
#include "delay.c"
#include "button.c"
#include "rcc.c"
#define HSI 0
#define HSE 1
#define PLL 2

volatile uint8_t direction = 1; // 0 вниз 1 вверх

void EXTI15_10_IRQHandler(void) {
	// сбросить флаг Pending события
	EXTI->PR = EXTI_PR_PR12;
	// запретить прерывание на выводе 12
	EXTI->IMR &= ~EXTI_IMR_MR12;
	// TODO
	direction = !direction;
	// разрешить прерывание на выводе 12
	EXTI->IMR |= EXTI_IMR_MR12;
}

int main(void) {
	uint32_t current_led = 1;
	uint8_t clockSource = HSI;

	rcc_init(clockSource);
	leds_init();
	delay_init(clockSource);
	button_init();


	for (;;) {
		led_on(current_led);
		current_led = next_led(current_led, direction);
		delay_ms(1000);
		leds_off();
	}
}
