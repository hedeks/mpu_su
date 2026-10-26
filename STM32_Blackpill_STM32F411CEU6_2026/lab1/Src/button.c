void button_init(){
    // button init
	// настроить PB12 как входную ногу МК
	GPIOB->MODER &= ~GPIO_MODER_MODE12;
	// на PB12 включить внутренний pull-up
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR12_0;
	// включить тактирование SysCfg
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// разрешить прерывания порта B
	SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI12_PB;
	// разрешить прерывание на выводе 12
	EXTI->IMR |= EXTI_IMR_MR12;
	// настроить прерывание на срез
	EXTI->FTSR |= EXTI_FTSR_TR12;
	// разрешить прерывания в NVIC, имя лежит в хедере stm32f401xc.h
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}