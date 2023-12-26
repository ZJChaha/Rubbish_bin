/*****************************************************************************************************
*																		  ,~.
*	ÓÒ±ßÊÇÎÒ¼Ò¸ç¸ç£¬ÄÜ±£ÓÓ³ÌÐò²»³öbug£¬Ç§Íò±ð¶¯£¬Ð¡ÐÄËüÀ´×ÄÄã--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Ô´ÎÄ¼þ:µ×ÅÌ¿ØÖÆÈÎÎñ												  (   )                      /)
* 																	   `-'\   ,                    )
*	×÷Õß:ÕÅ¾û³Û															   |  \        `~.        /
*																		   \   `._        \      /
*	2023-7-8																\     `._____,'    ,'
*																			 `-.             ,'
*					 															`-._     _,-'
*																					77jj'
*																				   //_||
*																				__//--'/`
*																			  ,--'/`  '
******************************************************************************************************/
//¸üÐÂÈÕÖ¾
//2023Äê12ÔÂ5ÈÕ£¬ÐÞ¸´ÒÑÖªbug£¬Ìá¸ßÁËÏµÍ³°²È«ÐÔ¡£ÐÞ¸ÄÁË¶à´¦×¢ÊÍ£¬Ìá¸ß¿É¶ÁÐÔ¡£





#include "Chassis_task.h"
#include "struct_typedef.h"
#include "can.h"
#include "cmsis_os.h"
#include "usart.h"
#include "remote_control.h"
#include "INS_task.h"
#include "pid.h"
#include "Calc_yaw_task.h"
#include "Buzzer_task.h"
#include "math.h"
#include "Teach_task.h"
#include "Detect_task.h"
//#include "arm_math.h"   //ARMµÄÊýÑ§¿âÃ²ËÆÒý²»½øÀ´£¬Äã¿ÉÒÔ³¢ÊÔÒ»ÏÂ¡£

chassis_t chassis;
pid_type_def motor_v;//¿ª»·¿ØÖÆµÄpid½á¹¹Ìå£¨ËÙ¶È±Õ»·£¬Î»ÖÃ¿ª»·£©
pid_type_def Trace_v[4];//±Õ»·¿ØÖÆµÄpid½á¹¹Ìå£¨ËÙ¶È¡¢Î»ÖÃÈ«±Õ»·£©


const RC_ctrl_t *RC;//ÒýÈëÒ£¿ØÆ÷µÄÖµ
extern motor_measure_t motor_chassis[7];//CAN×ÜÏß·µ»ØµÄµç»ú×´Ì¬½á¹¹Ìå
extern chassis_angle_t Chassis_angle;//µ×ÅÌ½Ç¶È½á¹¹Ìå
extern TIM_HandleTypeDef htim4;
extern uint8_t buzzer_flag;//´«¸ø·äÃùÆ÷ÌáÊ¾ÈÎÎñµÄ±êÖ¾
const static fp32 chassis_pid[3]={4,0,0};//£¨ËÙ¶È±Õ»·£¬Î»ÖÃ¿ª»·£©¿ØÖÆµÄpid²ÎÊý
const static fp32 Trace_pid[3]={2,0,0.5};//£¨ËÙ¶È¡¢Î»ÖÃÈ«±Õ»·£©¿ØÖÆµÄpid²ÎÊý
extern uint8_t Action_init_finish;
  
void Chassis_task(void const *pvParameters)
{
	
    vTaskDelay(750);
   // buzzer_flag=4;//¿ª»úÒôÀÖ	
	while(!Action_init_finish)
	{vTaskDelay(10);}
	
	buzzer_flag=3;//ÌáÊ¾Action³õÊ¼»¯Íê³É
	
	
//ÏÂÃæµÄÕâÐ©³õÊ¼»¯º¯Êý·ÅÔÚmainº¯ÊýÀï£¬²»ÒªÏñÎÒÒ»Ñù
	
//	*******************************	*******************************	*******************************	
    RC = get_remote_control_point();//½«Ò£¿ØÆ÷µÄÖµËùÔÚµÄµØÖ·´«¸øRC
	remote_control_init();//Ò£¿ØÆ÷½ÓÊÕ»º³åÇø³õÊ¼»¯
	
    PID_init(&motor_v,PID_POSITION,chassis_pid,12000,0);//pid¿ØÖÆÆ÷³õÊ¼»¯
	PID_init(&Trace_v[0],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[1],PID_POSITION,Trace_pid,4000,0);    
	PID_init(&Trace_v[2],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[3],PID_POSITION,Trace_pid,4000,0);
//********************************	*******************************	*******************************	
	
	
	while(1)
	{
	Chassis_SetMode(&chassis);//ÉèÖÃµ×ÅÌÔË¶¯Ä£Ê½
    Chassis_information_update(&chassis);//Êý¾Ý¸üÐÂ

	if(chassis.Mode==STOP)
	{
		Can_send_data(0,0,0,0);
	}

	else
	{
		
		if(emergency_brake_flag==0)//½ô¼±ÖÆ¶¯±êÖ¾Î»Îª0
		{
			Speed_Caculation(&chassis);//È«ÏòÂÖÔË¶¯Ñ§Äæ½â
			
			if(chassis.Mode==REMOTE_CONTROL || chassis.Mode==TEACH || chassis.Mode==GYROSCOPE)//ÕâÈýÖÖ¾ùÎªÒ£¿ØÆ÷¿ØÖÆ
			{
				for(uint8_t i=0;i<4;i++)
				{
				chassis.motor_out[i]=(int16_t)PID_calc(&motor_v,motor_chassis[i].speed_rpm,chassis.motor_temp[i]);//¼ÆËãÃ¿¸öµç»úµÄÊä³öÖµ
				}
			}
			else if(chassis.Mode==SELF_MOTION)	//ÕâÊÇ×Ô¶¯Ä£Ê½£¬ÆäPID²ÎÊýÓë¿ª»·Ê±²»Í¬£¬ËùÒÔÒªµ¥¶À·Ö³öÀ´
			{
				for(uint8_t c=0;c<4;c++)
				{
				chassis.motor_out[c]=(int16_t)PID_calc(&Trace_v[c],motor_chassis[c].speed_rpm,chassis.motor_temp[c]);
				}

			}

			Can_send_data(chassis.motor_out[0],chassis.motor_out[1],chassis.motor_out[2],chassis.motor_out[3]);
		}
		else//´¥·¢½ô¼±ÖÆ¶¯
		{
			Can_send_data(0,0,0,0);
		}
		
	}


	
	 vTaskDelay(2);
		

	
	}
	
	
}



void Speed_Caculation(chassis_t *Chassis)
{
	if (Chassis->Mode==REMOTE_CONTROL)	
	{
     
		if(Chassis->Wz_set!=0)
		{	
	Chassis->motor_temp[0]=Chassis->Vy_set+Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[1]=Chassis->Vy_set-Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[2]=-Chassis->Vy_set-Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis->Wz_set;
		}
		else
		{
	Chassis->motor_temp[0]=Chassis->Vy_set+Chassis->Vx_set-Chassis_angle.yaw_err*600;
	Chassis->motor_temp[1]=Chassis->Vy_set-Chassis->Vx_set-Chassis_angle.yaw_err*600;
	Chassis->motor_temp[2]=-Chassis->Vy_set-Chassis->Vx_set-Chassis_angle.yaw_err*600;
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set-Chassis_angle.yaw_err*600;	
		}
	}	

	else if(Chassis->Mode==GYROSCOPE)
	{
	
	Chassis->motor_temp[0]=Chassis->Vy_set+Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[1]=Chassis->Vy_set-Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[2]=-Chassis->Vy_set-Chassis->Vx_set+Chassis->Wz_set;
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis->Wz_set;

	}

	else if(Chassis->Mode==SELF_MOTION)
	{

	Chassis->motor_temp[0]=Chassis->Vy_set+Chassis->Vx_set+Chassis_angle.yaw*40;
	Chassis->motor_temp[1]=Chassis->Vy_set-Chassis->Vx_set+Chassis_angle.yaw*40;
	Chassis->motor_temp[2]=-Chassis->Vy_set-Chassis->Vx_set+Chassis_angle.yaw*40;
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis_angle.yaw*40;//×Ô¶¯ÔËÐÐÊ±Ç¿ÐÐËø¶¨³¯Ïò
		
	}
	else if(Chassis->Mode==TEACH)
	{
		

		if(teach_stop_flag==1)
		{

		Chassis->motor_temp[0]=0;
		Chassis->motor_temp[1]=0; 
		Chassis->motor_temp[2]=0;
		Chassis->motor_temp[3]=0;	
		}

	    else
		{
			
		Chassis->motor_temp[0]=Chassis->Vy_set+Chassis->Vx_set+Chassis_angle.yaw*40;
		Chassis->motor_temp[1]=Chassis->Vy_set-Chassis->Vx_set+Chassis_angle.yaw*40;
		Chassis->motor_temp[2]=-Chassis->Vy_set-Chassis->Vx_set+Chassis_angle.yaw*40;
		Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis_angle.yaw*40;
		
		}
	
	
	}

}

void Chassis_information_update(chassis_t *Chassis)
{

	if(Chassis->Mode==REMOTE_CONTROL)
	{
		Chassis->Vx_set=RC->rc.ch[0]*3;
		Chassis->Vy_set=RC->rc.ch[1]*3;
		Chassis->Wz_set=RC->rc.ch[2]*3;
		
	}
   
	else if(Chassis->Mode==TEACH)
	{	
		Chassis->Vx_set=RC->rc.ch[0]*3;
		Chassis->Vy_set=RC->rc.ch[1]*3;
		Chassis->Wz_set=RC->rc.ch[2]*3;
	}
	
	else if(Chassis->Mode==SELF_MOTION)
	{

		speed_x=flag_data.all_point[flag_index][0]-Action_now.x;//×Ô¶¯ÔËÐÐÊ±£¬ÆÚÍûÖµÊÇÏÂÒ»¸öµãµÄ×ø±ê£¬·´À¡ÖµÊÇµ±Ç°Î»ÖÃ
		speed_y=flag_data.all_point[flag_index][1]-Action_now.y;	
    	Chassis->Vx_set=speed_x*10;
		Chassis->Vy_set=speed_y*10;
	
	}	
	
	else if(Chassis->Mode==GYROSCOPE)
	{
		float after_analysis_x = 0.0f, after_analysis_y = 0.0f;

		
	        Chassis->Vx_set=RC->rc.ch[0]*3;
		Chassis->Vy_set=RC->rc.ch[1]*3;
		Chassis->Wz_set=RC->rc.ch[2]*3;
		
		after_analysis_y=Chassis->Vy_set*cos(Chassis_angle.yaw_rad)-Chassis->Vx_set*sin(Chassis_angle.yaw_rad);
		after_analysis_x=Chassis->Vy_set*sin(Chassis_angle.yaw_rad)+Chassis->Vx_set*cos(Chassis_angle.yaw_rad);
		
		
		Chassis->Vy_set=after_analysis_y;
		Chassis->Vx_set=after_analysis_x;
		
	}

}

void Chassis_SetMode(chassis_t *Chassis)
{
	if(RC->rc.s[1]==0x03 && RC->rc.s[0]==0x03)//¸ù¾ÝÒ£¿ØÆ÷ÉÏÃæÁ½¸ö¼üÀ´ÉèÖÃÄ£Ê½
	  Chassis->Mode=REMOTE_CONTROL;
	else if(RC->rc.s[1]==0x03 && RC->rc.s[0]==0x02)
	  Chassis->Mode=GYROSCOPE;
	else if(RC->rc.s[1]==0x03 && RC->rc.s[0]==0x01)
	  Chassis->Mode=STOP;
	else if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x03)
	  Chassis->Mode=TEACH;
	else if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x02)
	  Chassis->Mode=GET_DATA;
	else if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x01)
	  Chassis->Mode=SELF_MOTION;
	
	if(Chassis->Mode!=SELF_MOTION)//Ê¾½ÌÊ±£¬ÓÃÕâ¸ö¶«Î÷À´¶¨µã£¬Èýµ²¸ß¶È
	{
		if(RC->rc.ch[4]==0x294)//µÍ
		{random_flag=1;}
		else
		{random_flag=0;}
		
		if(RC->rc.ch[4]==-0x294)//ÖÐ
		{medium_flag=1;}
		 else
		{medium_flag=0;}		 
		
		if(RC->rc.ch[3]==0x294)//¸ß
		{apex_flag=1;}
		else
		{apex_flag=0;}	
	}

}












