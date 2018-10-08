#if 0
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
tiByte gbVXIS_InputMode[4];
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

void VS4210_DisplayoutputMode_Splite4(void)
{
	VS4210_JointKind1.WindowMap.WMap0 = 1 ;
	VS4210_JointKind1.WindowMap.WMap1 = 2 ;
	VS4210_JointKind1.WindowMap.WMap2 = 3 ;
	VS4210_JointKind1.WindowMap.WMap3 = 4 ;
	VS4210_JointKind1.OutputSize = gbVXIS_OuputSize ;
	VS4210_JointKind1.OutputMode = VS4210_FULL_MODE1;

	VS4210_VideoJoin_Output(&VS4210_JointKind1) ;
}

void VS4210_DisplayoutputMode_FullScreen(tiByte ch)
{
	VS4210_JointKind1.OutputSize = gbVXIS_OuputSize ;
	VS4210_JointKind1.OutputMode = VS4210_FULL_MODE0;
	
	switch (ch)
	{
		case CHANNEL1 : 
			VS4210_JointKind1.WindowMap.WMap0 = 1 ;
			break;
		case CHANNEL2 : 
			VS4210_JointKind1.WindowMap.WMap0 = 2 ;
			break;
		case CHANNEL3 : 
			VS4210_JointKind1.WindowMap.WMap0 = 3 ;
			break;
		case CHANNEL4 : 
			VS4210_JointKind1.WindowMap.WMap0 = 4 ;
			break;			
	}
	VS4210_VideoJoin_Output(&VS4210_JointKind1) ;
}

void vs4210_display_proc()
{
	tiByte display_output_mode = 0;

	TimeOutCount++ ;

	if (TimeOutCount == 10 )
	{
		HDMITX_DevLoopProc() ;
		
	}

	if (TimeOutCount > 10 )
	{
		dump_count ++ ;
		TimeOutCount = 0 ;
		display_output_mode = Get_DisplayoutputMode();
		if (display_output_mode == DISPLAY_MODE_4SPLIT)//SPLITMODE_SPLIT4)
		{
			VS4210_DisplayoutputMode_Splite4();
		}
		else
		{
			VS4210_DisplayoutputMode_FullScreen(Get_DisplayoutputChannel());
		}
		
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
	}

	VS4210_Line_Clear() ;
	VS4210_Line_Draw(gbVXIS_OuputModeflg ) ;
	gHDMI_Index = 0 ;

	OSD_Clear_All();
	VS4210_StartInit();

	I2CWrite(VS4210_ADDR, 0x10, 0x80);
	I2CWrite(VS4210_ADDR, 0x11, /*reg_IN1 |*/ 0x03);

	HDMI_Tx_Init();
	GetHDMIstate();
	I2CWrite(VS4210_ADDR, 0x81, 0x00);
	TimeOutCount = 0 ;

	OSD_init();

}
#endif
// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "common.h"

void vs4210_system_init()
{
	I2C_WRITE(0x40, 0x01 ,0x5A) ;  //reset

#if 0	//hdmi1080P_720P_mode1_w0-16bit-out-8bit-in
	I2C_WRITE(0x40,0x01,0x00);
	I2C_WRITE(0x40,0x02,0x00);
	I2C_WRITE(0x40,0x03,0x75);
	I2C_WRITE(0x40,0x04,0x18);
	I2C_WRITE(0x40,0x05,0x80);
	I2C_WRITE(0x40,0x06,0x20);
	I2C_WRITE(0x40,0x07,0x0C);
	I2C_WRITE(0x40,0x08,0x20);
	I2C_WRITE(0x40,0x09,0x0A);
	I2C_WRITE(0x40,0x0A,0x20);
	I2C_WRITE(0x40,0x0B,0x0A);
	I2C_WRITE(0x40,0x0C,0x00);
	I2C_WRITE(0x40,0x0D,0x20);
	I2C_WRITE(0x40,0x0E,0x10);
	I2C_WRITE(0x40,0x0F,0x00);
	
	I2C_WRITE(0x40,0x10,0x80);
	I2C_WRITE(0x40,0x11,0x83);
	I2C_WRITE(0x40,0x12,0x00);
	I2C_WRITE(0x40,0x13,0x00);
	I2C_WRITE(0x40,0x14,0x00);
	I2C_WRITE(0x40,0x15,0x88);
	I2C_WRITE(0x40,0x16,0x01);
	I2C_WRITE(0x40,0x17,0x00);
	I2C_WRITE(0x40,0x18,0xC0);
	I2C_WRITE(0x40,0x19,0x00);
	I2C_WRITE(0x40,0x1A,0x24);
//	I2C_WRITE(0x40,0x1B,0x30);	//Read only
	I2C_WRITE(0x40,0x1C,0x00);
	I2C_WRITE(0x40,0x1D,0x00);
	I2C_WRITE(0x40,0x1E,0x20);
	I2C_WRITE(0x40,0x1F,0x00);
	
	I2C_WRITE(0x40,0x20,0x00);
	I2C_WRITE(0x40,0x21,0x00);
//	I2C_WRITE(0x40,0x22,0x87);	//Read only
//	I2C_WRITE(0x40,0x23,0x98);	//Read only
//	I2C_WRITE(0x40,0x24,0x80);	//Read only
//	I2C_WRITE(0x40,0x25,0x44);	//Read only
//	I2C_WRITE(0x40,0x26,0x65);	//Read only
//	I2C_WRITE(0x40,0x27,0x38);	//Read only
	I2C_WRITE(0x40,0x28,0xF0);
	I2C_WRITE(0x40,0x29,0x00);
	I2C_WRITE(0x40,0x2A,0x80);
	I2C_WRITE(0x40,0x2B,0x11);
	I2C_WRITE(0x40,0x2C,0xFA);
	I2C_WRITE(0x40,0x2D,0x7F);
//	I2C_WRITE(0x40,0x2E,0x12);	//Read only
//	I2C_WRITE(0x40,0x2F,0x82);	//Read only

//	I2C_WRITE(0x40,0x30,0x6B);	//Read only
	I2C_WRITE(0x40,0x31,0x80);
	I2C_WRITE(0x40,0x32,0x80);
	I2C_WRITE(0x40,0x33,0x80);
	I2C_WRITE(0x40,0x34,0xA0);
	I2C_WRITE(0x40,0x35,0x18);
	I2C_WRITE(0x40,0x36,0x46);
	I2C_WRITE(0x40,0x37,0x64);
	I2C_WRITE(0x40,0x38,0x00);
	I2C_WRITE(0x40,0x39,0xFF);
	I2C_WRITE(0x40,0x3A,0x00);
	I2C_WRITE(0x40,0x3B,0x80);
	I2C_WRITE(0x40,0x3C,0x80);
	I2C_WRITE(0x40,0x3D,0x80);
	I2C_WRITE(0x40,0x3E,0x80);
	I2C_WRITE(0x40,0x3F,0x80);
	
	I2C_WRITE(0x40,0x40,0x80);
	I2C_WRITE(0x40,0x41,0x60);
	I2C_WRITE(0x40,0x42,0x5F);
	I2C_WRITE(0x40,0x43,0x37);
	I2C_WRITE(0x40,0x44,0x14);
	I2C_WRITE(0x40,0x45,0x02);
	I2C_WRITE(0x40,0x46,0x00);
	I2C_WRITE(0x40,0x47,0x00);
	I2C_WRITE(0x40,0x48,0x00);
	I2C_WRITE(0x40,0x49,0x00);
	I2C_WRITE(0x40,0x4B,0x20);
	I2C_WRITE(0x40,0x4C,0x00);
	I2C_WRITE(0x40,0x4D,0x00);
	I2C_WRITE(0x40,0x4E,0x00);
	I2C_WRITE(0x40,0x4F,0x7F);
	
	I2C_WRITE(0x40,0x50,0x20);
	I2C_WRITE(0x40,0x51,0x20);
	I2C_WRITE(0x40,0x52,0x00);
	I2C_WRITE(0x40,0x53,0x04);
	I2C_WRITE(0x40,0x54,0x00);
	I2C_WRITE(0x40,0x55,0xA0);
	I2C_WRITE(0x40,0x56,0x40);
	I2C_WRITE(0x40,0x57,0x04);
	I2C_WRITE(0x40,0x58,0x10);
	I2C_WRITE(0x40,0x59,0x08);
	I2C_WRITE(0x40,0x5A,0x10);
	I2C_WRITE(0x40,0x5B,0x00);
	I2C_WRITE(0x40,0x5C,0x68);
	I2C_WRITE(0x40,0x5D,0x40);
	I2C_WRITE(0x40,0x5E,0x08);
	I2C_WRITE(0x40,0x5F,0x21);
	
	I2C_WRITE(0x40,0x60,0x20);
	I2C_WRITE(0x40,0x61,0xB0);
	I2C_WRITE(0x40,0x62,0x33);
	I2C_WRITE(0x40,0x63,0x01);
	I2C_WRITE(0x40,0x64,0xFF);
	I2C_WRITE(0x40,0x65,0x01);
	I2C_WRITE(0x40,0x66,0xFF);
	I2C_WRITE(0x40,0x67,0x89);
	I2C_WRITE(0x40,0x68,0xAB);
	I2C_WRITE(0x40,0x69,0x28);
	I2C_WRITE(0x40,0x6A,0x6E);
	I2C_WRITE(0x40,0x6B,0x6E);
	I2C_WRITE(0x40,0x6C,0x6E);
	I2C_WRITE(0x40,0x6D,0x00);
	I2C_WRITE(0x40,0x6E,0x00);
	I2C_WRITE(0x40,0x6F,0x00);
	
	I2C_WRITE(0x40,0x70,0x00);
	I2C_WRITE(0x40,0x71,0x00);
	I2C_WRITE(0x40,0x72,0x00);
	I2C_WRITE(0x40,0x73,0x00);
	I2C_WRITE(0x40,0x74,0x02);
	I2C_WRITE(0x40,0x75,0xFF);
	I2C_WRITE(0x40,0x76,0xFF);
	I2C_WRITE(0x40,0x77,0xFF);
	I2C_WRITE(0x40,0x78,0xFF);
	I2C_WRITE(0x40,0x79,0xFF);
	I2C_WRITE(0x40,0x7A,0xFF);
	I2C_WRITE(0x40,0x7B,0xFF);
	I2C_WRITE(0x40,0x7C,0xFF);
	I2C_WRITE(0x40,0x7D,0xF0);
	I2C_WRITE(0x40,0x7E,0x80);
	I2C_WRITE(0x40,0x7F,0x00);
	
	I2C_WRITE(0x40,0x80,0x00);
	I2C_WRITE(0x40,0x81,0x34);	//It must be 0x00.
	I2C_WRITE(0x40,0x82,0x40);
	I2C_WRITE(0x40,0x83,0x05);
	I2C_WRITE(0x40,0x84,0x00);	//HSHIFT[7:0]
	I2C_WRITE(0x40,0x85,0xFC);
	//I2C_WRITE(0x40,0x85,0xD0);
	I2C_WRITE(0x40,0x86,0x05);
	I2C_WRITE(0x40,0x87,0x14);
	I2C_WRITE(0x40,0x88,0xDC);
	I2C_WRITE(0x40,0x89,0x6E);
	I2C_WRITE(0x40,0x8A,0x00);
	I2C_WRITE(0x40,0x8B,0x7E);
	I2C_WRITE(0x40,0x8C,0x03);
	I2C_WRITE(0x40,0x8D,0x28);
	I2C_WRITE(0x40,0x8E,0x8C);	//HSHIFT[10:8] | VSHIFT[10:8] | BOTTOM_MASK[9:8]
	//I2C_WRITE(0x40,0x8E,0x84);
	I2C_WRITE(0x40,0x8F,0x00);
	
	I2C_WRITE(0x40,0x90,0xCC);
	I2C_WRITE(0x40,0x91,0xA1);
	I2C_WRITE(0x40,0x92,0xC4);
	I2C_WRITE(0x40,0x93,0x62);
	I2C_WRITE(0x40,0x94,0x72);
	I2C_WRITE(0x40,0x95,0x00);
//	I2C_WRITE(0x40,0x96,0xEE);	//Read only
	I2C_WRITE(0x40,0x97,0x50);
	I2C_WRITE(0x40,0x98,0x1A);
	I2C_WRITE(0x40,0x99,0x90);
	I2C_WRITE(0x40,0x9A,0xFE);
	I2C_WRITE(0x40,0x9B,0x0C);	//WIN_MODE
	//I2C_WRITE(0x40,0x9B,0x00);
	I2C_WRITE(0x40,0x9C,0x10);
	I2C_WRITE(0x40,0x9D,0x00);
	I2C_WRITE(0x40,0x9E,0x02);
	I2C_WRITE(0x40,0x9F,0xEE);
	
	I2C_WRITE(0x40,0xA0,0x80);
	I2C_WRITE(0x40,0xA1,0x6E);
	I2C_WRITE(0x40,0xA2,0x72);
	I2C_WRITE(0x40,0xA3,0x65);
	I2C_WRITE(0x40,0xA4,0x00);
	I2C_WRITE(0x40,0xA5,0x80);
	I2C_WRITE(0x40,0xA6,0x00);
	I2C_WRITE(0x40,0xA7,0x70);
	I2C_WRITE(0x40,0xA8,0x00);
	I2C_WRITE(0x40,0xA9,0x1E);
	I2C_WRITE(0x40,0xAA,0x2E);
	I2C_WRITE(0x40,0xAB,0x00);
//	I2C_WRITE(0x40,0xAC,0x48);	//Read only
//	I2C_WRITE(0x40,0xAD,0xEE);	//Read only
//	I2C_WRITE(0x40,0xAE,0xEE);	//Read only
	I2C_WRITE(0x40,0xAF,0x99);

	I2C_WRITE(0x40,0xB0,0x00);
	I2C_WRITE(0x40,0xB1,0x00);
	I2C_WRITE(0x40,0xB2,0x00);
	I2C_WRITE(0x40,0xB3,0x00);
	I2C_WRITE(0x40,0xB4,0x00);
	I2C_WRITE(0x40,0xB5,0x00);
	I2C_WRITE(0x40,0xB6,0x00);
	I2C_WRITE(0x40,0xB8,0x00);
	I2C_WRITE(0x40,0xB9,0x00);
	I2C_WRITE(0x40,0xBA,0x00);
	I2C_WRITE(0x40,0xBB,0x00);
	I2C_WRITE(0x40,0xBC,0x90);
	I2C_WRITE(0x40,0xBD,0x01);
	I2C_WRITE(0x40,0xBE,0xA5);
	
	I2C_WRITE(0x40,0xC0,0x01);
	I2C_WRITE(0x40,0xC1,0x20);
	I2C_WRITE(0x40,0xC2,0x20);
	I2C_WRITE(0x40,0xC3,0x20);
	I2C_WRITE(0x40,0xC4,0x20);
	I2C_WRITE(0x40,0xC5,0x20);
	
	I2C_WRITE(0x40,0xD0,0xFF);
	I2C_WRITE(0x40,0xD1,0x3F);
	I2C_WRITE(0x40,0xD2,0xC2);
	I2C_WRITE(0x40,0xD3,0x3F);
	I2C_WRITE(0x40,0xD4,0x00);
	I2C_WRITE(0x40,0xD5,0x00);
	I2C_WRITE(0x40,0xD6,0x01);
	I2C_WRITE(0x40,0xD7,0x10);
	I2C_WRITE(0x40,0xD8,0x00);
	I2C_WRITE(0x40,0xDC,0x77);
//	I2C_WRITE(0x40,0xDD,0x00);	//Read only
//	I2C_WRITE(0x40,0xDE,0xAF);	//Read only
//	I2C_WRITE(0x40,0xDF,0xEF);	//Read only

	I2C_WRITE(0x40,0xE0,0x00);
	I2C_WRITE(0x40,0xE1,0x00);
	I2C_WRITE(0x40,0xE2,0x00);
	I2C_WRITE(0x40,0xE3,0x12);
	I2C_WRITE(0x40,0xE4,0x6A);
	I2C_WRITE(0x40,0xE5,0x80);
	I2C_WRITE(0x40,0xE6,0x00);
	I2C_WRITE(0x40,0xE7,0x00);
	I2C_WRITE(0x40,0xE8,0x00);
	I2C_WRITE(0x40,0xE9,0x00);
	I2C_WRITE(0x40,0xEA,0x00);
	I2C_WRITE(0x40,0xEB,0x00);
	I2C_WRITE(0x40,0xEC,0x88);	//WIN_Y_SHIFT[11:8] | WIN_X_SHIFT[11:8]
	I2C_WRITE(0x40,0xED,0x00);	//WIN_Y_SHIFT[7:0]
	I2C_WRITE(0x40,0xEE,0x00);	//WIN_X_SHIFT[7:0]
	I2C_WRITE(0x40,0xEF,0x00);
	
	//I2C_WRITE(0x40,0xF0,0x32);
	I2C_WRITE(0x40,0xF0,0x86);
	I2C_WRITE(0x40,0xF1,0x43);
	I2C_WRITE(0x40,0xF2,0x1F);
	I2C_WRITE(0x40,0xF3,0x1F);
	I2C_WRITE(0x40,0xF4,0x22);
	I2C_WRITE(0x40,0xF5,0x73);
	I2C_WRITE(0x40,0xF6,0x53);
	I2C_WRITE(0x40,0xF7,0x80);
	I2C_WRITE(0x40,0xF8,0x01);
	I2C_WRITE(0x40,0xF9,0x01);
	I2C_WRITE(0x40,0xFA,0x00);
	I2C_WRITE(0x40,0xFB,0x00);
	I2C_WRITE(0x40,0xFC,0x00);
	I2C_WRITE(0x40,0xFD,0x00);
	I2C_WRITE(0x40,0xFE,0x00);
	I2C_WRITE(0x40,0xFF,0x00);
#else	//in1080_out1080_mode12_w0-24bit-out-8bit-in
	I2C_WRITE(0x40,0x00,0x42);
	I2C_WRITE(0x40,0x01,0x00);
	I2C_WRITE(0x40,0x02,0x00);
	I2C_WRITE(0x40,0x03,0x75);
	I2C_WRITE(0x40,0x04,0x18);
	I2C_WRITE(0x40,0x05,0x80);
	I2C_WRITE(0x40,0x06,0x20);
	I2C_WRITE(0x40,0x07,0x0C);
	I2C_WRITE(0x40,0x08,0x21);
	I2C_WRITE(0x40,0x09,0x0A);
	I2C_WRITE(0x40,0x0A,0x21);
	I2C_WRITE(0x40,0x0B,0x0A);
	I2C_WRITE(0x40,0x0C,0x00);
	I2C_WRITE(0x40,0x0D,0x20);
	I2C_WRITE(0x40,0x0E,0x10);
	I2C_WRITE(0x40,0x0F,0x00);
	
	I2C_WRITE(0x40,0x10,0x80);
	//I2C_WRITE(0x40,0x11,0x03);
	I2C_WRITE(0x40,0x11,0x83);
	I2C_WRITE(0x40,0x12,0x00);
	I2C_WRITE(0x40,0x13,0x00);
	I2C_WRITE(0x40,0x14,0x00);
	I2C_WRITE(0x40,0x15,0x78);
	I2C_WRITE(0x40,0x16,0xFE);
	I2C_WRITE(0x40,0x17,0x00);
	I2C_WRITE(0x40,0x18,0xC0);
	I2C_WRITE(0x40,0x19,0x00);
	I2C_WRITE(0x40,0x1A,0x26);
//	I2C_WRITE(0x40,0x1B,0x30);	//Read only
	I2C_WRITE(0x40,0x1C,0x00);
	I2C_WRITE(0x40,0x1D,0x00);
	I2C_WRITE(0x40,0x1E,0x20);
	I2C_WRITE(0x40,0x1F,0x00);
	
	I2C_WRITE(0x40,0x20,0x00);
	I2C_WRITE(0x40,0x21,0x00);
//	I2C_WRITE(0x40,0x22,0x87);	//Read only
//	I2C_WRITE(0x40,0x23,0x98);	//Read only
//	I2C_WRITE(0x40,0x24,0x80);	//Read only
//	I2C_WRITE(0x40,0x25,0x44);	//Read only
//	I2C_WRITE(0x40,0x26,0x65);	//Read only
//	I2C_WRITE(0x40,0x27,0x38);	//Read only
	I2C_WRITE(0x40,0x28,0xF0);
	I2C_WRITE(0x40,0x29,0x00);
	I2C_WRITE(0x40,0x2A,0x80);
	I2C_WRITE(0x40,0x2B,0x01);
	I2C_WRITE(0x40,0x2C,0x80);
	I2C_WRITE(0x40,0x2D,0x00);
//	I2C_WRITE(0x40,0x2E,0x12);	//Read only
//	I2C_WRITE(0x40,0x2F,0x82);	//Read only

//	I2C_WRITE(0x40,0x30,0x6B);	//Read only
	I2C_WRITE(0x40,0x31,0x80);
	I2C_WRITE(0x40,0x32,0x80);
	I2C_WRITE(0x40,0x33,0x80);
	I2C_WRITE(0x40,0x34,0xA0);
	I2C_WRITE(0x40,0x35,0x18);
	I2C_WRITE(0x40,0x36,0x46);
	I2C_WRITE(0x40,0x37,0x64);
	I2C_WRITE(0x40,0x38,0x00);
	I2C_WRITE(0x40,0x39,0xFF);
	I2C_WRITE(0x40,0x3A,0x7F);
	I2C_WRITE(0x40,0x3B,0x80);
	I2C_WRITE(0x40,0x3C,0x80);
	I2C_WRITE(0x40,0x3D,0x80);
	I2C_WRITE(0x40,0x3E,0x80);
	I2C_WRITE(0x40,0x3F,0x80);
	
	I2C_WRITE(0x40,0x40,0x80);
	I2C_WRITE(0x40,0x41,0x60);
	I2C_WRITE(0x40,0x42,0x5F);
	I2C_WRITE(0x40,0x43,0x37);
	I2C_WRITE(0x40,0x44,0x14);
	I2C_WRITE(0x40,0x45,0x00);
	I2C_WRITE(0x40,0x46,0x00);
	I2C_WRITE(0x40,0x47,0x00);
	I2C_WRITE(0x40,0x48,0x00);
	I2C_WRITE(0x40,0x49,0x00);
	I2C_WRITE(0x40,0x4A,0x00);
	I2C_WRITE(0x40,0x4B,0xB2);
	I2C_WRITE(0x40,0x4C,0x32);
	I2C_WRITE(0x40,0x4D,0x32);
	I2C_WRITE(0x40,0x4E,0x32);
	I2C_WRITE(0x40,0x4F,0x7F);
	
	I2C_WRITE(0x40,0x50,0x20);
	I2C_WRITE(0x40,0x51,0x05);
	I2C_WRITE(0x40,0x52,0x10);
	I2C_WRITE(0x40,0x53,0x46);
	I2C_WRITE(0x40,0x54,0x40);
	I2C_WRITE(0x40,0x55,0x74);
	I2C_WRITE(0x40,0x56,0x3D);
	I2C_WRITE(0x40,0x57,0x04);
	I2C_WRITE(0x40,0x58,0x00);
	I2C_WRITE(0x40,0x59,0x08);
	I2C_WRITE(0x40,0x5A,0x10);
	I2C_WRITE(0x40,0x5B,0x00);
	I2C_WRITE(0x40,0x5C,0x68);
	I2C_WRITE(0x40,0x5D,0x50);
	I2C_WRITE(0x40,0x5E,0x4A);
	I2C_WRITE(0x40,0x5F,0x40);
	
	I2C_WRITE(0x40,0x60,0x74);
	I2C_WRITE(0x40,0x61,0x51);
	I2C_WRITE(0x40,0x62,0x33);
	I2C_WRITE(0x40,0x63,0x01);
	I2C_WRITE(0x40,0x64,0xFF);
	I2C_WRITE(0x40,0x65,0x01);
	I2C_WRITE(0x40,0x66,0xFF);
	I2C_WRITE(0x40,0x67,0x89);
	I2C_WRITE(0x40,0x68,0xAB);
	I2C_WRITE(0x40,0x69,0x88);
	I2C_WRITE(0x40,0x6A,0x6E);
	I2C_WRITE(0x40,0x6B,0x6E);
	I2C_WRITE(0x40,0x6C,0x6E);
	I2C_WRITE(0x40,0x6D,0x00);
	I2C_WRITE(0x40,0x6E,0x00);
	I2C_WRITE(0x40,0x6F,0x00);
	
	I2C_WRITE(0x40,0x70,0x00);
	I2C_WRITE(0x40,0x71,0x00);
	I2C_WRITE(0x40,0x72,0x00);
	I2C_WRITE(0x40,0x73,0x00);
	//I2C_WRITE(0x40,0x74,0x00);
	I2C_WRITE(0x40,0x74,0x02);
	I2C_WRITE(0x40,0x75,0xFF);
	I2C_WRITE(0x40,0x76,0xFF);
	I2C_WRITE(0x40,0x77,0xFF);
	I2C_WRITE(0x40,0x78,0xFF);
	I2C_WRITE(0x40,0x79,0xFF);
	I2C_WRITE(0x40,0x7A,0xFF);
	I2C_WRITE(0x40,0x7B,0xFF);
	I2C_WRITE(0x40,0x7C,0xFF);
	I2C_WRITE(0x40,0x7D,0xF0);
	I2C_WRITE(0x40,0x7E,0x80);
	I2C_WRITE(0x40,0x7F,0x00);
	
	I2C_WRITE(0x40,0x80,0x00);
	I2C_WRITE(0x40,0x81,0x34);	//It must be 0x00.
	I2C_WRITE(0x40,0x82,0x00);
	I2C_WRITE(0x40,0x83,0x05);
	I2C_WRITE(0x40,0x84,0x00);	//HSHIFT[7:0]
	I2C_WRITE(0x40,0x85,0xFD);
	//I2C_WRITE(0x40,0x85,0xD0);
	I2C_WRITE(0x40,0x86,0x04);
	I2C_WRITE(0x40,0x87,0x24);
	I2C_WRITE(0x40,0x88,0x94);
	I2C_WRITE(0x40,0x89,0x58);
	I2C_WRITE(0x40,0x8A,0x00);
	I2C_WRITE(0x40,0x8B,0x7E);
	I2C_WRITE(0x40,0x8C,0x03);
	I2C_WRITE(0x40,0x8D,0x2C);
	I2C_WRITE(0x40,0x8E,0x8C);	//HSHIFT[10:8] | VSHIFT[10:8] | BOTTOM_MASK[9:8]
	//I2C_WRITE(0x40,0x8E,0x84);
	I2C_WRITE(0x40,0x8F,0x00);
	
	I2C_WRITE(0x40,0x90,0xCC);
	I2C_WRITE(0x40,0x91,0x41);
	I2C_WRITE(0x40,0x92,0xC4);
	I2C_WRITE(0x40,0x93,0x84);
	I2C_WRITE(0x40,0x94,0x98);
	I2C_WRITE(0x40,0x95,0x80);
//	I2C_WRITE(0x40,0x96,0xEE);	//Read only
	I2C_WRITE(0x40,0x97,0x70);
	I2C_WRITE(0x40,0x98,0x29);
	I2C_WRITE(0x40,0x99,0x90);
	I2C_WRITE(0x40,0x9A,0x05);
	I2C_WRITE(0x40,0x9B,0x64);	//WIN_MODE
	//I2C_WRITE(0x40,0x9B,0x00);
	I2C_WRITE(0x40,0x9C,0x48);
	I2C_WRITE(0x40,0x9D,0x00);
	I2C_WRITE(0x40,0x9E,0x04);
	I2C_WRITE(0x40,0x9F,0x65);
	
	I2C_WRITE(0x40,0xA0,0x80);
	I2C_WRITE(0x40,0xA1,0xD8);
	I2C_WRITE(0x40,0xA2,0x98);
	I2C_WRITE(0x40,0xA3,0x87);
	I2C_WRITE(0x40,0xA4,0x00);
	I2C_WRITE(0x40,0xA5,0x80);
	I2C_WRITE(0x40,0xA6,0x00);
	I2C_WRITE(0x40,0xA7,0x70);
	I2C_WRITE(0x40,0xA8,0x40);
	I2C_WRITE(0x40,0xA9,0x1E);
	I2C_WRITE(0x40,0xAA,0x2E);
	I2C_WRITE(0x40,0xAB,0x00);
//	I2C_WRITE(0x40,0xAC,0x48);	//Read only
//	I2C_WRITE(0x40,0xAD,0xEE);	//Read only
//	I2C_WRITE(0x40,0xAE,0xEE);	//Read only
	I2C_WRITE(0x40,0xAF,0x99);

	I2C_WRITE(0x40,0xB0,0x00);
	I2C_WRITE(0x40,0xB1,0x00);
	I2C_WRITE(0x40,0xB2,0x00);
	I2C_WRITE(0x40,0xB3,0x00);
	I2C_WRITE(0x40,0xB4,0x00);
	I2C_WRITE(0x40,0xB5,0x00);
	I2C_WRITE(0x40,0xB6,0x00);
	I2C_WRITE(0x40,0xB7,0x00);
	I2C_WRITE(0x40,0xB8,0x01);
	I2C_WRITE(0x40,0xB9,0xF3);
	I2C_WRITE(0x40,0xBA,0x04);
	I2C_WRITE(0x40,0xBB,0x25);
	I2C_WRITE(0x40,0xBC,0x5C);
	I2C_WRITE(0x40,0xBD,0x04);
	I2C_WRITE(0x40,0xBE,0x51);
	I2C_WRITE(0x40,0xBF,0x00);
	
	I2C_WRITE(0x40,0xC0,0x00);
	I2C_WRITE(0x40,0xC1,0x20);
	I2C_WRITE(0x40,0xC2,0x20);
	I2C_WRITE(0x40,0xC3,0x20);
	I2C_WRITE(0x40,0xC4,0x20);
	I2C_WRITE(0x40,0xC5,0x20);
	I2C_WRITE(0x40,0xC6,0x00);
	I2C_WRITE(0x40,0xC7,0x00);
	I2C_WRITE(0x40,0xC8,0x00);
	I2C_WRITE(0x40,0xC9,0x00);
	I2C_WRITE(0x40,0xCA,0x00);
	I2C_WRITE(0x40,0xCB,0x00);
	I2C_WRITE(0x40,0xCC,0x00);
	I2C_WRITE(0x40,0xCD,0x00);
	I2C_WRITE(0x40,0xCE,0x00);
	I2C_WRITE(0x40,0xCF,0x00);
	
	I2C_WRITE(0x40,0xD0,0xFF);
	I2C_WRITE(0x40,0xD1,0x3F);
	I2C_WRITE(0x40,0xD2,0xC2);
	I2C_WRITE(0x40,0xD3,0x3F);
	I2C_WRITE(0x40,0xD4,0x00);
	I2C_WRITE(0x40,0xD5,0x00);
	I2C_WRITE(0x40,0xD6,0x00);
	I2C_WRITE(0x40,0xD7,0x00);
	I2C_WRITE(0x40,0xD8,0x00);
	I2C_WRITE(0x40,0xD9,0x00);
	I2C_WRITE(0x40,0xDA,0x00);
	I2C_WRITE(0x40,0xDB,0x00);
	I2C_WRITE(0x40,0xDC,0x77);
//	I2C_WRITE(0x40,0xDD,0x00);	//Read only
//	I2C_WRITE(0x40,0xDE,0xAF);	//Read only
//	I2C_WRITE(0x40,0xDF,0xEF);	//Read only

	I2C_WRITE(0x40,0xE0,0x00);
	I2C_WRITE(0x40,0xE1,0x00);
	I2C_WRITE(0x40,0xE2,0x00);
	I2C_WRITE(0x40,0xE3,0x43);
	I2C_WRITE(0x40,0xE4,0x3C);
	I2C_WRITE(0x40,0xE5,0xC0);
	I2C_WRITE(0x40,0xE6,0x00);
	I2C_WRITE(0x40,0xE7,0x00);
	I2C_WRITE(0x40,0xE8,0x00);
	I2C_WRITE(0x40,0xE9,0x00);
	I2C_WRITE(0x40,0xEA,0x00);
	I2C_WRITE(0x40,0xEB,0x00);
	I2C_WRITE(0x40,0xEC,0x88);	//WIN_Y_SHIFT[11:8] | WIN_X_SHIFT[11:8]
	I2C_WRITE(0x40,0xED,0xB5);	//WIN_Y_SHIFT[7:0]
	I2C_WRITE(0x40,0xEE,0x00);	//WIN_X_SHIFT[7:0]
	I2C_WRITE(0x40,0xEF,0x00);
	
	I2C_WRITE(0x40,0xF0,0x32);
	I2C_WRITE(0x40,0xF1,0x43);
	I2C_WRITE(0x40,0xF2,0x1F);
	I2C_WRITE(0x40,0xF3,0x1F);
	I2C_WRITE(0x40,0xF4,0x22);
	I2C_WRITE(0x40,0xF5,0x73);
	I2C_WRITE(0x40,0xF6,0x53);
	I2C_WRITE(0x40,0xF7,0x80);
	I2C_WRITE(0x40,0xF8,0x01);
	I2C_WRITE(0x40,0xF9,0x01);
	I2C_WRITE(0x40,0xFA,0x00);
	I2C_WRITE(0x40,0xFB,0x00);
	I2C_WRITE(0x40,0xFC,0x00);
	I2C_WRITE(0x40,0xFD,0x70);
	I2C_WRITE(0x40,0xFE,0x50);
	I2C_WRITE(0x40,0xFF,0x07);
#endif

}
