#include <stdint.h>
#include "stm32f401xc.h"

#define HSI 0
#define HSE 1
#define PLL 2

volatile uint32_t current_led = 1;
volatile uint32_t direction = 1;
volatile uint16_t ccr = 50;

void led_shim_on (uint32_t numberVDn, uint8_t ccrValue){
	TIM3->CR1 &= ~TIM_CR1_CEN;
	switch (numberVDn){
			case 1:
				TIM3->CCR3 = ccrValue; // PB0
				break;
			case 2:
				TIM3->CCR2 = ccrValue; // PA7
				break;
			case 3:
				TIM3->CCR1 = ccrValue; // PA6
				break;
		}
	TIM3->CR1 |= TIM_CR1_CEN;
}


void leds_off(){
	// запуск таймера в режим счёта
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->CCR1 = 0; // PA6
	TIM3->CCR2 = 0; // PA7
	TIM3->CCR3 = 0; // PB0
	TIM3->CR1 |= TIM_CR1_CEN;
}

uint32_t next_led (){
	if (direction == 0){
		if (current_led <= 1){
			current_led = 3;
		} else {
			current_led--;
		}
	} else if (direction >= 1){
		if (current_led == 3){
			current_led = 1;
		} else {
			current_led++;
		}
	}
	return current_led;
}

void leds_init(){
	// настройка тактирования
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOBEN_Pos)
			| (1 << RCC_AHB1ENR_GPIOAEN_Pos);

	// настройка PA6 PA7 PB0 на альтернативную функцию
	GPIOB->MODER |= GPIO_MODER_MODE0_1;
	GPIOA->MODER |= GPIO_MODER_MODE6_1
			| GPIO_MODER_MODE7_1;

	// выбор альтернативных функцих AF2 для PA6 PA7 PB0
	GPIOA->AFR[0] |= (2 << GPIO_AFRL_AFSEL6_Pos)
			| (2 << GPIO_AFRL_AFSEL7_Pos);
	GPIOB->AFR[0] |= (2 << GPIO_AFRL_AFSEL0_Pos);

}


void EXTI15_10_IRQHandler(void) {
	// сбросить флаг Pending события
	EXTI->PR = EXTI_PR_PR12;
	// запретить прерывание на выводе 12
	EXTI->IMR &= ~EXTI_IMR_MR12;
	// TODO
	//direction = !direction;
	// запуск таймера в режим счёта
	TIM4->CR1 |= TIM_CR1_CEN;
	// разрешить прерывание на выводе 12
	EXTI->IMR |= EXTI_IMR_MR12;

}

void TIM4_IRQHandler(void) { // гашение дребезга
	TIM4->CR1 &= ~TIM_CR1_CEN; // остановить таймер
	// проверка флага произошедшего события
	if (TIM4->SR & TIM_SR_UIF) {
		// очистка флага события
		TIM4->SR &= ~TIM_SR_UIF;
		if(!(GPIOB->IDR & GPIO_IDR_ID12)){
			leds_off();
			current_led = next_led();
			led_shim_on(current_led, ccr);
		}
	}
}

uint16_t adcReadValue(void) {
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while (!(ADC1->SR & ADC_SR_EOC)) {
		__NOP();
	}
	ADC1->SR &= ~ADC_SR_EOC;
	return ADC1->DR;
}

int main(void) {
	current_led = 1;
	uint8_t clockSource = HSI;

	switch(clockSource){
		case HSI:
			RCC->CFGR = 0;
			RCC->CR = 0;
			RCC->CR |= (1 << RCC_CR_HSION_Pos); // 16 Мгц
			while (!(RCC->CR & (1 << RCC_CR_HSIRDY_Pos))) {
				__NOP();
			}
			RCC->CFGR |= (RCC_CFGR_SW_HSI << RCC_CFGR_SW_Pos);

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

	leds_init();
	delay_init(clockSource);

	// включение тактирования таймера №3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 1000 - 1; // частота счёта таймера F_CPU / 1000
	TIM3->ARR = 250; // период таймера 10 мс35

	// включение ШИМ режима на канале 1 таймера №3
	TIM3->CCER |= TIM_CCER_CC1E
			| TIM_CCER_CC2E
			| TIM_CCER_CC3E;

	// настройка режима ШИМ
	TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos)
			| (6 << TIM_CCMR1_OC2M_Pos);
	TIM3->CCMR2	|= (6 << TIM_CCMR2_OC3M_Pos);

	// запуск таймера в режим счёта
	TIM3->CR1 |= TIM_CR1_CEN;

	leds_off();

	// вывод PA0 является каналом ADC1_IN0
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// настраиваем PA0 на аналоговый режим
	GPIOA->MODER |= GPIO_MODER_MODE1;
	// включить модуль АЦП
	ADC1->CR2 |= ADC_CR2_ADON;
	// положить канал ADC_IN0 в последовательность измерений
	ADC1->SQR3 = (1 << ADC_SQR3_SQ1_Pos);

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

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->PSC = (freq/1000UL) - 1; // частота счёта таймера 1 кГц
	TIM4->ARR = 20; // период счёта таймера 2 мс
	// событие обновления – только переполнение таймера
	TIM4->CR1 |= TIM_CR1_URS;
	// разрешить прерывания по переполнению (обновлению)
	NVIC_EnableIRQ(TIM4_IRQn); // разрешить прерывания в NVIC
	TIM4->DIER |= TIM_DIER_UIE;
	for (;;) {
		ccr = (uint32_t) 250 * (adcReadValue() / 4095.0f);
		led_shim_on(current_led, ccr);
	}
}
