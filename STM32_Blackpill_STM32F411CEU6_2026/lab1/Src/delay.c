// TODO delay_init(clockSource)
void delay_init(uint8_t clockSource){
	uint32_t freq;
	switch (clockSource){
		case HSE:
			freq = 25000000UL;
			break;
		case HSI:
			freq = 15700000UL;
			break;
		case PLL:
			freq = 84000000UL;
			break;
	}
	SysTick->CTRL = 0;
	SysTick->LOAD = (freq / 1000UL) - 1; // (0.001 / (1 / частота) = частота * 0.001
	SysTick->VAL = 0;
	SysTick->CTRL = (1 << SysTick_CTRL_CLKSOURCE_Pos) | (1 << SysTick_CTRL_ENABLE_Pos);
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1 << SysTick_CTRL_COUNTFLAG_Pos))) {
            __NOP();
        }
    }
}