/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	源文件:检测任务													  (   )                      /)
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
//更新日志
//2023年12月5日，修复已知bug，提高了系统安全性。修改了多处注释，提高可读性。




#include "Detect_task.h"
#include "Teach_task.h"
#include "Chassis_task.h"
#include "usart.h"
#include "cmsis_os.h"
#include "can.h"
#include "Buzzer_task.h"
#include "remote_control.h"


extern motor_measure_t motor_chassis[7];//引入电机的各种状态
extern uint8_t buzzer_flag;
extern RC_ctrl_t *RC;//引入遥控器的值

uint8_t emergency_brake_flag;
int16_t X_err,last_X;
int16_t Y_err,last_Y;
uint8_t count_A,count_B;





void Detect_task(void const *pvParameters)
{

	emergency_brake_flag=0;
	
    vTaskDelay(15000);
	
	
	while(1)
	{
	
	X_err=Action_now.x-last_X;
	Y_err=Action_now.y-last_Y;	
			
		
	if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x01)//自动模式发疯检测
	{
	
		if(abs_int(X_err)>80 || abs_int(Y_err)>80)
			count_A++;//若坐标值改变较大，则计数值加一
		else
			count_A=0;
		if(count_A>10)
		{
			buzzer_flag=2;
			emergency_brake_flag=1;//计数值达到10，即一秒之后，触发警报并紧急制动，但一般不会执行到这里(我说的)。
		}

	}
	
	if(RC->rc.s[1]==0x03 || RC->rc.s[1]==0x02)//Action掉线检测和底盘悬空检测
	{
		if(motor_chassis[0].speed_rpm>10 && motor_chassis[1].speed_rpm>10 && motor_chassis[2].speed_rpm>10 && motor_chassis[3].speed_rpm>10)
		{	
			if(abs_int(X_err)==0 && abs_int(Y_err)==0)	//若坐标值无变化，则计数值加一
				count_B++;
			else
				count_B=0;
		}
		if(count_B>30)//计数值达到30即3秒之后触发警报并紧急制动
		{
			buzzer_flag=2;
			emergency_brake_flag=1;
		}
		
	}
	

	last_X=Action_now.x;
	last_Y=Action_now.y;
	
	vTaskDelay(100);
	
	}

}











