/********************************************************************************
*
*  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
*  Module		: The decoder's video format module
*  Description	: Clock information
*  Author		:
*  Date         :
*  Version		: Version 1.0
*
********************************************************************************
*  History      :
*
*
********************************************************************************/
#ifndef _RAPTOR3_CLOCK_
#define _RAPTOR3_CLOCK_

//FPGA Test
#define	NC_VD_FPGA_CLK_VCLK_148500000_PHASE1	0x60
#define	NC_VD_FPGA_CLK_VCLK_74250000_PHASE1		0x40

//AUTO VOUT CLK SET Test
#define	AUTO_VCLK_SET 1

typedef struct _NC_VD_CLK_VO_STR{
    unsigned char Port;				// PORT_A, PORTB, PORTC, PORTD
    unsigned char devnum;
    unsigned char ClkSel;			// [7:4] Port Clock SEL, [3:0] Port Clock Delay
}NC_VD_CLK_VO_STR;

typedef struct _NC_VD_CLK_STR{
	unsigned char ch;
	unsigned char devnum;
	unsigned char value;
}NC_VD_CLK_STR;

#endif
