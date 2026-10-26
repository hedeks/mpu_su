void led_on(uint32_t numberVDn){
	switch (numberVDn){
		case 1:
			GPIOB->BSRR |= GPIO_BSRR_BS0;
			break;
		case 2:
			GPIOA->BSRR |= GPIO_BSRR_BS7;
			break;
		case 3:
			GPIOA->BSRR |= GPIO_BSRR_BS6;
			break;
		case 4:
			GPIOA->BSRR |= GPIO_BSRR_BS5;
			break;
		case 5:
			GPIOA->BSRR |= GPIO_BSRR_BS4;
			break;
		case 6:
			GPIOA->BSRR |= GPIO_BSRR_BS3;
			break;
		case 7:
			GPIOA->BSRR |= GPIO_BSRR_BS2;
			break;
		default:
			break;
	}
}

void leds_off(){
	GPIOB->BSRR |= GPIO_BSRR_BR0;
	GPIOA->BSRR |= GPIO_BSRR_BR7;
	GPIOA->BSRR |= GPIO_BSRR_BR6;
	GPIOA->BSRR |= GPIO_BSRR_BR5;
	GPIOA->BSRR |= GPIO_BSRR_BR4;
	GPIOA->BSRR |= GPIO_BSRR_BR3;
	GPIOA->BSRR |= GPIO_BSRR_BR2;
}

uint32_t next_led (uint32_t current_led, uint32_t direction){
	if (direction == 0){
		if (current_led <= 1){
			current_led = 7;
		} else {
			current_led--;
		}
	} else if (direction >= 1){
		if (current_led == 7){
			current_led = 1;
		} else {
			current_led++;
		}
	}
	return current_led;
}

void leds_init(){
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOBEN_Pos) | (1 << RCC_AHB1ENR_GPIOAEN_Pos);
		GPIOB->MODER |= (1 << GPIO_MODER_MODE0_Pos);
		GPIOA->MODER |= (1 << GPIO_MODER_MODE2_Pos) | (1 << GPIO_MODER_MODE3_Pos) | (1 << GPIO_MODER_MODE4_Pos)
				| (1 << GPIO_MODER_MODE5_Pos)| (1 << GPIO_MODER_MODE6_Pos) | (1 << GPIO_MODER_MODE7_Pos);
}
