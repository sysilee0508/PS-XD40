// *****************************************************
// Company : Techpoint Inc
// $Date: 2013-09-17 
// $Log:  $
// ******************************************************

#include "Config.h"
//#include "reg.h"
#include "TP2912_Typedefs.h"
#include "TP2912_i2c.h"
//#include "main.h"
#include "printf.h"
#include "Device_Tx.h"
#include "TP2912.h"
//#include "Table.c"

//#define TVP7002_clkbypass

CODE_P	BYTE	*ptr_TVP7002;
CODE_P 	BYTE	*ptr_ADV7611_HDMIMAP;
CODE_P 	BYTE	*ptr_ADV7611_IO;

extern CODE BYTE TVP7002_1080P30_DataSet[];
extern CODE BYTE TVP7002_720P_DataSet[];
extern CODE BYTE TVP7002_480i_DataSet[];
extern CODE BYTE ADV7611_HDMIMAP_Dataset[];
extern CODE BYTE ADV7611IO__1080P30_Dataset[];
extern CODE BYTE ADV7611IO__720P60_Dataset[];
extern CODE BYTE ADV7611IO__720P30_Dataset[];
extern CODE BYTE ADV7611IO__720P25_Dataset[];
extern CODE BYTE ADV7611IO__480i_Dataset[];
extern CODE BYTE PO3100K__720PP30_Dataset[];

extern DATA BYTE ManVidRes;

void TVP7002_720P(){
	ptr_TVP7002 = TVP7002_720P_DataSet;	// Video ADC
	I2CDeviceSet( ptr_TVP7002 );
}

 void TVP7002_1080P30(){
	 ptr_TVP7002 = TVP7002_1080P30_DataSet; // Video ADC
	 I2CDeviceSet( ptr_TVP7002 );
 }
 void TVP7002_480i(){
	 ptr_TVP7002 = TVP7002_480i_DataSet; // Video ADC
	 I2CDeviceSet( ptr_TVP7002 );
 }

 void ADV7611_RGBOut(){
 	TP2912_WriteI2C(0x98,0x02,0xf2);
	TP2912_WriteI2C(0x98,0x03,0x40);
	TP2912_WriteI2C(0x98,0x15,0xb0);
 }

 void ADV7611_1080P30(){
 	ptr_ADV7611_IO = ADV7611IO__1080P30_Dataset;
	I2CDeviceSet( ptr_ADV7611_IO );
#ifdef TP2915BD
	if(nRGB==0) 
		ADV7611_RGBOut();
#endif
 }
  void ADV7611_720P30(){
 	ptr_ADV7611_IO = ADV7611IO__720P30_Dataset;
	I2CDeviceSet( ptr_ADV7611_IO );
#ifdef TP2915BD
		if(nRGB==0) 
			ADV7611_RGBOut();
#endif

 }
 void ADV7611_720P25(){
 	ptr_ADV7611_IO = ADV7611IO__720P25_Dataset;
	I2CDeviceSet( ptr_ADV7611_IO );
#ifdef TP2915BD
		if(nRGB==0) 
			ADV7611_RGBOut();
#endif

 }
 void ADV7611_720P60(){
 	ptr_ADV7611_IO = ADV7611IO__720P60_Dataset;
	I2CDeviceSet( ptr_ADV7611_IO );
#ifdef TP2915BD
		if(nRGB==0) 
			ADV7611_RGBOut();
#endif

 }
 void ADV7611_480i(){
 	ptr_ADV7611_IO = ADV7611IO__480i_Dataset;
	I2CDeviceSet( ptr_ADV7611_IO );
#ifdef TP2915BD
		if(nRGB==0) 
			ADV7611_RGBOut();
#endif

 }
 
 void Set_ADV7611_CPMAP(BYTE Addr){
 	TP2912_WriteI2C(Addr,0x6c,0x00);
 }
/*
 void Set_ADV7611_HDMI(BYTE Addr){
 	WriteI2C(Addr,0x01,0x01);
 }
*/
 void Set_ADV7611_SubAddr(BYTE DAddr,BYTE  CECAddr,BYTE INFOAddr,BYTE DPLLAddr, BYTE KSVAddr,BYTE EDIDAddr, BYTE HDMIAddr,BYTE CPAddr){
 	TP2912_WriteI2C(DAddr,0xf4,CECAddr);		//CEC Map I2C address
 	TP2912_WriteI2C(DAddr,0xf5,INFOAddr);		//InforFrame Map I2C address
 	TP2912_WriteI2C(DAddr,0xf8,DPLLAddr);		//DPLL Map I2C address
 	TP2912_WriteI2C(DAddr,0xf9,KSVAddr);		//KSV Map I2C address
 	TP2912_WriteI2C(DAddr,0xfa,EDIDAddr);		//EDID Map I2C address
 	TP2912_WriteI2C(DAddr,0xfb,HDMIAddr);		//HDMI Map I2C address
 	TP2912_WriteI2C(DAddr,0xfd,CPAddr);		//CP Map I2C address
 }

 void SetInit_ADV7611(){				//HDMI RX
 	BYTE DAddr = 0x98;
	BYTE CECAddr = 0x80;
	BYTE INFOAddr = 0x7c;
	BYTE DPLLAddr = 0x4c;
	BYTE KSVAddr = 0x62;
	BYTE EDIDAddr = 0x6c;
	BYTE HDMIAddr = 0x68;
	BYTE CPAddr = 0x44;

	Set_ADV7611_SubAddr(DAddr,CECAddr,INFOAddr,DPLLAddr,KSVAddr,EDIDAddr,HDMIAddr,CPAddr);
	Set_ADV7611_CPMAP(CPAddr);
	
	ptr_ADV7611_HDMIMAP = ADV7611_HDMIMAP_Dataset;
	I2CDeviceSet( ptr_ADV7611_HDMIMAP );

 }
 
 void SetInit_CAM_PO3100K(){
 
	 BYTE	 *ptr_PO3100K;
 
	 ptr_PO3100K = PO3100K__720PP30_Dataset;
	 I2CDeviceSet( ptr_PO3100K );
	 
 }

void SetDevicesFor1080P30(){
	
	TVP7002_1080P30();
	ADV7611_1080P30();
//	Printf("\r\nWrite done to devices registers for 1080P30...");
}
void SeteDvicesFor720P60(){
	TVP7002_720P(); 				//Set 720P for TVP7002(Video ADC)
	ADV7611_720P60();
//	Printf("\r\nWrite done to devices registers for 720P60...");
}
void SetDevicesFor720P30(){
	
	ADV7611_720P30();

}

void SetDevicesFor720P25(){
	
	ADV7611_720P25();

}
void SetDevicesFor480i(){
	
	ADV7611_480i();

}
void SetPLL_Reg(BYTE pll_n, BYTE pll_r, BYTE pll_q, BYTE pll_p, BYTE pll_pdiv1, BYTE vco_range ){
	BYTE Reg98,Reg99,Reg9a,Reg9b;

	Reg98= pll_n>>4;
	Reg99= (pll_n<<4)|(pll_r>>5);
	Reg9a=(pll_r<<3)|(pll_q>>3);
	Reg9b=(pll_q<<5)|(pll_p<<2)|vco_range;

	TP2912_WriteI2C(CDCE913,0x83,pll_pdiv1);
	TP2912_WriteI2C(CDCE913,0x98,Reg98);
	TP2912_WriteI2C(CDCE913,0x99,Reg99);
	TP2912_WriteI2C(CDCE913,0x9a,Reg9a);
	TP2912_WriteI2C(CDCE913,0x9b,Reg9b);
	
}

void Set_CDCE913_PLLIC(){
	BYTE pll_n,pll_r,pll_q,pll_p,pll_pdiv1,vco_range;
	
	TP2912_WriteI2C(CDCE913,0x81,0x05);
	TP2912_WriteI2C(CDCE913,0x94,0x6d);
	if(ManVidRes==AHD720P30_36M || ManVidRes==AHD720P25_36M){	//vco=144MHz
		pll_n=0x10;
		pll_r=0x01;
		pll_p=0x02;
		pll_q=0x15;
		vco_range=0x01;
		pll_pdiv1=0x04;
	}
	else{														//vco=148.5MHz
		pll_n=0x0b;
		pll_r=0x00;
		pll_p=0x02;
		pll_q=0x16;
		vco_range=0x01;					
	
		if((ManVidRes&0x08)==0x08)								//4M/5M/8M
			pll_pdiv1=0x01;
		else if(ManVidRes==R720P30_TVI20 || ManVidRes==R720P25_TVI20 ||
	            ManVidRes==AHD720P30 || ManVidRes==AHD720P25)		
			pll_pdiv1=0x04;
		else 	
			pll_pdiv1=0x02;
	}
	
	SetPLL_Reg(pll_n,pll_r,pll_q,pll_p,pll_pdiv1,vco_range);
	
	if(ManVidRes==NTSC || ManVidRes==PAL){
		TP2912_WriteI2C(CDCE913,0x94,0xed);
		TP2912_WriteI2C(CDCE913,0x83,0x01);
	}
	
}


void Init_RegSet_For_Devices(){
	
	SetInit_ADV7611();

	if(TP2912_ReadI2C(CDCE913,0x80) != 0xff)
		Set_CDCE913_PLLIC();

	if(TP2912_ReadI2C(PO3100K,0x00) != 0xff){
		SetInit_CAM_PO3100K();
//		if(!F37M125){							// for 37.125MHz video clock out
		if(ManVidRes==R720P30_TVI20 || ManVidRes==R720P25_TVI20){		
			TP2912_WriteI2C(PO3100K,0x03,0x00);		//Set registers group A
			TP2912_WriteI2C(PO3100K,0x25,0x0a);
			TP2912_WriteI2C(PO3100K,0x26,0xc1);
			TP2912_WriteI2C(PO3100K,0xd3,0xc0);
		}
	}
	switch(ManVidRes){
		case R1080P30: 
		case NR1920x532:
			SetDevicesFor1080P30();
			break;
		case R720P30_TVI20:
			SetDevicesFor720P30();
			break;
		case R720P25_TVI20:
			SetDevicesFor720P25();
			break;
		case NTSC:
			SetDevicesFor480i();
			break;
		default:
			SeteDvicesFor720P60();
			break;
	}
	/*
	if(ManVidRes == R1080P30)				
		SetDevicesFor1080P30();
	else
		SeteDvicesFor720P60();
	*/
	#ifdef TVP7002_clkbypass
		TP2912_WriteI2C(TVP7002,0x0f,0x2f);
	#endif
		
}

