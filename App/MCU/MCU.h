#ifndef __MCU_H__
#define	__MCU_H__

//#define __TEST_MFCUpdate__		0	// Test MFC Update
//#define __TEST_FRBUpdate__		0	// Test FRB Update
//#define __TEST_RSTUpdate__		0	// Test RST Update
//
//#define __USE_EDIDUpdate__		0	// Use EDID Update

#define MDIN3XX_RST_HIGH			GPIOC->BSRR |= GPIO_Pin_12	//PC12
#define MDIN3XX_RST_LOW				GPIOC->BRR |= GPIO_Pin_12

#define I2C_SCL_HIGH				GPIOB->BSRR |= GPIO_Pin_6 	//PB6
#define I2C_SCL_LOW 				GPIOB->BRR  |= GPIO_Pin_6

#define I2C_SDA_HIGH				GPIOB->BSRR = 0x00000080 	//PB7
#define I2C_SDA_LOW 				GPIOB->BRR  = 0x00000080
#define I2C_SDA_INPUT 				(GPIOB->IDR & 0x00000080)
#define I2C_SDA_INPUT_MODE		\
	GPIOB->CRL &= 0x0fffffff; 	\
	GPIOB->CRL |= 0x80000000
#define I2C_SDA_OUTPUT_MODE 	\
	GPIOB->CRL &= 0x0fffffff; 	\
	GPIOB->CRL |= 0x30000000

#define KEY_ROW0_HIGH				GPIOB->BSRR = 0x00000008 	//PB3 //ROW0
#define KEY_ROW0_LOW				GPIOB->BRR  = 0x00000008
#define KEY_ROW1_HIGH				GPIOB->BSRR = 0x00000010 	//PB4 //ROW1
#define KEY_ROW1_LOW				GPIOB->BRR  = 0x00000010
#define KEY_EN_HIZ_MODE 			GPIOB->CRL  = 0x33344333 	//Change PB3, PB4 to Floating input in Input mode
#define KEY_ROWS_OUT_MODE 			GPIOB->CRL  = 0x33333333 	//Change PB3, PB4 to Output mode

#define KEY_LED0_HIGH				GPIOB->BSRR = 0x00000100 	//PB8 //LED0
#define KEY_LED0_LOW				GPIOB->BRR  = 0x00000100
#define KEY_LED1_HIGH				GPIOB->BSRR = 0x00000200 	//PB9
#define KEY_LED1_LOW				GPIOB->BRR  = 0x00000200

#define KEY_LED1_5_HIGH				GPIOB->BSRR = 0x00001000
#define KEY_LED1_5_LOW				GPIOB->BRR  = 0x00001000
#define KEY_LED2_6_HIGH				GPIOB->BSRR = 0x00002000
#define KEY_LED2_6_LOW				GPIOB->BRR  = 0x00002000
#define KEY_LED3_7_HIGH				GPIOB->BSRR = 0x00004000
#define KEY_LED3_7_LOW				GPIOB->BRR  = 0x00004000
#define KEY_LED4_HIGH				GPIOB->BSRR = 0x00008000
#define KEY_LED4_LOW				GPIOB->BRR  = 0x00008000

#define KEY_DATA_OUTPUT_MODE 		GPIOB->CRH = 0x33334b33 	//Change PB12, PB13, PB14, PB15 to Output mode
#define KEY_DATA1_5_INPUT 			((GPIOB->IDR >> 12) & 0x00000001)	//GPIOB12 status read
#define KEY_DATA2_6_INPUT 			((GPIOB->IDR >> 13) & 0x00000001)	//GPIOB13 status read
#define KEY_DATA3_7_INPUT 			((GPIOB->IDR >> 14) & 0x00000001) 	//GPIOB14 status read
#define KEY_DATA4_INPUT 			((GPIOB->IDR >> 15) & 0x00000001) 	//GPIOB15 status read
#define KEY_DATA_INPUT_MODE  		GPIOB->CRH = 0x88884b33 	//Change PB12, PB13, PB14, PB15 to Input mode

#define BUZZER_HIGH					GPIOB->BSRR = 0x00000004 	//PB2
#define BUZZER_LOW					GPIOB->BRR  = 0x00000004

#define ALARMOUT_HIGH				GPIOB->BSRR = GPIO_Pin_1
#define ALARMOUT_LOW 				GPIOB->BRR  = GPIO_Pin_1

#define SPI_CS_HIGH					GPIOC->BSRR = GPIO_Pin_3
#define SPI_CS_LOW					GPIOC->BRR  = GPIO_Pin_3
#define SPI_CLK_HIGH				GPIOC->BSRR = GPIO_Pin_2
#define SPI_CLK_LOW					GPIOC->BRR  = GPIO_Pin_2
#define SPI_MISO_DATA				(GPIOC->IDR & 0x00000001)

#define MDIN3xx_CS_HIGH				GPIOB->BSRR = 0x00000020 	//PB5
#define MDIN3xx_CS_LOW 				GPIOB->BRR  = 0x00000020
#define MDIN3xx_ALE_HIGH			GPIOC->BSRR = 0x00000080 	//PC7
#define MDIN3xx_ALE_LOW 			GPIOC->BRR  = 0x00000080

#define MDIN3xx_WR_HIGH				GPIOA->BSRR = 0x80000000 	//PA15
#define MDIN3xx_WR_LOW 				GPIOA->BRR  = 0x80000000
#define MDIN3xx_RD_STROBE_HIGH		MDIN3xx_WR_HIGH
#define MDIN3xx_RD_STROBE_LOW 		MDIN3xx_WR_LOW

#define MDIN3xx_DS_HIGH				GPIOA->BSRR = 0x10000000 	//PA12
#define MDIN3xx_DS_LOW 				GPIOA->BRR  = 0x10000000
#define MDIN3xx_WR_STROBE_HIGH		MDIN3xx_DS_HIGH
#define MDIN3xx_WR_STROBE_LOW		MDIN3xx_DS_LOW

#define MDIN3xx_ADDR_DATA_OUTPUT(x) 	((GPIOA->ODR) = (x))
#define MDIN3xx_ADDR_DATA_OUTPUT_MODE 	GPIOA->CRH = 0x33333333; GPIOA->CRL = 0x33333333
#define MDIN3xx_ADDR_DATA_INPUT 		GPIOA->IDR //(GPIOA->IDR & 0x0000ffff)
#define MDIN3xx_ADDR_DATA_INPUT_MODE  	GPIOA->CRH = 0x44444444; GPIOA->CRL = 0x44444444 //floating


extern BYTE EEP_buf[2048];

extern void MCU_init(void);			
extern void USART3_Init(void);
extern int fputc(int ch, FILE *f);
extern void IRQ_Init(void);
extern void RTC_Configuration(void);

extern void write_eeprom_all(void);
extern void read_eeprom_all(void);

#endif
