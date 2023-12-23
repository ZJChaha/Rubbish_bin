/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Դ�ļ�:�߶ȿ�������												  (   )                      /)
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
//������־
//2023��12��5�գ��޸���֪bug�������ϵͳ��ȫ�ԡ��޸��˶ദע�ͣ���߿ɶ��ԡ�




#include "Altitude_control_task.h"
#include "tim.h"
#include "cmsis_os.h"
#include "Teach_task.h"
#include "struct_typedef.h"
#include "cmsis_os.h"
#include "remote_control.h"

#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))//�޷�

#define default_value_A 180//��ֻ���ʼ�Ƕ�(����)
#define default_value_B 8
#define default_value_C 163
#define default_value_D 80
#define default_value_E 97//������ʼ�Ƕ�(����)

#define medium_offset 25//�еȸ߶ȶ�Ӧ����ת�Ƕ�
#define apex_offset 68//���߶ȶ�Ӧ����ת�Ƕ�
#define open_offset 58//������ת�Ƕȣ����ǵ���270��������ô��ˣ����û��ˣ����ܾ��У�

servo_motor_t Servo_Motor;//�����ṹ�������������Ž�ȥ


extern RC_ctrl_t *RC;//����ң������ֵ������û���������������ڳ�bug���������ų����⣬�������øĻ�ȥ��

short count_1,count_2;

extern uint8_t Action_init_finish;



void Altitude_control_task(void const *pvParameters)//�߶ȿ�������
{

	while(!Action_init_finish)
	{vTaskDelay(10);}
	
	
	
	//��ʼ������Ƕ�
	Servo_Motor.A=default_value_A;
	Servo_Motor.B=default_value_B;
	Servo_Motor.C=default_value_C;
	Servo_Motor.D=default_value_D;	
    Servo_Motor.E=default_value_E;
	
	while(1)
    {
		
		
		
		
		if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x01)//ȫ�Զ�ģʽ�£���������������ģ�����if else�Ϳ��Ը㶨��ǰ����Ҫ��ʾ�������Э��(����λ��)
		{
			
			if(flag_data.special_point[flag_index][0]==flag_data.special_flag_projection[flag_index])
			{random_flag=1;}
			else
			{random_flag=0;}
			if(flag_data.special_point[flag_index][1]==flag_data.special_flag_projection[flag_index])
			{medium_flag=1;}
			else
			{medium_flag=0;}
			if(flag_data.special_point[flag_index][2]==flag_data.special_flag_projection[flag_index])
			{apex_flag=1;}
			else
			{apex_flag=0;}

			if(apex_flag==1 && medium_flag==0 && random_flag==0)
			{
				Servo_Motor.A=default_value_A-apex_offset;
				Servo_Motor.B=default_value_B+apex_offset;
				Servo_Motor.C=default_value_C-apex_offset;
				Servo_Motor.D=default_value_D+apex_offset;	
				Servo_Motor.E=default_value_E-open_offset;	
			}
			else if(medium_flag==1 && apex_flag==0 && random_flag==0)
			{
				Servo_Motor.A=default_value_A-medium_offset;
				Servo_Motor.B=default_value_B+medium_offset;
				Servo_Motor.C=default_value_C-medium_offset;
				Servo_Motor.D=default_value_D+medium_offset;
				Servo_Motor.E=default_value_E-open_offset;	
			}
			else if(random_flag==1 && medium_flag==0 && apex_flag==0)
			{
				Servo_Motor.A=default_value_A;
				Servo_Motor.B=default_value_B;
				Servo_Motor.C=default_value_C;
				Servo_Motor.D=default_value_D;
				Servo_Motor.E=default_value_E-open_offset;
			}
			else 	
			{
				Servo_Motor.A=default_value_A;
				Servo_Motor.B=default_value_B;
				Servo_Motor.C=default_value_C;
				Servo_Motor.D=default_value_D;	
				Servo_Motor.E=default_value_E;		
				
			}
		 }
		
		else if(RC->rc.s[1]==0x03)//�ֶ����Ƹ߶ȺͿ���
		{
		
			
			if(RC->rc.ch[3]>0xC8)				
				count_1=1;
			else if(RC->rc.ch[3]<-0xC8)				
				count_1=-1;				 
			else
				count_1=0;
			
			
			if(RC->rc.ch[4]>0xC8)				
				count_2=-1;
			else if(RC->rc.ch[4]<-0xC8)				
				count_2=1;				 
			else
				count_2=0;
			
			
			
			 
			Servo_Motor.A-=count_1;
			Servo_Motor.B+=count_1;
			Servo_Motor.C-=count_1;
			Servo_Motor.D+=count_1;
			Servo_Motor.E+=count_2;
			
			Servo_Motor.A=_constrain(Servo_Motor.A,default_value_A-apex_offset,default_value_A);
			Servo_Motor.B=_constrain(Servo_Motor.B,default_value_B,default_value_B+apex_offset);
			Servo_Motor.C=_constrain(Servo_Motor.C,default_value_C-apex_offset,default_value_C);
			Servo_Motor.D=_constrain(Servo_Motor.D,default_value_D,default_value_D+apex_offset);
			Servo_Motor.E=_constrain(Servo_Motor.E,default_value_E-open_offset,default_value_E);
			
			osDelay(20);
			


		}

	
	
	
	
	  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,angle_to_duty(Servo_Motor.A));
	  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,angle_to_duty(Servo_Motor.B));
	  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,angle_to_duty(Servo_Motor.C));
	  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,angle_to_duty(Servo_Motor.D));
      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,angle_to_duty(Servo_Motor.E));
		
		
		vTaskDelay(2);
		

    }



}







uint32_t angle_to_duty(uint32_t angle)//���Ƕ�ת����CCR��ֵ
{

    uint32_t x;
	x=11.11*angle+500;


	if(x>2500)
	{x=2500;}
	if(x<500)
	{x=500;}


	return x;
}








