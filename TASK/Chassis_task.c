/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	源文件:底盘控制任务												  (   )                      /)
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
//#include "arm_math.h"   //ARM的数学库貌似引不进来，你可以尝试一下。

chassis_t chassis;
pid_type_def motor_v;//开环控制的pid结构体（速度闭环，位置开环）
pid_type_def Trace_v[4];//闭环控制的pid结构体（速度、位置全闭环）


const RC_ctrl_t *RC;//引入遥控器的值
extern motor_measure_t motor_chassis[7];//CAN总线返回的电机状态结构体
extern chassis_angle_t Chassis_angle;//底盘角度结构体
extern TIM_HandleTypeDef htim4;
extern uint8_t buzzer_flag;//传给蜂鸣器提示任务的标志
const static fp32 chassis_pid[3]={4,0,0};//（速度闭环，位置开环）控制的pid参数
const static fp32 Trace_pid[3]={2,0,0.5};//（速度、位置全闭环）控制的pid参数
extern uint8_t Action_init_finish;
  
void Chassis_task(void const *pvParameters)
{
	
    vTaskDelay(750);
    buzzer_flag=4;//开机音乐	
	while(!Action_init_finish)
	{vTaskDelay(10);}
	
	buzzer_flag=3;//提示Action初始化完成
	
	
//下面的这些初始化函数放在main函数里，不要像我一样
	
//	*******************************	*******************************	*******************************	
    RC = get_remote_control_point();//将遥控器的值所在的地址传给RC
	remote_control_init();//遥控器接收缓冲区初始化
	
    PID_init(&motor_v,PID_POSITION,chassis_pid,12000,0);//pid控制器初始化
	PID_init(&Trace_v[0],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[1],PID_POSITION,Trace_pid,4000,0);    
	PID_init(&Trace_v[2],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[3],PID_POSITION,Trace_pid,4000,0);
//********************************	*******************************	*******************************	
	
	
	while(1)
	{
	Chassis_SetMode(&chassis);//设置底盘运动模式
    Chassis_information_update(&chassis);//数据更新

	if(chassis.Mode==STOP)
	{
		Can_send_data(0,0,0,0);
	}

	else
	{
		
		if(emergency_brake_flag==0)//紧急制动标志位为0
		{
			Speed_Caculation(&chassis);//全向轮运动学逆解
			
			if(chassis.Mode==REMOTE_CONTROL || chassis.Mode==TEACH || chassis.Mode==GYROSCOPE)//这三种均为遥控器控制
			{
				for(uint8_t i=0;i<4;i++)
				{
				chassis.motor_out[i]=(int16_t)PID_calc(&motor_v,motor_chassis[i].speed_rpm,chassis.motor_temp[i]);//计算每个电机的输出值
				}
			}
			else if(chassis.Mode==SELF_MOTION)	//这是自动模式，其PID参数与开环时不同，所以要单独分出来
			{
				for(uint8_t c=0;c<4;c++)
				{
				chassis.motor_out[c]=(int16_t)PID_calc(&Trace_v[c],motor_chassis[c].speed_rpm,chassis.motor_temp[c]);
				}

			}

			Can_send_data(chassis.motor_out[0],chassis.motor_out[1],chassis.motor_out[2],chassis.motor_out[3]);
		}
		else//触发紧急制动
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
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis_angle.yaw*40;//自动运行时强行锁定朝向
		
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

		speed_x=flag_data.all_point[flag_index][0]-Action_now.x;//自动运行时，期望值是下一个点的坐标，反馈值是当前位置
		speed_y=flag_data.all_point[flag_index][1]-Action_now.y;	
    	Chassis->Vx_set=speed_x*10;
		Chassis->Vy_set=speed_y*10;
	
	}	
	
	else if(Chassis->Mode==GYROSCOPE)
	{
	    Chassis->Vx_set=RC->rc.ch[0]*3;
		Chassis->Vy_set=RC->rc.ch[1]*3;
		Chassis->Wz_set=RC->rc.ch[2]*3;
		
		Chassis->Vy_set=Chassis->Vy_set*cos(Chassis_angle.yaw_rad)-Chassis->Vx_set*sin(Chassis_angle.yaw_rad);//小陀螺，但不完全是
		Chassis->Vx_set=Chassis->Vy_set*sin(Chassis_angle.yaw_rad)+Chassis->Vx_set*cos(Chassis_angle.yaw_rad);
	}

}

void Chassis_SetMode(chassis_t *Chassis)
{
	if(RC->rc.s[1]==0x03 && RC->rc.s[0]==0x03)//根据遥控器上面两个键来设置模式
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
	
	if(Chassis->Mode!=SELF_MOTION)//示教时，用这个东西来定点，三挡高度
	{
		if(RC->rc.ch[4]==0x294)//低
		{random_flag=1;}
		else
		{random_flag=0;}
		
		if(RC->rc.ch[4]==-0x294)//中
		{medium_flag=1;}
		 else
		{medium_flag=0;}		 
		
		if(RC->rc.ch[3]==0x294)//高
		{apex_flag=1;}
		else
		{apex_flag=0;}	
	}

}












