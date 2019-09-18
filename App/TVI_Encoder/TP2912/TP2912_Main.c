//****************************************************************/
/*                                                              							*/
/*                     HDTVI encocer & decoder              	              */
/*                                                              							*/
/****************************************************************/

//#include <stdio.h>
//#include <string.h>
#include "Config.h"
//#include "reg.h"
#include "TP2912_Typedefs.h"
#include "TP2912_main.h"
#include "TP2912_i2c.h"
//#include "printf.h"
#include "Device_Tx.h"
//#include "Device_Rx.h"
//#include "Table.c"
//#include "Coaxitron.h"
#include "TP2912.h"

//BOOL	I2CInitialBlocking = 1;
//BOOL	DetAutoRes = 0;
//BOOL	ManualEQ=OFF;
//BOOL 	MDIN_debug = 0;
BOOL 	AHD_nEN = 1;		

//DATA	BYTE	ManChSel;
//DATA	BYTE	IsChannel;

#ifdef TEST_SERCOM
BOOL		MaskEcho = 0;
DATA	BYTE	StoreArg1 = 0, StoreArg2 = 0;
DATA	BYTE	AddressOffset = 0;
#endif //TEST_SERCOM

//DATA	WORD    tm01=0;
//DATA    BYTE    tm02=0;
//DATA	BYTE    tic01=0;
//DATA	BYTE	tic_rs485=0;
//DATA	BYTE	mS_timer;
DATA	BYTE	I2CAddressDeb = TVII2CAddress;

//BOOL     TVI_ISR = 0;
//DATA	BYTE	Ex0_INT=0;

DATA	BYTE	ManVidRes;
//extern DATA	BYTE	TVITxID;
//extern DATA BYTE TPIAddr;

//extern DATA BYTE cam_ch;
//extern CODE PelcoD_cmd PelcoD_cmd_dataset[];

//===================== Serial ============================================
//#define BUF_MAX 	10
//static	DATA BYTE	RS_buf[BUF_MAX];
//static	DATA BYTE   RS_in=0, RS_out=0;
//BOOL	        RS_Xbusy=0;			// bit RS_Xbusy=0;

//================== Debug key input ======================================
//#define MaxBuf		30					 
//#define MaxArg		30					
//static  PDATA BYTE  commdbuf[MaxBuf];
//static  PDATA BYTE  commdptr;
//static  PDATA BYTE	  bptr;
//static  PDATA BYTE  arg[MaxArg], argn;	

//CODE_P	BYTE	*ptr_TVP7002;
//CODE_P	BYTE	*ptr_CDCE913;

//CODE_P	BYTE	*ptr_THS8200_720p;
//CODE_P	BYTE	*ptr_THS8200_1080i;
//CODE_P	BYTE	*ptr_THS8200_1080p30f;

/*****************************************************************************/
/*      Timer 0 Interrupt                                                    */
/*****************************************************************************/
#ifdef TP2912_MAIN
INTERRUPT(1, timer0_int)
{
		
	tm01++;						//9600Hz(104us)
	tm02++;
	tic_rs485++;

	#ifdef CLOCK_11M
	if( tm01 > 48 ) {			// 5msec
	#elif defined CLOCK_22M
	if( tm01 > 48*2 ) {		
	#endif
	
		tm01 = 0;
	    tic01++;
	}

	if(tm02>10){
		tm02=0;
		if(mS_timer) 
			mS_timer--;
	}
}

/*****************************************************************************/
/*      Timer 1 Interrupt                                                    */
/*****************************************************************************/
/*
INTERRUPT(5, timer2_int)
{
	ET2 = 0;		//disable interrupt timer2
	
	tic_rs485++;
	
	TF2 = 0; 		//clear interrupt flag
	ET2 = 1;		//enable interrupt timer2
}
*/
#endif

#ifdef TP2912_MAIN
//****************************************************************************/
//      Serial Interrupt                                                   
//****************************************************************************/
INTERRUPT(4, serial_int)
{
	if( RI ) {					//--- Receive interrupt ----
		RI = 0;
		RS_buf[RS_in++] = SBUF;
		if( RS_in>=BUF_MAX ) RS_in = 0;
	}

	if( TI ) {					//--- Transmit interrupt ----
		TI = 0;
		RS_Xbusy=0;
	}
}

//****************************************************************************/
//      INT0 Interrupt                                                   
//****************************************************************************/

INTERRUPT(0, ex0_isr)
{
	if(TVI_ISR)
		Ex0_INT = 1;
}


//=============================================================================
//		Serial RX Check 												   
//=============================================================================
BYTE RS_ready(void)
{
	if( RS_in == RS_out ) return 0;
	else return 1;
}
//=============================================================================
//		Serial RX														   
//=============================================================================
BYTE RS_rx(void)
{
	BYTE	ret;
		
	ES = 0;
	ret = RS_buf[RS_out];
	RS_out++;
	if(RS_out >= BUF_MAX) 
		RS_out = 0;
	ES = 1;

	return ret;
}
//=============================================================================
//		Serial TX														   
//=============================================================================
void RS_tx(BYTE tx_buf)
{
	while(1) {
		if(!RS_Xbusy) {
			SBUF = tx_buf;
			RS_Xbusy=1;
			break;
		}
	}
}
/*===========================================================================*/
/*        Initialize CPU                                                     */
/*===========================================================================*/
void InitCPU(void)
{
	/*----- Initialize interrupt -------------*/
						// Incase using Timer1 for serial
	TH1 = 0xff; 		//    SMOD = 0      SMOD =1						
						// 0ffh :57600 bps				
						// 0fdh : 9600 bps	0fdh :19200 bps				
						// 0fah : 4800 bps								
						// 0f4h : 2400 bps							
						// 0e8h : 1200 bps								


//	TH1 = 0xf0;
	SCON = 0x50;		// 0100 0000 mode 1 - 8 bit UART				
						// Enable serial reception						
    TMOD = 0x22;		// 0010 0010 timer 0 - 8 bit auto reload		
						// timer 1 - baud rate generator				
    TCON = 0x55;		// 0101 0001 timer 0,1 run						
						// int 0,  edge triggered						
						// int 1,  edge triggered						
						// TF1 TR1 TF0 TR0	EI1 IT1 EI0 IT0				
//	TH0 = TL0 = 64;		// 64=4608 Hz at 11.0592MHz
						// 64=4800Hz at 11.0592MHz (11.0592/12)/192=4800Hz. (255-64+1=192)
						
	TH0 = TL0 = 160;	// 160=9600Hz at 11.0592MHz (11.0592/12)/96=9600Hz. (255-160+1=96)
						
	PCON = 0x80;		// 0000 0000 SMOD(double baud rate bit) = 1		
	IP	 = 0x02;		// 0000 0000 interrupt priority					
						// -  - PT2 PS PT1 PX1 PT0 PX0	 		         
	IE	 = 0x92;		// 1001 0010 interrupt enable:Serial,TM0			
						// EA - ET2 ES ET1 EX1 ET0 EX0	
	
	EX0 = 1;			// Enable INT0

	TI	 = 1;			
	ES   = 1;
/*
//	Using Timer2 for serial
	T2CON = 0x34;		//TF2 EXF2 RCLK TCLK EXEN2 TR2 C/T2 CP/RL2
						//using Timer2 for serial interface
						//Enable Timer2
	RCAP2H = 0xff;		//Baud rate = 11.0596MHz/(32x(65536-(RCAP2H,RCAP2L)))
	RCAP2L = 0xfa;		// For 57600	
*/
	
/*
//	Using Timer2 for counter
	ET2 = 1;
	TR2 = 1;
	TH2 = 0xff;
	TL2 = 0xfb;

	RCAP2H = 0xff;
	RCAP2L = 0xfb;
*/	
	CHPENR = 0x87;		// Enable AUX RAM in Winbond(W78E516B)
	CHPENR = 0x59;		//
	CHPCON = 0x10;		//
	CHPENR = 0x00;		// Write Disable

}
/*===========================================================================*/
/*		New Line			                             */
/*===========================================================================*/
void NewLine(void)
{
 	Puts("\r\n");	
}

void PutsP(PDATA_P BYTE *ptr)
{
	BYTE ch;
	while(*ptr!='\0') {
		ch = *ptr++;
		RS_tx(ch);
	}
}

/*===========================================================================*/
/*		Prompt				                                                 */
/*===========================================================================*/
void Prompt(void)
{
	//Printf("\r\nI2C[%02x]>", (WORD)I2CAddressDeb);
}
/*===========================================================================*/
/*		I2C Edit Read		                                                 */
/*===========================================================================*/
void MonWriteI2C(BYTE addr, BYTE index, BYTE val) 
{
	//Printf("\r\nWrite %02xh to [Adrs(%02xh)Index(%02xh)]", (WORD)val, (WORD)addr, (WORD)index);
	TP2912_WriteI2C(addr, index, val);
}

/*===========================================================================*/
/*		I2C Edit Read		                                                 */
/*===========================================================================*/
BYTE MonReadI2C(BYTE addr, BYTE index)
{
	BYTE val;

	val = TP2912_ReadI2C(addr, index);
	//Printf("Read [Adrs:%02xh,%02xh] %02xh", (WORD)addr, (WORD)index, (WORD)val);

	return val;
}

/*void MonReadI2Cmn(BYTE addr, BYTE *wrdata, BYTE argcnt)
{
	BYTE i, rcnt;

	Printf("Read  [Adrs:%02xh ", (WORD)addr);
	for(i=0; i<argcnt-1; i++)
		Printf(" Index:%02xh ", wrdata[i] );
	Printf("] ");

	rcnt = wrdata[argcnt-1];
	ReadI2Cmn(addr, argcnt-1, rcnt, wrdata);

	for(i=0; i<rcnt; i++)
		Printf("%02xh ", (WORD)wrdata[i]);
}
*/
WORD MonReadI2C16(BYTE addr, BYTE indexH, BYTE indexL) 
{
	WORD val;

	val = TP2912_ReadI2C16(addr, indexH, indexL);

	//Printf("Read [Adrs%02xh, %02x%02xh] %04xh",(WORD)addr, (WORD)indexH,(WORD)indexL,(WORD)val );
	return val;

}

void MonWriteI2C16(BYTE addr, BYTE indexH, BYTE indexL, BYTE valH, BYTE valL) 
{
	//Printf("\r\nWrite %02x%02xh to [Adrs(%02xh)Index(%02x%02xh)]", (WORD)valH, (WORD)valL, (WORD)addr, (WORD)indexH, (WORD)indexL);
	WriteI2C16(addr, indexH, indexL, valH, valL);
}
#endif //#ifdef TP2912_MAIN


/*===========================================================================*/
/*		Convert ASCII to Binery                                              */
/*===========================================================================*/
BYTE Asc1Bin(BYTE asc)
{
	if(asc>='0' && asc <='9') return (asc - '0');
	else if(asc>='a' && asc <='f') return (asc - 'a' + 0x0a);
	else if(asc>='A' && asc <='F') return (asc - 'A' + 0x0a);
	else return 0xff;
}

BYTE Asc2Bin(PDATA_P BYTE *s)
{
	BYTE bin;
	BYTE bin0;

	bin = 0;
	while(*s != '\0' && *s !=' ') {
		bin = bin<<4;
		if((bin0=Asc1Bin(*s))==0xff)
			return 0xff;
		bin = bin + bin0;
		s++;
	}
	return (bin);
}

void I2CDeviceSet_( BYTE addr, CODE_P BYTE *RegSet)
{
	int	cnt=0;
	BYTE index, val;

	// Output discription
	#ifdef TASKING
	while( *RegSet ) 
		RS_tx( *RegSet++ );
	RegSet++;
	#endif

	while ( *RegSet != 0xFF ) {
/*		if( cnt==0 ) {
			cnt = *RegSet;
			RegSet+=1;
		}
*/
		index = *RegSet;
		val = *(RegSet+1);
//		MonWriteI2C(addr, index, val);
		TP2912_WriteI2C(addr, index, val);

//		Puts("\r\n");

		RegSet+=2;

		cnt--;
	}

	delay(50);
}

void I2CDeviceSet( CODE_P BYTE *RegSet)
{
	int	cnt=0;
	BYTE addr, index, val;

	// Output discription
	#ifdef TASKING
	while( *RegSet ) 
		RS_tx( *RegSet++ );
	RegSet++;
	#endif

	while ( *RegSet != 0xFF ) {
		if( cnt==0 ) {
			addr = *RegSet;

			cnt = *(RegSet+1);
			RegSet+=2;
		}
		index = *RegSet;
		val = *(RegSet+1);
//		MonWriteI2C(addr, index, val);
		TP2912_WriteI2C(addr, index, val);

//		Puts("\r\n");

		RegSet+=2;

		cnt--;
	}

	delay(50);
}


/*===========================================================================*/
/*		Help					                                             */
/*===========================================================================*/
#ifdef TP2912_MAIN
void Usage(void)
{
	delay(2);
	if(TVITxID==TP2915)
		//Printf("\r\n+-----   TP2915 board(Keil)-Rev%d.%02x   -----+", (WORD)TP2915FWVER/0x100, (WORD)TP2915FWVER%0x100);
	else if(TVITxID==TP2912B)
		//Printf("\r\n+-----   TP2912B board(Keil)-Rev%d.%02x   -----+", (WORD)TP2912FWVER/0x100, (WORD)TP2912FWVER%0x100);
	else if(TVITxID==TP2912)
		//Printf("\r\n+-----   TP2912 board(Keil)-Rev%d.%02x   -----+", (WORD)TP2912FWVER/0x100, (WORD)TP2912FWVER%0x100);
	else if(TVITxID==TP2910)
		//Printf("\r\n+-----   TP2910 board(Keil)-Rev%d.%02x   -----+", (WORD)TP2910FWVER/0x100, (WORD)TP2910FWVER%0x100);
	else if((TVITxID==TP2801A)||(TVITxID==TP2801B)||(TVITxID==TP2803))
		//Printf("\r\n+-----   TP2801 board(Keil)-Rev%d.%02x   -----+", (WORD)TP2801FWVER/0x100, (WORD)TP2801FWVER%0x100);
	else
		//Printf("\r\n+-----   TVI_Rx Chip is not detected   -----+");			
	
	Puts("\r\n| W Index Data   Write register                 |");	delay(1);
	Puts("\r\n| R Index        Read register                  |");	delay(1);
	Puts("\r\n| D Index Index  Dump registers                 |");	delay(1);	
	Puts("\r\n| C Address      Change I2C Address             |");	delay(1);	
	Puts("\r\n| < >            Register++,Register--          |");	delay(1);	
	Puts("\r\n| /              Repeat previous command        |");	delay(1);
	Puts("\r\n+-----------------------------------------------+");	delay(1);
}
#endif //#ifdef TP2912_MAIN

/*
BYTE Dec2Hex(BYTE val)
{
	BYTE j =1;
	BYTE dat=0,rem;

	while(val != 0){
		rem = val%16;
		val = val/16;
		dat = dat + (rem*j);
//		Printf("\r\nValue = %02d, remain = %02d, data = %02d",(WORD)val,(WORD)rem,(WORD)dat);
		j=j*10;
	}

//	Printf("\r\nLast Value = %02d",(WORD)dat);
	return dat;

}*/

#ifdef TP2912_MAIN
BYTE StringCommand()
{
	BYTE *cmds[MaxBuf]  ;
	BYTE cnt = 0;
	BYTE cmd_ptr=0;
	bit eol;
	BYTE i2c_dat;
	//BYTE PTZ_cmd;
	BYTE pan_spd=0;
	BYTE tilt_spd=0;
	BYTE cur_res;
	//PelcoD_cmd PD_cmdset, *PD_cmd, *PD_cmd_sum;
	
//	PD_cmd_sum = &PD_cmdset;
	cmds[cmd_ptr++] = &commdbuf[commdptr];							//first string command
	
	while(commdbuf[commdptr]!=' '&& commdbuf[commdptr] != '\0'){				
		commdptr ++;
		cnt++;
	}

	if(cnt <= 1) {													//one character command
		commdptr = 0;
		return 1;		
	}	

	if(commdbuf[commdptr] == '\0')									//one string command
		eol = 1;
	else{															//multi string command
		eol = 0;
    	commdbuf[commdptr++] = '\0';
	}

	while(!eol){													//subsidiarily string command

		if( commdbuf[commdptr]=='\0' ) {
				eol = 1; break;
		}
		else if(commdbuf[commdptr]==' ')
			commdbuf[commdptr++] = '\0';

		cmds[cmd_ptr++] = &commdbuf[commdptr];
		arg[++argn] = Asc2Bin(&commdbuf[commdptr]);

		while( commdbuf[commdptr]!=' ' && commdbuf[commdptr]!='\0' )
				commdptr++;
	}

	if((!strcmp(cmds[0],"I2C")) || (!strcmp(cmds[0],"i2c"))){
		//Printf("\r\nStarting I2C test...");

		if((!strcmp(cmds[1],"TEST")) || (!strcmp(cmds[1],"test"))){
			while(1){
				TP2912_WriteI2C(TVII2CAddress,0x44,0x07);
				TP2912_WriteI2C(TVII2CAddress,0x30,0x48);
				i2c_dat=TP2912_ReadI2C(TVII2CAddress,0x01);
				if(i2c_dat!=0x7e){
					//Printf("\r\nI2C error found");
					break;
				}
			}
			
		}
	}

	else if((!strcmp(cmds[0],"R16")) || (!strcmp(cmds[0],"r16"))){
		NewLine();
		arg[2] = MonReadI2C16(I2CAddressDeb, arg[1], arg[2]);
	}

	else if((!strcmp(cmds[0],"W16")) || (!strcmp(cmds[0],"w16"))){
		NewLine();
		MonWriteI2C16(I2CAddressDeb, arg[1], arg[2], arg[3], arg[4]);
	}
	else if((!strcmp(cmds[0],"ISR")) || (!strcmp(cmds[0],"isr"))){
		if((!strcmp(cmds[1],"0"))){
			//Printf("\r\nInterrupt Service disabled");
			TVI_ISR=0;
		}
		else if((!strcmp(cmds[1],"1"))){
			//Printf("\r\nInterrupt Service enabled");
			TVI_ISR=1;
			Clear_Interrupt();
		}
		
	}
	else if((!strcmp(cmds[0],"RXD")) || (!strcmp(cmds[0],"rxd"))){
		if((!strcmp(cmds[1],"ON"))||(!strcmp(cmds[1],"on"))){
			TVI_ISR=1;
			Init_UpData_Comm();
			//Printf("\r\nReady to PTZ RxD");
		}
		else if((!strcmp(cmds[1],"OFF"))||(!strcmp(cmds[1],"off"))){
			TVI_ISR=0;
			//Printf("\r\nDisabled PTZ RxD");
		}
	}
	else if((!strcmp(cmds[0],"NTSC"))||(!strcmp(cmds[0],"ntsc"))){
			ManVidRes=NTSC;
			Set_CDCE913_PLLIC();
			TVI_Init();
			if(!Test_Pat)	
			TP2912_WriteI2C(TPIAddr,0x02,(TP2912_ReadI2C(TPIAddr,0x02)|0x40));
	}
	else if((!strcmp(cmds[0],"PAL"))||(!strcmp(cmds[0],"pal"))){
			ManVidRes=PAL;
			Set_CDCE913_PLLIC();
			TVI_Init();
			if(!Test_Pat)	
			TP2912_WriteI2C(TPIAddr,0x02,(TP2912_ReadI2C(TPIAddr,0x02)|0x40));
	}
	else if((!strcmp(cmds[0],"TVI")) || (!strcmp(cmds[0],"tvi"))){
		cur_res=ManVidRes;
	
		if((!strcmp(cmds[1],"8M15"))||(!strcmp(cmds[1],"8m15")))
			ManVidRes=R8M15;
		else if((!strcmp(cmds[1],"8M125"))||(!strcmp(cmds[1],"8m125")))
			ManVidRes=R8M125;
		else if((!strcmp(cmds[1],"QHD30"))||(!strcmp(cmds[1],"qhd30")))
			ManVidRes=RQHD30;
		else if((!strcmp(cmds[1],"QHD25"))||(!strcmp(cmds[1],"qhd25")))
			ManVidRes=RQHD25;
		else if((!strcmp(cmds[1],"5M20"))||(!strcmp(cmds[1],"5m20")))
			ManVidRes=R5M20;
		else if((!strcmp(cmds[1],"1080P60"))||(!strcmp(cmds[1],"1080p60")))
			ManVidRes=R1080P60;
		else if((!strcmp(cmds[1],"1080P50"))||(!strcmp(cmds[1],"1080p50")))
			ManVidRes=R1080P50;
		else if((!strcmp(cmds[1],"1080P30"))||(!strcmp(cmds[1],"1080p30")))
			ManVidRes=R1080P30;
		else if((!strcmp(cmds[1],"1080P25"))||(!strcmp(cmds[1],"1080p25")))
			ManVidRes=R1080P25;
		else if((!strcmp(cmds[1],"720P60"))||(!strcmp(cmds[1],"720p60")))
			ManVidRes=R720P60;
		else if((!strcmp(cmds[1],"720P50"))||(!strcmp(cmds[1],"720p50")))
			ManVidRes=R720P50;
		else if((!strcmp(cmds[1],"720P30"))||(!strcmp(cmds[1],"720p30")))
			ManVidRes=R720P30_TVI20;
		else if((!strcmp(cmds[1],"720P25"))||(!strcmp(cmds[1],"720p25")))
			ManVidRes=R720P25_TVI20;
		else if((!strcmp(cmds[1],"1920X532"))||(!strcmp(cmds[1],"1920x532")))
			ManVidRes=NR1920x532;
		
		else{
			//Printf("\r\nNot supported...");
			//Printf("\r\nTyping... TVI 8M15,8M125,5M20,QHD30,QHD25,1080P30,1080P25,720P60,720P50,720P30 or 720P25");
			Prompt();
			commdptr = 0;
			commdbuf[0]='\0';
			return 0;
		}

		Set_CDCE913_PLLIC();
		Set_PLL();
		TVI_Init();
		Set_OutputMode();
		if(!Test_Pat)	
			TP2912_WriteI2C(TPIAddr,0x02,(TP2912_ReadI2C(TPIAddr,0x02)|0x40));
	}
	
	else if((!strcmp(cmds[0],"AHD")) || (!strcmp(cmds[0],"ahd"))){
		cur_res=ManVidRes;
	
		if((!strcmp(cmds[1],"QHD30"))||(!strcmp(cmds[1],"qhd30")))
			ManVidRes=AHD_QHD30;
		else if((!strcmp(cmds[1],"QHD25"))||(!strcmp(cmds[1],"qhd25")))
			ManVidRes=AHD_QHD25;
		else if((!strcmp(cmds[1],"1080P30"))||(!strcmp(cmds[1],"1080p30")))
			ManVidRes=AHD1080P30;
		else if((!strcmp(cmds[1],"1080P25"))||(!strcmp(cmds[1],"1080p25")))
			ManVidRes=AHD1080P25;
		else if((!strcmp(cmds[1],"720P30"))||(!strcmp(cmds[1],"720p30")))
			ManVidRes=AHD720P30;
		else if((!strcmp(cmds[1],"720P25"))||(!strcmp(cmds[1],"720p25")))
			ManVidRes=AHD720P25;
		else if((!strcmp(cmds[1],"720P30_36M"))||(!strcmp(cmds[1],"720p30_36m")))
			ManVidRes=AHD720P30_36M;
		else if((!strcmp(cmds[1],"720P25_36M"))||(!strcmp(cmds[1],"720p25_36m")))
			ManVidRes=AHD720P25_36M;
		else{
			//Printf("\r\nNot supported...");
			//Printf("\r\nTyping... AHD QHD30,QHD25,1080P30,1080P25,720P30 or 720P25");
			Prompt();
			commdptr = 0;
			commdbuf[0]='\0';
			return 0;
		}
//		if((ManVidRes&0x10)== 0x10){
			Set_CDCE913_PLLIC();
			Set_PLL();
			AHD_Init();
			Set_OutputMode();
			if(!Test_Pat)	
				TP2912_WriteI2C(TPIAddr,0x02,(TP2912_ReadI2C(TPIAddr,0x02)|0x40));
//		}
	}

	else if(!strcmp(cmds[0],"480")){
		
		TVP7002_480i();
	}

	else if(!strcmp(cmds[0],"720")){
		
		TVP7002_720P();
	}
	else
			Puts(" <- Command Error!!");
	

	
	Prompt();
	commdptr = 0;
	commdbuf[0]='\0';
	
	return 0;

}



/*===========================================================================*/
/*			          Debug Command Service Routine                          */
/*===========================================================================*/
void DebugKeyIn(BYTE ikey)
{
	BYTE i;

	#ifdef TEST_SERCOM
	#endif //TEST_SERCOM

	bit eol, cerror;
	static bit comment=0;

	//----- if end of loading -----

	//----- if comment, echo back and ignore -----
	if( comment ) {
		if( ikey=='\r' )
			comment = 0;
		else {
			RS_tx(ikey);
			return;
		}
	}
	else if( ikey==';' ) {
		comment = 1;
		RS_tx(ikey);
		return;
	}

	//----- Command Interpret ------------------------------

	if( (ikey=='\r') || (ikey=='/') ) {
		#ifdef TEST_SERCOM
		if(ikey=='\r')
			MaskEcho = 0;
		#endif //TEST_SERCOM
		comment = 0;

		if (ikey == '/') {      // Repeat 
			PutsP(commdbuf);
			commdptr = bptr;
			commdbuf[0]=arg[0];
		}

		commdbuf[commdptr] = '\0';
		
		bptr = commdptr;
		commdptr = 0;
		argn = 0;
		eol	= 0;
		cerror = 0;
		
		while(commdbuf[commdptr]==' ') commdptr++;		// Cut leading space
		if (commdbuf[commdptr]=='\0'){
  			eol = 1;									// Exit, if null command
		}
		
	//	if(!StringCommand()) return;

	//	arg[argn++] = commdbuf[commdptr++];				// Get command

		arg[argn] = commdbuf[commdptr];		//Get first character
		if(!StringCommand()) return;
		argn++; commdptr++;

		while( !cerror && !eol ) {
			if( commdbuf[commdptr]!=' ' && commdbuf[commdptr]!='\0' ) {
				cerror = 1; break;
			}

			while( commdbuf[commdptr]==' ' ) commdptr++;
			if( commdbuf[commdptr]=='\0' ) {
				eol = 1; break;
			}
			arg[argn++] = Asc2Bin(&commdbuf[commdptr]);
			while( commdbuf[commdptr]!=' ' && commdbuf[commdptr]!='\0' ) {
				commdptr++;
			}
		}

		if(I2CInitialBlocking){	
			cerror = 0;
			I2CInitialBlocking = 0;
		}

		if (cerror) {
			Puts(" <- Error!!");
			Prompt();
			commdptr = bptr = 0;
			return;
		}

		//----- Command ---------------------------

		
		switch(arg[0]) {
			
			case 'w':
			case 'W':
				if(argn == 3) {
//					NewLine();
					MonWriteI2C(I2CAddressDeb, arg[1], arg[2]);	
					delay(1);
					MonReadI2C(I2CAddressDeb, arg[1]);
					
				}
				
			
				break;
			
			case 'r':
			case 'R':
				if( argn==2 ) {
					NewLine();
					arg[2] = MonReadI2C(I2CAddressDeb, arg[1]);
				
				}
				else if( argn > 2 ) {	// Read several Byte with several index.
										// r cmd1 cmd2 ... rcnt
										// ex) r 10 20 5
										// read 5 bytes from index 1020.
										// Useful for Z86129(CC Decorder).
					NewLine();
	//				MonReadI2Cmn(I2CAddressDeb, &arg[1], argn-1);
				}
				break;
			
			case 'D':
			case 'd':
				if(argn == 1) {
					arg[1]+=8;
					arg[2] = arg[1]+7;
				}
				else if(argn == 2)
					arg[2] = arg[1]+7;
				for (i=arg[1]; i<=arg[2] && i<0xff; i++) {
					NewLine();
					MonReadI2C(I2CAddressDeb, i);	
				}
				break;
			
			case 'C':	/* Change I2C Address	*/
			case 'c':
				if(argn == 2)
					I2CAddressDeb = arg[1];
				//Printf("\r\nI2C address : %02x", (WORD)I2CAddressDeb);
				break;
		
			
			case 'H':
			case 'h':
			case '?':
				Usage();
				break;
			
			}
		
		Prompt();
		commdptr = 0;
		commdbuf[0]='\0';
	}

	//----- 1 byte command ----------------------------
	else {

			switch(ikey) {

			case 0x08:		// back space
				if(commdptr>0) {
					commdbuf[commdptr--]='\0';
					RS_tx(ikey);
					RS_tx(' ');
					RS_tx(ikey);
				}
				break;

			case 0x1B:		// Esc 
				Prompt();
				commdptr = 0;commdbuf[0]='\0';
				break;

			case ',':
			case '<':
				RS_tx('\r');
				MonWriteI2C(I2CAddressDeb, arg[1], --arg[2]);
				MonReadI2C(I2CAddressDeb, arg[1]);

				Prompt();
				commdptr = 0;commdbuf[0]='\0';
				break;

			case '.':
			case '>':
				RS_tx('\r');
				MonWriteI2C(I2CAddressDeb, arg[1], ++arg[2]);
				MonReadI2C(I2CAddressDeb, arg[1]);

				Prompt();
				commdptr = 0;commdbuf[0]='\0';
				break;



			default:
				#ifdef TEST_SERCOM

				if(ikey == '^' || ikey == '$')
 		    		MaskEcho = 1;

				if(!MaskEcho)
				#endif //TEST_SERCOM
				
				RS_tx(ikey);
				commdbuf[commdptr++] = ikey;
				break;
			}
		
	}

}


void InitReset(){
		int t = tm01;

		reset = 0;
		while(t == tm01);
		
//		delay(1);
		reset = 1;

}

void Main_Initialize()
{
	InitCPU();
	delay(10);
	InitReset();
//	Buff_nEN = 0;
#ifdef TP2915BD
	Buff_nEN_2915 = 0;
#else
	Buff_nEN = 0;
#endif
	commdptr = 0;
	NewLine();
	delay(10);
	InitRegisterSet();
	Usage();
	delay(10);
	Prompt();
}

/*===========================================================================*/
/*                             MAIN PROGRAM                                  */
/*===========================================================================*/

void main (void)
{

	Main_Initialize();
	
	while(1) 
	{	
		if(RS_ready())					//----- Check Serial Port	
			DebugKeyIn( RS_rx() );
		Get_TVI_ISR();
	}
}
#endif //#ifdef TP2912_MAIN

