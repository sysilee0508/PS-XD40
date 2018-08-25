// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "VS4210.h"
// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
#define DUMP_TIME 10

#define _TitleOn			0x04	//xxxxx100
#define _ContentOn			0x02	//xxxxx010
#define _BottomOn			0x01	//xxxxx001

#define _TransParentColor  8



typedef enum
{
    _OUTPUT_720P60=0,
    _OUTPUT_1080P60,
    _OUTPUT_720P30,
    _OUTPUT_1080P30,

    _OUTPUT_NUMBER
} OUTPUT_SIZE;
// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static tiByte gCloseWindowflg[4]  ;
static tiByte gbPreVXIS_InputMode[4] ;
static unsigned short dump_count=0;
static tiByte bOldSignal[4];
static tiByte OutSize30fpsFlg1 = 0 ;
static int TimeOutCount ;


// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
tiByte bSignal[4]= {0,0,0,0} ;
tiByte gbVXIS_InputMode[4] ;
tiByte gbVXIS_OuputModeflg = VS4210_FULL_MODE1;
tByte gaHDMI_MapWindowIndex[4] = {0,1,2,3} ;
tiByte gbVXIS_OuputSize =  _OUTPUT_1080P60;
bool EnDrawOSD = 1 ;

T_VS4210_JointKind VS4210_JointKind1 ;

tByte gHDMI_Index = 2 ;
unsigned short gHActive[4], gVActive[4] ;
unsigned short gHTotal[4], gVTotal[4] ;

extern tiByte PreVState[4] ;
extern tiByte VState[4] ;
extern tiByte AState[4] ;
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------
void delay1ms(unsigned short ms) 
{
	Delay_ms(ms);
}

void msleep(unsigned int i )
{
	Delay_ms(i);
}

//--------------------------------------------------------------------------------------------------------------------------
tByte I2CRead(unsigned char maddr,unsigned char saddr)
{
	return I2C_READ(maddr, saddr);
} 

//--------------------------------------------------------------------------------------------------------------------------
bit I2CWrite(unsigned char maddr,unsigned char saddr,unsigned char value)
{
	I2C_WRITE(maddr, saddr, value);
	
	return 1 ;
}

//--------------------------------------------------------------------------------------------------------------------------
bit I2CMultiWrite(unsigned char maddr,unsigned char saddr,unsigned char number,tcByte *data)
{
	unsigned char i;
	unsigned char val;

	I2C_Start();

	I2C_P2S(maddr&0xFE);
	AckDetect();

	I2C_P2S(saddr);
	AckDetect();

	for(i = 0; i < number; i++)
	{
		val = data[i];
		I2C_P2S(val);
		AckDetect();
	}
	I2C_Stop();

	return 1 ;
}

//--------------------------------------------------------------------------------------------------------------------------
bit I2CMultiAttribute(unsigned char maddr,unsigned char saddr,unsigned char number,unsigned char *value)
{
	unsigned char rbuf[80] ;
	unsigned int i ;

	for ( i = 0 ; i < number; i++ )
	{
		rbuf[i] = value[0];
	}
	I2CMultiWrite(VS4210_ADDR, saddr, number,&rbuf[0]);

	return 1 ;
}

tByte HDMIRX_ReadI2C_Byte(tByte RegAddr)
{
	tByte uc ;
	tByte Index ;
	//Index =gaHDMI_MapIndex[gHDMI_Index] ;
	Index =gHDMI_Index ;

	if (Index == 0 )
	{
		uc = I2CRead(HDMIRXADR,RegAddr) ;
	}
	else if (Index == 1 )
	{
		uc = I2CRead(HDMIRXADR_B,RegAddr) ;
	}
	else if (Index == 2 )
	{
		uc = I2CRead(HDMIRXADR,RegAddr) ;
	}
	else
	{
		uc = I2CRead(HDMIRXADR_B,RegAddr) ;
	}

	return uc ;
}

tByte HDMIRX_WriteI2C_Byte(tByte RegAddr,tByte val)
{
	tByte Index ;
	//Index =gaHDMI_MapIndex[gHDMI_Index] ;
	Index =gHDMI_Index ;

	if (Index == 0 )
	{
		I2CWrite(HDMIRXADR,RegAddr,val) ;
	}
	else if (Index == 1 )
	{
		I2CWrite(HDMIRXADR_B,RegAddr,val) ;
	}
	else if (Index == 2 )
	{
		I2CWrite(HDMIRXADR,RegAddr,val) ;
	}
	else
	{
		I2CWrite(HDMIRXADR_B,RegAddr,val) ;
	}
	return 0 ;
}

tByte HDMIRX_WriteI2C_ByteN(tByte RegAddr,tcByte *pData,int N)
{
	tByte Index ;
	//Index =gaHDMI_MapIndex[gHDMI_Index] ;
	Index =gHDMI_Index ;

	if (Index == 0 )
	{
		I2CMultiWrite(HDMIRXADR,RegAddr,N,pData) ;
	}
	else if (Index == 1 )
	{
		I2CMultiWrite(HDMIRXADR_B,RegAddr,N,pData) ;
	}
	else if (Index == 2 )
	{
		I2CMultiWrite(HDMIRXADR,RegAddr,N,pData) ;
	}
	else
	{
		I2CMultiWrite(HDMIRXADR_B,RegAddr,N,pData) ;
	}
	return 0  ;
}

//--------------------------------------------------------------------------------------------------------------------------
void SetOsdSize(tByte block,tByte sizeX,tByte sizeY)
{
	tByte temp;
	switch(block)
	{
	case _Title0 :
		temp=I2CRead(VS4210_ADDR,0x53);
		I2CWrite(VS4210_ADDR,0x53,temp&0x3f);
		I2CWrite(VS4210_ADDR,0x53,sizeX&0x3f);
		break;
	case _Title1 :
		temp=I2CRead(VS4210_ADDR,0x53);
		I2CWrite(VS4210_ADDR,0x53,(temp&0x3f) | 0x40  ) ;
		I2CWrite(VS4210_ADDR,0x53,(sizeX&0x3f) | 0x40  ) ;
		break;
	case _Title2 :
		temp=I2CRead(VS4210_ADDR,0x53);
		I2CWrite(VS4210_ADDR,0x53,(temp&0x3f) | 0x80 )  ;
		I2CWrite(VS4210_ADDR,0x53,(sizeX&0x3f) | 0x80 )  ;
		break;
	case _Title3 :
		temp=I2CRead(VS4210_ADDR,0x53);
		I2CWrite(VS4210_ADDR,0x53,(temp&0x3f) | 0xC0 )  ;
		I2CWrite(VS4210_ADDR,0x53,(sizeX&0x3f) | 0xC0 )  ;
		break;

	case _Bar0:
		temp=I2CRead(VS4210_ADDR,0x5E);
		I2CWrite(VS4210_ADDR,0x5E,temp&0x3f) ;
		I2CWrite(VS4210_ADDR,0x5E,sizeX&0x3f) ;
		break;
	case _Bar1:
		temp=I2CRead(VS4210_ADDR,0x5E);
		I2CWrite(VS4210_ADDR,0x5E,(temp&0x3f)| 0x40)  ;
		I2CWrite(VS4210_ADDR,0x5E,(sizeX&0x3f)| 0x40)  ;
		break;
	case _Bar2:
		temp=I2CRead(VS4210_ADDR,0x5E);
		I2CWrite(VS4210_ADDR,0x5E,(temp&0x3f) | 0x80);
		I2CWrite(VS4210_ADDR,0x5E,(sizeX&0x3f) | 0x80);
		break;
	case _Bar3:
		temp=I2CRead(VS4210_ADDR,0x5E);
		I2CWrite(VS4210_ADDR,0x5E,(temp&0x3f) | 0xC0);
		I2CWrite(VS4210_ADDR,0x5E,(sizeX&0x3f) | 0xC0);
		break;

	case _Content:
		I2CWrite(VS4210_ADDR,0x58,sizeX&0x3f);
		I2CWrite(VS4210_ADDR,0x59,sizeY&0x1f);
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear(tByte Win )
{
    tByte temp1;
    tByte temp2 ;

    if (Win == 0 )
    {
        SetOsdSize(_Title0,0,1);
        SetOsdSize(_Bar0,0,1);
        //clear 1
        /*
        temp1 = I2CRead(VS4210_ADDR,0x53);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x53,0x00|temp2 );

        temp1 =I2CRead(VS4210_ADDR,0x5E);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x5E,0x00|temp2 );
        I2CWrite(VS4210_ADDR,0x51,0x00);
        */
    }
    else if (Win == 1 )
    {
        temp1 = I2CRead(VS4210_ADDR,0x53);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x53,0x40|temp2 );

        temp1 =I2CRead(VS4210_ADDR,0x5E);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x5E,0x40|temp2 );
        I2CWrite(VS4210_ADDR,0x51,0x00);
    }
    else if (Win == 2 )
    {
        temp1 = I2CRead(VS4210_ADDR,0x53);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x53,0x80|temp2 );

        temp1 =I2CRead(VS4210_ADDR,0x5E);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x5E,0x80|temp2 );
        I2CWrite(VS4210_ADDR,0x51,0x00);
    }
    else if (Win == 3 )
    {
        temp1 = I2CRead(VS4210_ADDR,0x53);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x53,0xC0|temp2 );

        temp1 =I2CRead(VS4210_ADDR,0x5E);
        temp2 = temp1 & 0x3f ;
        I2CWrite(VS4210_ADDR,0x5E,0xC0|temp2 );
        I2CWrite(VS4210_ADDR,0x51,0x00);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_All(void )
{
	tByte i ;
	for (i = 0 ; i < 4 ; i++)
	{
		OSD_Clear(i) ;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_234(void )
{
	tByte i ;
	for (i = 1 ; i < 4 ; i++)
	{
		OSD_Clear(i) ;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_34(void )
{
	tByte i ;
	for (i = 2 ; i < 4 ; i++)
	{
		OSD_Clear(i) ;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_4(void )
{
	OSD_Clear(3) ;
}

//--------------------------------------------------------------------------------------------------------------------------
void SetOsdStartAddress(tByte block,tByte addr)
{
    switch(block)
    {
    case(_Title0):
    case(_Title1):
    case(_Title2):
    case(_Title3):
        I2CWrite(VS4210_ADDR,0x52,addr);
        break;

    case(_Content):
        I2CWrite(VS4210_ADDR,0x57,addr);
        break;

    case(_Bar0):
    case(_Bar1):
    case(_Bar2):
    case(_Bar3):
        I2CWrite(VS4210_ADDR,0x5D,addr);
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void OsdDumpString(tByte OSDaddr,tcByte *Strdata)
{
    tByte Lan=0;
    Lan=Strdata[0];
    I2CWrite(VS4210_ADDR,0xfd,0x10);
    I2CWrite(VS4210_ADDR,0xfe,OSDaddr);
    Strdata++;
    I2CMultiWrite(VS4210_ADDR,0xff,Lan,Strdata);
}

//--------------------------------------------------------------------------------------------------------------------------
void OsdDumpAttribute(tByte OSDaddr,tcByte *Strdata,tByte index)
{
    tByte Lan=0;
    Lan=Strdata[0];
    I2CWrite(VS4210_ADDR,0xfd,0x70);
    I2CWrite(VS4210_ADDR,0xfe,OSDaddr);
    I2CMultiAttribute(VS4210_ADDR,0xff,Lan,&index);
}

//--------------------------------------------------------------------------------------------------------------------------
void SetOsdPosition(tByte block,tWord X_Pos,tWord Y_Pos)
{
    tByte XHighByte;
    tByte XLowByte;
    tByte YHighByte;
    tByte YLowByte;

    XLowByte = (tByte)(X_Pos&0x00FF);
    XHighByte = ((tByte)((X_Pos>>8)&0x07))<<4;

    YLowByte = (tByte)(Y_Pos&0x00FF);
    YHighByte = (tByte)((Y_Pos>>8)&0x07 );

    switch(block)
    {
    case(_Title0):
    case(_Title1):
    case(_Title2):
    case(_Title3):
        I2CWrite(VS4210_ADDR,0x54,XHighByte|YHighByte);
        I2CWrite(VS4210_ADDR,0x55,XLowByte);
        I2CWrite(VS4210_ADDR,0x56,YLowByte);
        break;
    case(_Content):
        I2CWrite(VS4210_ADDR,0x5A,XHighByte|YHighByte);
        I2CWrite(VS4210_ADDR,0x5B,XLowByte);
        I2CWrite(VS4210_ADDR,0x5C,YLowByte);
        break;
    case(_Bar0):
    case(_Bar1):
    case(_Bar2):
    case(_Bar3):
        I2CWrite(VS4210_ADDR,0x5F,XHighByte|YHighByte);
        I2CWrite(VS4210_ADDR,0x60,XLowByte);
        I2CWrite(VS4210_ADDR,0x61,YLowByte);
        break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void setTransParentIndex(tByte index)
{
    tByte temp;
    temp=I2CRead(VS4210_ADDR,0x69);
    temp=(temp&0x0f)|(index<<4);
    I2CWrite(VS4210_ADDR,0x69,temp);
    I2CWrite(VS4210_ADDR,0x58,0x0);  // VS4210 //black
}

//--------------------------------------------------------------------------------------------------------------------------
void VXIS_fSetOsdOnOff(tByte block)
{
    tByte temp=0;
    temp=I2CRead(VS4210_ADDR,0x51);
    temp=(temp&0xf8)|(block&0x07);
    I2CWrite(VS4210_ADDR,0x51,temp);
}
//--------------------------------------------------------------------------------------------------------------------------
void VX_Write_OSD_In_OutString(tByte ch , tcByte *Str1,tcByte *Str2, int out_mode)
{
    tiWord ix,iy ;

    
	if (out_mode == VS4210_FULL_MODE1 )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == 1)
            {
                ix = 180 ;
                iy = 61 ;
            }
            else
            {
                ix = 244 ;
                iy = 37 ;
            }
        }
        else if (ch == 1 )
        {
            if(gbVXIS_OuputSize == 1)
            {
                ix = 1140 ;
                iy = 61 ;
            }
            else
            {
                ix = 886 ;
                iy = 37 ;
            }
        }
        else if (ch == 2 )
        {
            if(gbVXIS_OuputSize == 1)
            {
                ix = 180 ;
                iy = 603 ;
            }
            else
            {
                ix = 244 ;
                iy = 401 ;
            }
        }
        else //(ch == 3 )
        {
            if(gbVXIS_OuputSize == 1)
            {
                ix = 1140 ;
                iy = 603 ;
            }
            else
            {
                ix = 886 ;
                iy = 401 ;
            }
        }

        }
    else
    {
        ix = 224 ;
        iy = 64 ;
    }

    if (ch == 0  )
    {
        SetOsdSize(_Title0,Str1[0],1);
        SetOsdStartAddress(_Title0,0x00);
        OsdDumpString(0x00,Str1);
        OsdDumpAttribute(0x00,Str1,0x07);
        SetOsdPosition(_Title0,ix,iy);

        SetOsdSize(_Bar0,Str2[0],1);
        SetOsdStartAddress(_Bar0,0x40);
        OsdDumpString(0x40,Str2);
        OsdDumpAttribute(0x40,Str2,0x07);
        SetOsdPosition(_Bar0,ix,iy + 20);


    }
    else if (ch == 1 )
    {
        SetOsdSize(_Title1,Str1[0],1);

        SetOsdStartAddress(_Title1,0x10);
        OsdDumpString(0x10,Str1);
        OsdDumpAttribute(0x10,Str1,0x07);
        SetOsdPosition(_Title1,ix,iy);


        SetOsdSize(_Bar1,Str2[0],1);
        SetOsdStartAddress(_Bar1,0x50);
        OsdDumpString(0x50,Str2);
        OsdDumpAttribute(0x50,Str2,0x07);
        SetOsdPosition(_Bar1,ix,iy + 20);
    }


    else if (ch == 2 )
    {
        SetOsdSize(_Title2,Str1[0],1);

        SetOsdStartAddress(_Title1,0x20);
        OsdDumpString(0x20,Str1);
        OsdDumpAttribute(0x20,Str1,0x07);
        SetOsdPosition(_Title2,ix,iy);

        SetOsdSize(_Bar2,Str2[0],1);
        SetOsdStartAddress(_Bar2,0x60);
        OsdDumpString(0x60,Str2);
        OsdDumpAttribute(0x60,Str2,0x07);
        SetOsdPosition(_Bar2,ix,iy + 20);

    }

    else if (ch == 3 )
    {
        SetOsdSize(_Title3,Str1[0],1);

        SetOsdStartAddress(_Title3,0x30);
        OsdDumpString(0x30,Str1);
        OsdDumpAttribute(0x30,Str1,0x07);
        SetOsdPosition(_Title3,ix,iy);

        SetOsdSize(_Bar3,Str2[0],1);
        SetOsdStartAddress(_Bar3,0x70);
        OsdDumpString(0x70,Str2);
        OsdDumpAttribute(0x70,Str2,0x07);
        SetOsdPosition(_Bar3,ix,iy + 20);

    }


    setTransParentIndex(_TransParentColor);


    VXIS_fSetOsdOnOff(_TitleOn | _BottomOn );

	msleep(5);
}

//--------------------------------------------------------------------------------------------------------------------------
 void VXIS_fShowInOutMode(tiByte ch , tiByte in_mode, tiByte out_mode  )
{
    tcByte *ptr1 ;
    tByte chFix ;
    chFix = gaHDMI_MapWindowIndex[ch] ;

    if (EnDrawOSD == 1 )
    {

        chFix = ch ;

        if (chFix == 0 )
        {
            ptr1 = OSD_CH0_param ;
        }
        else  	if (chFix == 1 )
        {
            ptr1 = OSD_CH1_param ;
        }
        else  	if (chFix == 2 )
        {
            ptr1 = OSD_CH2_param ;
        }
        else
        {
            ptr1 = OSD_CH3_param ;
        }

        switch(in_mode)
        {

        case _OSD_1920X1080P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1920x1080P_param,out_mode) ;
            break;

        case _OSD_No_Signal :
            VX_Write_OSD_In_OutString(ch , ptr1, OSD_No_Signal_param,out_mode) ;
            break;
        }
    }

}

//--------------------------------------------------------------------------------------------------------------------------
void VXIS_fShowInOutMode0Fast(tiByte ch , tiByte in_mode, tiByte out_mode  )
{
    tcByte *ptr1 ;
    tByte chFix ;

    if (EnDrawOSD == 1 )
    {
        if (ch == 0 )
        {
            ptr1 = OSD_CH0_param ;
        }
        else if (ch == 1 )
        {
            ptr1 = OSD_CH1_param ;
        }
        else if (ch == 2 )
        {
            ptr1 = OSD_CH2_param ;
        }
        else
        {
            ptr1 = OSD_CH3_param ;
        }

        chFix = 0  ;
        switch(in_mode)
        {
        case _OSD_720X480I60 :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_720x480I60_param,out_mode) ;
            break ;

        case _OSD_720X480P60 :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_720x480P60_param,out_mode ) ;
            break ;

        case _OSD_720X576I50 :
            VX_Write_OSD_In_OutString(chFix , ptr1,OSD_720x576I50_param,out_mode) ;
            break ;

        case _OSD_720X576P50 :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_720x576P50_param,out_mode) ;
            break ;

        case _OSD_1920X1080I :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1920x1080I_param,out_mode) ;
            break;

        case _OSD_1280X720P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1280x720P_param,out_mode) ;
            break;

        case _OSD_1920X1080P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1920x1080P_param,out_mode) ;
            break;

    #ifndef HDMI_ONLY //DVI
        case _OSD_640X480P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_640X480P_param,out_mode) ;
            break;
        case _OSD_8000X600P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_8000X600P_param,out_mode) ;
            break;
        case _OSD_1024X768P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1024X768P_param,out_mode) ;
            break;
        case _OSD_1280X768P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1280X768P_param,out_mode) ;
            break;
        case _OSD_1360X768P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1360X768P_param,out_mode) ;
            break;
        case _OSD_1366X768P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1366X768P_param,out_mode) ;
            break;
        case _OSD_1280X800P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1280X800P_param,out_mode) ;
            break;
        case _OSD_1440X900P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1440X900P_param,out_mode) ;
            break;
        case _OSD_1280X1024P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1280X1024P_param,out_mode) ;
            break;
        case _OSD_1400X1050P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1400X1050P_param,out_mode) ;
            break;
        case _OSD_1680X1050P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1680X1050P_param,out_mode) ;
            break;
        case _OSD_1600X1200P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1600X1200P_param,out_mode) ;
            break;
        case _OSD_1920X1200P :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_1920X1200P_param,out_mode) ;
            break;
    #endif

        case _OSD_OUT_OF_RANGE :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_Out_of_range_param,out_mode) ;
            break;

        case _OSD_No_Signal :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_No_Signal_param,out_mode) ;
            break;

        default :
            VX_Write_OSD_In_OutString(chFix , ptr1, OSD_Not_Support_param,out_mode) ;
            break;
        }
    }

}

void OSD_init()
{
	tByte  i ;
	for (i = 0 ; i< CH_IN_NUMBER ; i++)
	{
		gHDMI_Index = i ;
		VXIS_fShowInOutMode(gaHDMI_MapWindowIndex[gHDMI_Index]  ,  gbVXIS_InputMode[gHDMI_Index] , gbVXIS_OuputModeflg )  ;
	}
}

static void dump_vidmode()
{
	tByte osdflash = 0;
	tByte j ;

	gbVXIS_InputMode[gHDMI_Index] = GetHDMI_InputMode(gHDMI_Index ) ;

	if (gbPreVXIS_InputMode[gHDMI_Index] != gbVXIS_InputMode[gHDMI_Index] )
	{
		printf("gHDMI_Index =%d,gbVXIS_InputMode=0x%02x,gbVXIS_OuputModeflg=%d \r\n",(int) gaHDMI_MapWindowIndex[gHDMI_Index] , (int) gbVXIS_InputMode[gHDMI_Index], (int)gbVXIS_OuputModeflg ) ;
		osdflash = VS4210_CheckMode0Status() ;

		if (osdflash == 1)
		{
			if (gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w3 )
			{
				VXIS_fShowInOutMode0Fast(gHDMI_Index ,  gbVXIS_InputMode[gHDMI_Index] , gbVXIS_OuputModeflg ) ;
			}
			else
			{
				for (j = 0 ; j < gWindowNum ; j++)
				{
					if (gaHDMI_MapWindowIndex[j]  ==  gHDMI_Index )
					{
						if (gaOpenWindowflg[j] == 1 )
						{
							VXIS_fShowInOutMode(j, gbVXIS_InputMode[gHDMI_Index] , gbVXIS_OuputModeflg ) ;
						}
						else
						{
							OSD_Clear(j);
						}
					}
				}
			}
		}
		gbPreVXIS_InputMode[gHDMI_Index] = gbVXIS_InputMode[gHDMI_Index]  ;
		VS4210_displayon();
		dump_count = 0 ;
	}
}

void HDMI_VideoModeChangeDetect(void)
{
	tiByte i ,j ;
	tByte  int1data2  ;
	tByte  tmp   ;
	tmp = gHDMI_Index ;
	for (i =0 ; i < CH_IN_NUMBER ; i++ )
	{
		gHDMI_Index = i ;
		int1data2 = HDMIRX_ReadI2C_Byte(0x10);
		if ( (int1data2 & 0x04 ) == 0x00 )
		{
			if (gCloseWindowflg[i] == 0   )
			{
				gCloseWindowflg[i] = 1  ;
				if (gbVXIS_OuputModeflg >= VS4210_1080P_MODE1 )
				{
					//if ((gbVXIS_OuputModeflg == VS4210_1080P_MODE24) && (gbVXIS_OuputSize == _OUTPUT_1080P60 ))
					{
						if (gaHDMI_MapWindowIndex[0] == gHDMI_Index )
						{
							if (OutSize30fpsFlg1 ==1 )
							{
								VS4210_CloseWindowsX(0);
							}
							else
							{
								VS4210_CloseWindowsX(0);
								VS4210_CloseWindowsX(1);
								VS4210_CloseWindowsX(2);
							}
						}
						else if (gaHDMI_MapWindowIndex[1] == gHDMI_Index )
						{
							if (OutSize30fpsFlg1 ==1 )
							{
								VS4210_CloseWindowsX(1);
							}
							else
							{
								VS4210_CloseWindowsX(3);
							}
						}
					}
					//else
					//{
						for ( j = 0 ; j < gWindowNum ; j++)
						{
							if (gaHDMI_MapWindowIndex[j] == gHDMI_Index )
							{
								VS4210_CloseWindowsX(j);
							}
						}
					//}
					//printf("@@%s,%d,i=%d,mapi=%d \n", __func__,__LINE__ ,i,gaHDMI_MapWindowIndex[i] ) ;
				}
				else
				{
					if (gbVXIS_OuputModeflg == i )
					{
						VS4210_CloseAllWindows();
					}
				}
				gbPreVXIS_InputMode[i] = 250 ;
			}
		}
		else
		{
			gCloseWindowflg[i] = 0  ;
		}
	}
	gHDMI_Index = tmp ;
}

static void HDMI_Rx_NoSignal(void )
{
	tByte j ;
	if (gbVXIS_InputMode[gHDMI_Index] !=  _OSD_No_Signal  )
	{
		gbPreVXIS_InputMode[gHDMI_Index] = 251 ;
		gbVXIS_InputMode[gHDMI_Index] =_OSD_No_Signal  ;

		if( (gbVXIS_OuputModeflg >= VS4210_1080P_MODE1 ) &&  (gbVXIS_OuputModeflg < VS4210_1080P_MODE_NUMBER ))
		{
			for ( j = 0 ; j < gWindowNum ; j++)
			{
				if (gaHDMI_MapWindowIndex[j] == gHDMI_Index )
				{
					if (gaOpenWindowflg[j] == 1 )
					{
						VXIS_fShowInOutMode(j ,  gbVXIS_InputMode[gHDMI_Index] , gbVXIS_OuputModeflg )  ;
					}
					else
					{
						OSD_Clear(j);
					}
				}
			}
		}

		else if (gbVXIS_OuputModeflg <=  VS4210_1080P_MODE0_w3 )
		{
			if (gHDMI_Index == gbVXIS_OuputModeflg )
			{
				VS4210_CloseAllWindows();
				VXIS_fShowInOutMode0Fast(gHDMI_Index ,  gbVXIS_InputMode[gHDMI_Index] , gbVXIS_OuputModeflg ) ;
			}
		}

		if ( bSignal[gaHDMI_MapWindowIndex[0]] == 0  )
		{
			VS4210_channel0clear(0 ) ;
		}
	}
}

void vs4210_display_proc()
{

	TimeOutCount++ ;

	if (TimeOutCount == 10 )
	{
		HDMITX_DevLoopProc() ;
	}

	if (TimeOutCount > 10 )
	{
		dump_count ++ ;
		TimeOutCount = 0 ;

		VS4210_JointKind1.WindowMap.WMap0 = 1 ;
		VS4210_JointKind1.WindowMap.WMap1 = 2 ;
		VS4210_JointKind1.WindowMap.WMap2 = 3 ;
		VS4210_JointKind1.WindowMap.WMap3 = 4 ;
		VS4210_JointKind1.OutputSize = gbVXIS_OuputSize ;
		VS4210_JointKind1.OutputMode = VS4210_FULL_MODE1;
		OutSize30fpsFlg1 = 0 ;

		VS4210_VideoJoin_Output(&VS4210_JointKind1) ;
		//I2CWrite(VS4210_ADDR, 0x1A, 0x0E);

		for (gHDMI_Index = 0 ; gHDMI_Index < CH_IN_NUMBER ; gHDMI_Index++)
		{
			bSignal[gHDMI_Index] = CheckHDMIRX();
			if (PreVState[gHDMI_Index] != VState[gHDMI_Index]  )
			{
				PreVState[gHDMI_Index]  = VState[gHDMI_Index] ;
			}

			if(bSignal[gHDMI_Index] != bOldSignal[gHDMI_Index] )
			{
				bOldSignal[gHDMI_Index] = bSignal[gHDMI_Index] ;
				// if Changed Mode ...
				if( bSignal[gHDMI_Index] == 1  )
				{
					dump_vidmode();
				}
			}
			else
			{
				HDMI_VideoModeChangeDetect();
			}
		}

		if( dump_count > DUMP_TIME  )
		{
			dump_count = 0 ;
			for (gHDMI_Index = 0 ; gHDMI_Index < CH_IN_NUMBER ; gHDMI_Index++)
			{
				if( bSignal[gHDMI_Index] ==1  )
				{
					dump_vidmode();
				}
				else
				{
					HDMI_Rx_NoSignal()  ;
				}
			}
		}
	}
	msleep(5) ;
}

//--------------------------------------------------------------------------------------------------------------------------
void vs4210_system_init()
{

    tiByte i ;
    tiByte reg_IN0, reg_IN1, reg_INA, reg_OS0C, reg_OS0D, reg_PT0 = 0;

    for (i = 0 ; i < CH_IN_NUMBER ; i++)
    {
        bSignal[i] = 0 ;
        gbPreVXIS_InputMode[i] = 251 ;
        bOldSignal[i] = 0  ;
        gCloseWindowflg[i] = 0 ;
    }
    PreVXIS_FULLOuputModeflg = 0 ;
    dump_count = 0 ;

    I2CWrite(VS4210_ADDR, 0x01 ,0x5A) ;  //reset

    for (i = 0 ; i < CH_IN_NUMBER ; i++ )
    {
        gHDMI_Index = i ;
        InitHDMIRX(0 );
        //HDMIRX_WriteI2C_Byte(0x89 ,0x80);
    }

    VS4210_Line_Clear() ;
    VS4210_Line_Draw(gbVXIS_OuputModeflg ) ;
    gHDMI_Index = 0 ;

    OSD_Clear_All();
    VS4210_StartInit();

    //reg_IN0 = I2CRead(VS4210_ADDR, 0x10);
    I2CWrite(VS4210_ADDR, 0x10, 0x80);
    //reg_IN1 = I2CRead(VS4210_ADDR, 0x11);
    I2CWrite(VS4210_ADDR, 0x11, /*reg_IN1 |*/ 0x03);
    //reg_IN1 = I2CRead(VS4210_ADDR, 0x11);
    //reg_INA = I2CRead(VS4210_ADDR, 0x1A);
    //I2CWrite(VS4210_ADDR, 0x1A, 0x0E);

    HDMI_Tx_Init();
    GetHDMIstate();
    //reg_OS0C = I2CRead(VS4210_ADDR, 0x81);
    I2CWrite(VS4210_ADDR, 0x81, 0x00);
    //reg_OS0D = I2CRead(VS4210_ADDR, 0x82);
    //reg_PT0 = I2CRead(VS4210_ADDR, 0xF0);
    //I2CWrite(VS4210_ADDR, 0xF0, reg_PT0 | 0x86);
    //reg_PT0 = I2CRead(VS4210_ADDR, 0xF0);
    TimeOutCount = 0 ;

    OSD_init();

}
