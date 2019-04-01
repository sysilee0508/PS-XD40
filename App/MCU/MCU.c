#include "common.h"

//-----------------------------------------------------------------------------
//	MCU Initialize (STM32F100x)  MAX 64MHz
//-----------------------------------------------------------------------------
void MCU_init(void)			
{
	/* activate HSI (Internal 8MHz High Speed oscillator) */
	RCC->CR |= 0x00000001;		//HSI on (Use Internal 8MHz clock)
	RCC->CFGR = 0x00000000;		//system clock = HSI
	while((RCC->CFGR & 0x0000000C) != 0); //HSI oscillator used as system clock

	RCC->CR &= 0xFEFFFFFF;		//PLL off
	FLASH->ACR |= 0x00000002;	//Set Latency (In case of  72MHz, set 0x00000002)

	RCC->CFGR = 0x00388400;		//PLL setting(PLLSRC=HSI/2, PLLMUL=16 Total=64MHz)
	RCC->CR |= 0x01000000;		//PLL on
	while((RCC->CR & 0x02000000) == 0); //PLL clock ready

	RCC->CFGR |= 0x00000002;	//system clock=PLL(PCLK1=32MHz, PCLK2=64MHz)
	while((RCC->CFGR & 0x0000000C) != 0x00000008); //PLL used as system clock
	//-------------------------------------------------------------------------

	//Clock Enable  (Remap �ϱ� ���� ���� �����ؾ� Remap�� ���������� �۵��Ѵ�)  
	RCC->APB1ENR |= RCC_APB1Periph_USART3; 
	RCC->APB1ENR |= RCC_APB1Periph_TIM2;
	RCC->APB1ENR |= RCC_APB1Periph_TIM3;
	RCC->APB2ENR |= RCC_APB2Periph_GPIOA; 
	RCC->APB2ENR |= RCC_APB2Periph_GPIOB; 
	RCC->APB2ENR |= RCC_APB2Periph_GPIOC; 
	RCC->APB2ENR |= RCC_APB2Periph_GPIOD; 
	RCC->APB2ENR |= RCC_APB2Periph_AFIO; 

	//Alternative function remap
	// SW-DG Enable (JTAG Disable)
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //PA13,14 REmap
	// Using HSE Oscillator
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);    		 //PD0,1 REmap

//	[GPIO A]
//	 0 : H_AD0
//	 1 : H_AD1
//	 2 : H_AD2
//	 3 : H_AD3
//	 4 : H_AD4
//	 5 : H_AD5
//	 6 : H_AD6
//	 7 : H_AD7
//	 8 : H_AD8
//	 9 : H_AD9
//	10 : H_AD10
//	11 : H_AD11
//	12 : H_RDB // MDIN Read Strobe (active low)
//	13 : SWDIO
//	14 : SWCLK
//	15 : H_WRB //MDIN Write strobe (active low)
	GPIOA->CRH = 0x33333333;			//Configure all PAs as OUTPUT why?
	GPIOA->CRL = 0x33333333;
	GPIOA->ODR = 0x0000FFFF;			//Initialize PAs to High

//	[GPIO B]
//    0 : NC
//	1 : ALM_OUT
//	2 : BUX_OUT
//	3 : K_ROW0 (active low)
//	4 : K_ROW1 (active low)
//	5 : I2C_SCL_M (active low)	/I2C_M clock
//	6 : I2C_SCL (active low) /I2C clock
//	7 : I2C_SDA (active low) /I2C data (shared)
//	8 : K_LED_R0 (active high)
//	9 : K_LED_R1 (active high)
//	10: TXD(UART3)
//	11: RXD(UART3)
//	12: K_CL0_MISO
//	13: K_CL1_MOSI
//	14: K_CL2_CLK
//	15: K_CL3_KCS
	GPIOB->CRH = 0x33334B33;		//PB10 uart_TX(alternate function), GPIO11-uart_RX
	GPIOB->CRL = 0x33377333; 		//PB3,4 --> output/open-drain //PB6,7 --> alternative/push-pull
	GPIOB->ODR = 0x0000FCFB;		//GPIOB02(BUZ_OUT),GPIOB8(LED0),GPIOB9(LED1)to Low.

//	[GPIO C]
//  0 : SPI_MISO
//	1 : SPI_MOSI
//	2 : SPI_CLK
//	3 : SPI_CS
//	4 : NC
//	5 : NC
//	6 : NC
//	7 : H_ALE //MDIN HOST_ALE : host address latch enable (active high)
//	8 : NC
//	9 : NC
//	10: NC
//	11: NC
//	12: nRST
//	13: INT (TAMPER_RTC)
//	14: OSC32I
//	15: OSC32O
	GPIOC->CRH = 0x33833333;			//GPIOC13 is used for TAMPER-RTC INT
	GPIOC->CRL = 0x33333338;			//PC0 : input(pull-up)
	GPIOC->ODR = 0x0000FFFF;			//spi_clk & spi_cs is low (active high)

//	[GPIO D]
//  0 : OSCI
//	1 : OSCO
//	2 : NC
	GPIOD->CRH = 0x33333333;			//Only PD0 and PD1 is used for OSCI and OSCO. They are already remapped.
	GPIOD->CRL = 0x33333333;
	GPIOD->ODR = 0x0000ffff;			//

	Delay_ms(200); // why we need delay 200ms here?
}

//-----------------------------------------------------------------------------
//  IThe IAR compiler uses the fputc function when using the printf function.
//-----------------------------------------------------------------------------
//int fputc(int ch, FILE *f)
//{
//    //Use USART3 as debug port
//    //Write a character to the USART3
//
//    if(ch == '\n')
//    {
//        USART_SendData(USART3, '\r');
//        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//        USART_SendData(USART3, '\n');
//    }
//    else
//    {
//        USART_SendData(USART3, (uint8_t)ch);
//    }
//
//    //Loop until the end of transmission
//    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//
//    return ch;
//}

//-----------------------------------------------------------------------------
//	Interrupt initialize
//-----------------------------------------------------------------------------
void IRQ_Init(void)		
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	// set vector table from 0x08000000
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitTypeDef NVIC_InitStructure;		// initialize interrupt

	// Timer2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Timer3
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//UART3 interrupt -------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// RTC -------------------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
