/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Դ�ļ�:�������													  (   )                      /)
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




#include "Detect_task.h"
#include "Teach_task.h"
#include "Chassis_task.h"
#include "usart.h"
#include "cmsis_os.h"
#include "can.h"
#include "Buzzer_task.h"
#include "remote_control.h"


extern motor_measure_t motor_chassis[7];//�������ĸ���״̬
extern uint8_t buzzer_flag;
extern RC_ctrl_t *RC;//����ң������ֵ

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
			
		
	if(RC->rc.s[1]==0x02 && RC->rc.s[0]==0x01)//�Զ�ģʽ������
	{
	
		if(abs_int(X_err)>80 || abs_int(Y_err)>80)
			count_A++;//������ֵ�ı�ϴ������ֵ��һ
		else
			count_A=0;
		if(count_A>10)
		{
			buzzer_flag=2;
			emergency_brake_flag=1;//����ֵ�ﵽ10����һ��֮�󣬴��������������ƶ�����һ�㲻��ִ�е�����(��˵��)��
		}

	}
	
	if(RC->rc.s[1]==0x03 || RC->rc.s[1]==0x02)//Action���߼��͵������ռ��
	{
		if(motor_chassis[0].speed_rpm>10 && motor_chassis[1].speed_rpm>10 && motor_chassis[2].speed_rpm>10 && motor_chassis[3].speed_rpm>10)
		{	
			if(abs_int(X_err)==0 && abs_int(Y_err)==0)	//������ֵ�ޱ仯�������ֵ��һ
				count_B++;
			else
				count_B=0;
		}
		if(count_B>30)//����ֵ�ﵽ30��3��֮�󴥷������������ƶ�
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











