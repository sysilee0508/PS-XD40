#include "..\main\common.h"

/* Private function prototypes -----------------------------------------------*/

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
//	5 : MDIN_CE (active low)
//	6 : I2C_SCL (active low) /I2C1
//	7 : I2C_SDA (active low) /I2C1
//	8 : K_LED_R0 (active high)
//	9 : K_LED_R1 (active high)
//	10: TXD(UART3)
//	11: RXD(UART3)
//	12: K_CL0_MISO
//	13: K_CL1_MOSI
//	14: K_CL2_CLK
//	15: K_CL3_KCS
    GPIOB->CRH = 0x33334B33;			//PB10 uart_TX(alternate function), GPIO11-uart_RX
    GPIOB->CRL = 0x33377333; //GPIOB->CRL = 0xBB377333;			//PB3,4 --> output/open-drain //PB6,7 --> alternative/push-pull
    GPIOB->ODR = 0x0000FCFB;			//GPIOB02(BUZ_OUT),GPIOB8(LED0),GPIOB9(LED1)to Low.

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
    GPIOC->ODR = 0x0000fff3;			//spi_clk & spi_cs is low (active high)

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
//	USART3 Initialize (Debeg serial)
//-----------------------------------------------------------------------------
#define CR1_UE_Set                ((uint16_t)0x2000)  //!< USART Enable Mask 
#define CR1_UE_Reset              ((uint16_t)0xDFFF)  //!< USART Disable Mask 

void USART3_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    //USARTx configuration
    //USARTx configured as follow:
    //-Baudrate = 115200
    //-Word Length = 8bits
    //-One stop bit
    //-No parity
    //-Flow control None.
    //-Receive and transmit enabled

//    USART_InitStructure.USART_BaudRate      = 115200;
	if(sys_env.baud_rate == 0) USART_InitStructure.USART_BaudRate = 1200;
	else if(sys_env.baud_rate == 1) USART_InitStructure.USART_BaudRate = 2400;
	else if(sys_env.baud_rate == 2) USART_InitStructure.USART_BaudRate = 4800;
	else if(sys_env.baud_rate == 3) USART_InitStructure.USART_BaudRate = 9600;
	else if(sys_env.baud_rate == 4) USART_InitStructure.USART_BaudRate = 19200;

    USART_InitStructure.USART_WordLength    = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits      = USART_StopBits_1;
    USART_InitStructure.USART_Parity        = USART_Parity_No; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode          = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //Enable the USART3
    USART3->CR1 |= CR1_UE_Set;
}


//-----------------------------------------------------------------------------
//  IThe IAR compiler uses the fputc function when using the printf function.
//-----------------------------------------------------------------------------
int fputc(int ch, FILE *f)
{
    //Use USART3 as debug port
    //Write a character to the USART3

    if(ch == '\n')
    {
        USART_SendData(USART3, '\r');
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        USART_SendData(USART3, '\n');
    }
    else 
    {
        USART_SendData(USART3, (uint8_t)ch);
    }

    //Loop until the end of transmission
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

    return ch;
}

//-----------------------------------------------------------------------------
//	Interrupt initialize (timer interrupt) 
//-----------------------------------------------------------------------------
void IRQ_Init(void)		
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	// set vector table from 0x08000000
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitTypeDef NVIC_InitStructure;		// initialize interrupt

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM2->CR1 = 0x0005;				// up-counter enable
	TIM2->PSC = 639;				
	//TIM2->ARR = 9;					// 64MHz/(1+639)/(1+9) = 10000Hz
	TIM2->ARR = 99;					// 64MHz/(1+639)/(1+99) = 1000Hz
	TIM2->SR = 0x0000;				// clear TIM2 interrupt flags
	TIM2->DIER = 0x0001;			// enable TIM2 update interrupt
	//Setting timer interrupt 1ms--------------------------------------------

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM3->CR1 = 0x0005;				// up-counter enable
	TIM3->PSC = 639;	
	TIM3->ARR = 999;					// 64MHz/(1+639)/(1+1999) = 50Hz  --> 20ms
	TIM3->SR = 0x0000;				// clear TIM3 interrupt flags
	TIM3->DIER = 0x0001;			// enable TIM3 update interrupt
	//Setting timer interrupt 20ms--------------------------------------------

	//Setting UART3 interrupt -------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Setting UART3 interrupt  -------------------------------------------------

	//Setting RTC -------------------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//Setting RTC -------------------------------------------------------------
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RTC_Configuration(void)
{
	// Enable PWR and BKP clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	// Allow access to BKP Domain
	PWR_BackupAccessCmd(ENABLE);

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)	
	{
		// Reset Backup Domain
		BKP_DeInit();		//RTC related register reset (RTC related registers are not reset in system reset)
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}


	// Enable LSE 
	RCC_LSEConfig(RCC_LSE_ON);

	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	// Select the RTC Clock Source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	// Enable the RTC Clock
	RCC_RTCCLKCmd(ENABLE);

	// RTC configuration 
	// Wait for RTC APB registers synchronisation
	RTC_WaitForSynchro();
	// Wait until last write operation on RTC registers has finished
	RTC_WaitForLastTask();

	// Set the RTC time base to 1s
	RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
	// Wait until last write operation on RTC registers has finished
	RTC_WaitForLastTask();

	// Enable the RTC Second 
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	// Wait until last write operation on RTC registers has finished 
	RTC_WaitForLastTask();
}


//-----------------------------------------------------------------------------------
// Flash Programming (like EEPROM)
//-----------------------------------------------------------------------------------
#define ProgramTimeout        ((uint32_t)0x00002000)
#define	EEPROM_ADDR	0x0803F800 //256K 
BYTE EEP_buf[2048];

void write_eeprom_all(void)
{
	DWORD *pEEP_buf;
	WORD i;

	pEEP_buf = (DWORD *)EEP_buf;

	FLASH_Unlock(); 

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP| FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	FLASH_ErasePage(EEPROM_ADDR);                                    

	for(i = 0; i < 512; i++)
	{
		FLASH_ProgramWord(EEPROM_ADDR+(i*4),pEEP_buf[i]);
	}
}

void read_eeprom_all(void)
{
	vu32 Addr;
	DWORD *pEEP_buf;
	WORD i;
	
	pEEP_buf = (DWORD *)EEP_buf;

	Addr = EEPROM_ADDR;

	for(i = 0; i < 512; i++)
	{
		*pEEP_buf = *((vu32 *)Addr);
		Addr += 4;
		pEEP_buf++;
	}
}

