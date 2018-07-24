#include "common.h"
#include "Sequence.h"


// Dwell time은 각 채널당 분할화면당 각각 부여한다.(16분할, 20분할 제외)
// full, 16분할, 20분할 일 때는 1~20채널을 순차적으로 전환한다.

u8 bAuto_Seq_Flag = 0;	//Auto Seq 기능 ON/OFF
u8 vAuto_Seq_Cnt = 0;	
u8 vAuto_Seq_Index = 0; 


void Auto_Seq_Init(void)
{
	u8 Index_flag = 0;
	u8 vStart_CH; 
	u8 vMODE;
	
	if(sys_status.current_split_mode <= FULL_9)
	{
		vMODE = FULL_1CH;
		vStart_CH = sys_status.current_split_mode;
	}
	else if(sys_status.current_split_mode == SPLIT4_1)
	{
		vMODE = SPLIT_04;
		vStart_CH = 0;
	}
	else if(sys_status.current_split_mode == SPLIT4_2)
	{
		vMODE = SPLIT_04;
		vStart_CH = 4;
	}
	else if(sys_status.current_split_mode >= SPLIT9_1)
	{
		vMODE = SPLIT_09;
		vStart_CH = 0;
	}

	if(sys_env.bLossAutoSkip)
	{
#ifdef __4CH__
		if((vVideo_Loss&0x0000000f) == 0x0000000f) return; 
#endif
	}

	if(bAuto_Seq_Flag == 0)
	{
		vAuto_Seq_Index = 0;
	}
#ifdef __4CH__
	if(vMODE == SPLIT_04) vMODE = FULL_1CH;
#endif
	switch(vMODE)
	{
		case FULL_1CH:
		{
			vAuto_Seq_Cnt = sys_env.vDWELL[0];

			if(bAuto_Seq_Flag)
			{

#ifdef __4CH__
				if(vAuto_Seq_Index < FULL_4) vAuto_Seq_Index++;
				else vAuto_Seq_Index = FULL_1;
#endif
			}
			
			if(sys_env.bLossAutoSkip)
			{
				do
				{
					if(vVideo_Loss&(0x00000001<<vAuto_Seq_Index)) 
					{
#ifdef __4CH__
						if(vAuto_Seq_Index < FULL_4) vAuto_Seq_Index++;
						else vAuto_Seq_Index = FULL_1;
#endif
					}
					else Index_flag = 1; 
				
				}
				while(Index_flag == 0); 
			}	

			//Full 1채널 전환	
			vMODE = FULL_1CH; 
			vStart_CH = FULL_1+vAuto_Seq_Index; 

			if(vStart_CH < FULL_9)
			{
				sys_status.current_split_mode = vStart_CH;
				aux_display_flag = 0;
#if 0 //Louis
				FullScreen(vStart_CH);
#endif
				Set_border_line();
				DEMO_SetPIPViewWIND(0);	// update pip/pop window
			}
			else 
			{
				sys_status.current_split_mode = vStart_CH;
				Set_border_line();
				aux_display_flag = 1;
				DEMO_SetPIPViewWIND(0);	// update pip/pop window
			}
			
			if(sys_env.bOSD_Display)
			{
				if(sys_env.bTITLE_ON) {Erase_OSD(); OSG_Display_CH_name();}
			}
		}
		break;
		
		case SPLIT_04:
		{
			vAuto_Seq_Cnt = sys_env.vDWELL[1];

			if(bAuto_Seq_Flag)
			{
			}
			
			if(sys_env.bLossAutoSkip)
			{
				do
				{
					if((vVideo_Loss&(0x0000000f<<(vAuto_Seq_Index*4))) == (0x0000000f<<(vAuto_Seq_Index*4))) 
					{
					}
					else Index_flag = 1; 

				}
				while(Index_flag == 0); 
			}	

			//4분할 전환
			vMODE = SPLIT_04; 
			switch(vAuto_Seq_Index)
			{
			}

			if(vAuto_Seq_Index == 0)
				sys_status.current_split_mode = SPLIT4_2;
			else 
				sys_status.current_split_mode = SPLIT4_1;
			aux_display_flag = 0;
#if 0 //Louis
		    SGQ_4CH_INIT(change_mode[cmode]);
#endif
			Set_border_line();
			DEMO_SetPIPViewWIND(0);	// update pip/pop window


			if(sys_env.bOSD_Display)
			{
				if(sys_env.bTITLE_ON) {Erase_OSD(); OSG_Display_CH_name();}
			}
		}
		break;
	}

	bAuto_Seq_Flag = ON;
}


void Auto_Seq_Cnt(void)
{
    static u32 timeout = 0;

    if(!TIME_AFTER(tick_10ms,timeout))
        return;

    timeout = tick_10ms + 100; // 10ms * 100 = 1S

	if(vAuto_Seq_Cnt)  
	{
		vAuto_Seq_Cnt--;
	}
}


void Auto_Sequence(void)
{
	//static u8 vPre_vMODE = 0xff;
	static u8 vPre_vStart_CH = 0xff;

	u8 Index_flag = 0;
	u8 vStart_CH; 
	u8 vMODE;
	
	if(sys_status.current_split_mode <= FULL_9)
	{
		vMODE = FULL_1CH;
		vStart_CH = sys_status.current_split_mode;
	}
	else if(sys_status.current_split_mode == SPLIT4_1)
	{
		vMODE = SPLIT_04;
		vStart_CH = 0;
	}
	else if(sys_status.current_split_mode == SPLIT4_2)
	{
		vMODE = SPLIT_04;
		vStart_CH = 4;
	}
	else if(sys_status.current_split_mode >= SPLIT9_1)
	{
		vMODE = SPLIT_09;
		vStart_CH = 0;
	}

	if(bAuto_Seq_Flag == ON)	
	{
		if(sys_env.bLossAutoSkip)
		{
#ifdef __4CH__
			if(vVideo_Loss == 0x0000000f) return; 
#endif
		}
		
		if(vAuto_Seq_Cnt == 0)  
		{
			switch(vMODE)
			{
				case FULL_1CH:  
				{
					if(sys_env.vDWELL[0] == 0) return;

					vAuto_Seq_Cnt = sys_env.vDWELL[0];
#ifdef __4CH__
					if(vAuto_Seq_Index < FULL_4) vAuto_Seq_Index++;
					else vAuto_Seq_Index = FULL_1;
#endif
					if(sys_env.bLossAutoSkip)
					{
						do
						{
							if(vVideo_Loss&(0x00000001<<vAuto_Seq_Index)) 
							{
#ifdef __4CH__
								if(vAuto_Seq_Index < FULL_4) vAuto_Seq_Index++;
								else vAuto_Seq_Index = FULL_1;
#endif
							}
							else Index_flag = 1; 
						
						}
						while(Index_flag == 0); 
					}	

					//채널 전환
					vMODE = FULL_1CH; 
					vStart_CH = FULL_1+vAuto_Seq_Index; 

					if(vStart_CH < FULL_9)
					{
						sys_status.current_split_mode = vStart_CH;
#if 0 //Louis
						FullScreen(vStart_CH);
#endif
						//Set_border_line();
						aux_display_flag = 0;
						if(vPre_vStart_CH == FULL_9) DEMO_SetPIPViewWIND(0);	// update pip/pop window
					}
					else 
					{
						sys_status.current_split_mode = vStart_CH;
						//Set_border_line();
						aux_display_flag = 1;
						DEMO_SetPIPViewWIND(0);	// update pip/pop window

					}

					if(sys_env.bOSD_Display)
					{
						if(sys_env.bTITLE_ON) {Erase_OSD(); OSG_Display_CH_name();}
					}
				}
				break;

				case SPLIT_04:
				{
					vAuto_Seq_Cnt = sys_env.vDWELL[1];
					if(sys_env.bLossAutoSkip)
					{
						do
						{
							if((vVideo_Loss&(0x0000000f<<(vAuto_Seq_Index*4))) == (0x0000000f<<(vAuto_Seq_Index*4))) 
							{
							}
							else Index_flag = 1; 
						}
						while(Index_flag == 0); 
					}	
					//4분할 전환
					vMODE = SPLIT_04; 
					switch(vAuto_Seq_Index)
					{
					}

					if(vAuto_Seq_Index == 0)
						sys_status.current_split_mode = SPLIT4_2;
					else 
						sys_status.current_split_mode = SPLIT4_1;
#if 0 //Louis
					SGQ_4CH_INIT(change_mode[cmode]);
#endif
					//Set_border_line();
					//DEMO_SetPIPViewWIND(0);	// update pip/pop window

					if(sys_env.bOSD_Display)
					{
						if(sys_env.bTITLE_ON) {Erase_OSD(); OSG_Display_CH_name();}
					}
				}
				break;
			}
		}
	}

	//if(vPre_vMODE != vMODE) bMode_change_flag = 1;
	//if(vPre_vStart_CH != vStart_CH) bMode_change_flag = 1;  

	//vPre_vMODE = vMODE;		
	vPre_vStart_CH = vStart_CH;		
}

