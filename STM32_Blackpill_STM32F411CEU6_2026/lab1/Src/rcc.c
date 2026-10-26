void rcc_init(uint8_t clockSource){
    switch(clockSource){
		case HSI:
			RCC->CR |= RCC_CR_HSION;
			// дождаться стабилизации тактирующего сигнала
			while (!(RCC->CR & RCC_CR_HSIRDY)) {
			__NOP(); // «пустая» инструкция
			}
			// выбрать HSE в качестве SYSCLK
			RCC->CFGR |= RCC_CFGR_SW_HSI;
			break;
		case HSE:
			RCC->CFGR = 0;
			RCC->CR = 0;
			RCC->CR |= (1 << RCC_CR_HSEON_Pos); // 25 Мгц
			while (!(RCC->CR & (1 << RCC_CR_HSERDY_Pos))) {
				__NOP();
			}
			RCC->CFGR |= (RCC_CFGR_SW_HSE << RCC_CFGR_SW_Pos);
			break;
		case PLL:
			RCC->CFGR = 0;
			RCC->CR = 0;
			RCC->PLLCFGR = 0;
			RCC->CR |= (1 << RCC_CR_HSEON_Pos); // 25 Мгц
			while (!(RCC->CR & (1 << RCC_CR_HSERDY_Pos))) {
				__NOP();
			}

			RCC->CR &= ~(1 << RCC_CR_PLLON_Pos);
			RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLSRC_Pos) |
					(25UL << RCC_PLLCFGR_PLLM_Pos) |
					(336UL << RCC_PLLCFGR_PLLN_Pos) |
						(1 << RCC_PLLCFGR_PLLP_Pos);
			RCC->CR |= (1 << RCC_CR_PLLON_Pos);
			while (!(RCC->CR & (1 << RCC_CR_PLLRDY_Pos))){
				__NOP();
			}
			do {
				FLASH -> ACR |= 2;
			} while ((FLASH -> ACR & FLASH_ACR_LATENCY) != 2);

			RCC->CFGR |= RCC_CFGR_PPRE1 | (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
			break;
	}
}