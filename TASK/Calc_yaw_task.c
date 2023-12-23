/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	源文件:校准YAW轴数据						     			 		  (   )                      /)
* 																	   `-'\   ,                    )
*	作者:张钧驰															   |  \        `~.        /
*																		   \   `._        \      /
*	2023-7-8																\     `._____,'    ,'
*																			 `-.             ,'
*					 															`-._     _,-'
*																					77jj'
*																				   //_||
*																				__//--'/`
*																			  ,--'/`  '
******************************************************************************************************/
#include "Calc_yaw_task.h"
#include "cmsis_os.h"
#include "INS_task.h"
#include "usart.h"


chassis_angle_t Chassis_angle;




void Calc_yaw_task(void const *pvParameters)//减缓零漂，解决数据跳变
{

	vTaskDelay(600);
	Chassis_angle.YAW=get_INS_angle_point();
	
	
	
   while(1)
{
  
	Chassis_angle.yaw=Action.yaw; //*Chassis_angle.YAW*RTA;        
	Chassis_angle.yaw_rad=Chassis_angle.yaw/RTA;
	Chassis_angle.yaw_C=*Chassis_angle.YAW*RTA; 
	if(fpabs(Chassis_angle.yaw_C-Chassis_angle.last_yaw)>25)
	{
	Chassis_angle.last_yaw=Chassis_angle.yaw_C;
	}
	
	Chassis_angle.last_yaw=Chassis_angle.this_yaw;
	Chassis_angle.this_yaw= Chassis_angle.yaw_C;
	
	Chassis_angle.yaw_last_err=Chassis_angle.yaw_err;
	
	
	if(Chassis_angle.this_yaw>160||Chassis_angle.this_yaw<-160)
	{
	
		if(Chassis_angle.last_yaw>0 && Chassis_angle.this_yaw<0)	
			Chassis_angle.yaw_err=360-(Chassis_angle.last_yaw-Chassis_angle.this_yaw);	
		else
		{
			if(Chassis_angle.last_yaw<0 && Chassis_angle.this_yaw>0)		
				Chassis_angle.yaw_err=360+(Chassis_angle.last_yaw-Chassis_angle.this_yaw);	
			else
				Chassis_angle.yaw_err=Chassis_angle.last_yaw-Chassis_angle.this_yaw;
		}
    }
	else
		Chassis_angle.yaw_err=Chassis_angle.last_yaw-Chassis_angle.this_yaw;

	
	if(fpabs(Chassis_angle.yaw_err)<0.07f)
	{
		Chassis_angle.yaw_err=0;   
	}
	Chassis_angle.yaw_err=Chassis_angle.yaw_last_err*0.6f+Chassis_angle.yaw_err*0.4f;
	

	vTaskDelay(10);
	
}	



}	



	fp32 fpabs(fp32 i)
	{
		fp32 m;
		m=(i>0)?i:-i;
		return m;	

	}





