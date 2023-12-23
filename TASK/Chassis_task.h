/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	头文件:底盘控制任务												  (   )                      /)
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
#include "struct_typedef.h"

#define REMOTE_CONTROL 1
#define GYROSCOPE 2
#define STOP 3
#define TEACH 4
#define GET_DATA 5
#define SELF_MOTION 6





typedef struct
{

uint8_t Mode;//底盘运动模式
	
short Vx_set,Vy_set,Wz_set;//X,Y轴运动和原地转向的设定值
fp32 yaw_set;
int motor_temp[4];//四个电坤的期望值
int motor_out[4];//四个电坤的最终输出
	

}chassis_t;






void Chassis_task(void const *pvParameters);

void Speed_Caculation(chassis_t *Chassis);

void Chassis_information_update(chassis_t *Chassis);

void Chassis_SetMode(chassis_t *Chassis);



