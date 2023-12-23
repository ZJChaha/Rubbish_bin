/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	ͷ�ļ�:���̿�������												  (   )                      /)
* 																	   `-'\   ,                    )
*	����:�ž���															   |  \        `~.        /
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

uint8_t Mode;//�����˶�ģʽ
	
short Vx_set,Vy_set,Wz_set;//X,Y���˶���ԭ��ת����趨ֵ
fp32 yaw_set;
int motor_temp[4];//�ĸ�����������ֵ
int motor_out[4];//�ĸ��������������
	

}chassis_t;






void Chassis_task(void const *pvParameters);

void Speed_Caculation(chassis_t *Chassis);

void Chassis_information_update(chassis_t *Chassis);

void Chassis_SetMode(chassis_t *Chassis);



