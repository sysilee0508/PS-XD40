// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "VS4210.h"
// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
#define VS4210_FULL_MODE1 0x10

#define _OSD_1920X1080P		17
#define _OSD_No_Signal		21

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
T_VS4210_JointKind VS4210_JointKind1 ;
// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
tiByte bSignal[4]= {0,0,0,0} ;
tiByte gbVXIS_InputMode[4] ;
tiByte gbVXIS_OuputModeflg = VS4210_FULL_MODE1;
tByte gaHDMI_MapWindowIndex[4] = {0,1,2,3} ;
tiByte gbVXIS_OuputSize =  _OUTPUT_1080P60;
bool EnDrawOSD = 1 ;


tByte gHDMI_Index = 0 ;
unsigned short gHActive[4], gVActive[4] ;
unsigned short gHTotal[4], gVTotal[4] ;
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------
void msleep(unsigned int i )
{
	Delay_ms(i);
}

//--------------------------------------------------------------------------------------------------------------------------
tByte I2CRead(unsigned char maddr,unsigned char saddr)
{
	unsigned char receive_data;

	I2C_Start();

	I2C_P2S(maddr&0xFE);
	AckDetect();

	I2C_P2S(saddr);
	AckDetect();

	I2C_Start(); 
	I2C_P2S(maddr|0x01);
	AckDetect();

	receive_data = I2C_S2P();
	NotAck();

	I2C_Stop();

	return receive_data;

} 

//--------------------------------------------------------------------------------------------------------------------------
bit I2CWrite(unsigned char maddr,unsigned char saddr,unsigned char value)
{
	I2C_Start();

	I2C_P2S(maddr&0xFE);
	AckDetect();

	I2C_P2S(saddr);
	AckDetect();

	I2C_P2S((BYTE)((value >> 8)&0x000000ff));
	AckDetect();
	I2C_P2S((BYTE)((value)&0x000000ff));
	AckDetect();

	I2C_Stop();
	
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
		I2C_P2S((BYTE)((val >> 8)&0x000000ff));
		AckDetect();
		I2C_P2S((BYTE)((val)&0x000000ff));
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
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_34(void )
{
}

//--------------------------------------------------------------------------------------------------------------------------
void OSD_Clear_4(void )
{
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
}

//--------------------------------------------------------------------------------------------------------------------------
void vs4210_system_init()
{
	unsigned int i;

	I2CWrite(VS4210_ADDR, 0x01 ,0x5A) ;  //reset
	msleep(100) ;

	VS4210_Line_Clear() ;
	VS4210_Line_Draw(gbVXIS_OuputModeflg ) ;

	OSD_Clear_All();
	VS4210_StartInit();
	//VS4210_No_Signal_Init();

	VS4210_JointKind1.WindowMap.WMap0 = 1 ;
	VS4210_JointKind1.WindowMap.WMap1 = 2 ;
	VS4210_JointKind1.WindowMap.WMap2 = 3 ;
	VS4210_JointKind1.WindowMap.WMap3 = 4 ;
	VS4210_JointKind1.OutputSize = gbVXIS_OuputSize ;
	VS4210_JointKind1.OutputMode = VS4210_FULL_MODE1;

#if 1//for VS4210 output test
	for (i = 0; i < 4; i++)
	{
		VXIS_fShowInOutMode(i, _OSD_No_Signal , gbVXIS_OuputModeflg ) ;

	}

	VS4210_displayon();
#endif

	//VS4210_VideoJoin_Output(&VS4210_JointKind1) ;


}
