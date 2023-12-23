/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Դ�ļ�:���̿�������												  (   )                      /)
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
//#include "arm_math.h"   //ARM����ѧ��ò����������������Գ���һ�¡�

chassis_t chassis;
pid_type_def motor_v;//�������Ƶ�pid�ṹ�壨�ٶȱջ���λ�ÿ�����
pid_type_def Trace_v[4];//�ջ����Ƶ�pid�ṹ�壨�ٶȡ�λ��ȫ�ջ���


const RC_ctrl_t *RC;//����ң������ֵ
extern motor_measure_t motor_chassis[7];//CAN���߷��صĵ��״̬�ṹ��
extern chassis_angle_t Chassis_angle;//���̽ǶȽṹ��
extern TIM_HandleTypeDef htim4;
extern uint8_t buzzer_flag;//������������ʾ����ı�־
const static fp32 chassis_pid[3]={4,0,0};//���ٶȱջ���λ�ÿ��������Ƶ�pid����
const static fp32 Trace_pid[3]={2,0,0.5};//���ٶȡ�λ��ȫ�ջ������Ƶ�pid����
extern uint8_t Action_init_finish;
  
void Chassis_task(void const *pvParameters)
{
	
    vTaskDelay(750);
    buzzer_flag=4;//��������	
	while(!Action_init_finish)
	{vTaskDelay(10);}
	
	buzzer_flag=3;//��ʾAction��ʼ�����
	
	
//�������Щ��ʼ����������main�������Ҫ����һ��
	
//	*******************************	*******************************	*******************************	
    RC = get_remote_control_point();//��ң������ֵ���ڵĵ�ַ����RC
	remote_control_init();//ң�������ջ�������ʼ��
	
    PID_init(&motor_v,PID_POSITION,chassis_pid,12000,0);//pid��������ʼ��
	PID_init(&Trace_v[0],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[1],PID_POSITION,Trace_pid,4000,0);    
	PID_init(&Trace_v[2],PID_POSITION,Trace_pid,4000,0);
	PID_init(&Trace_v[3],PID_POSITION,Trace_pid,4000,0);
//********************************	*******************************	*******************************	
	
	
	while(1)
	{
	Chassis_SetMode(&chassis);//���õ����˶�ģʽ
    Chassis_information_update(&chassis);//���ݸ���

	if(chassis.Mode==STOP)
	{
		Can_send_data(0,0,0,0);
	}

	else
	{
		
		if(emergency_brake_flag==0)//�����ƶ���־λΪ0
		{
			Speed_Caculation(&chassis);//ȫ�����˶�ѧ���
			
			if(chassis.Mode==REMOTE_CONTROL || chassis.Mode==TEACH || chassis.Mode==GYROSCOPE)//�����־�Ϊң��������
			{
				for(uint8_t i=0;i<4;i++)
				{
				chassis.motor_out[i]=(int16_t)PID_calc(&motor_v,motor_chassis[i].speed_rpm,chassis.motor_temp[i]);//����ÿ����������ֵ
				}
			}
			else if(chassis.Mode==SELF_MOTION)	//�����Զ�ģʽ����PID�����뿪��ʱ��ͬ������Ҫ�����ֳ���
			{
				for(uint8_t c=0;c<4;c++)
				{
				chassis.motor_out[c]=(int16_t)PID_calc(&Trace_v[c],motor_chassis[c].speed_rpm,chassis.motor_temp[c]);
				}

			}

			Can_send_data(chassis.motor_out[0],chassis.motor_out[1],chassis.motor_out[2],chassis.motor_out[3]);
		}
		else//���������ƶ�
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
	Chassis->motor_temp[3]=Chassis->Vx_set-Chassis->Vy_set+Chassis_angle.yaw*40;//�Զ�����ʱǿ����������
		
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

		speed_x=flag_data.all_point[flag_index][0]-Action_now.x;//�Զ�����ʱ������ֵ����һ��������꣬����ֵ�ǵ�ǰλ��
		speed_y=flag_data.all_point[flag_index][1]-Action_now.y;	
    	Chassis->Vx_set=speed_x*10;
		Chassis->Vy_set=speed_y*10;
	
	}	
	
	else if(Chassis->Mode==GYROSCOPE)
	{
	    Chassis->Vx_set=RC->rc.ch[0]*3;
		Chassis->Vy_set=RC->rc.ch[1]*3;
		Chassis->Wz_set=RC->rc.ch[2]*3;
		
		Chassis->Vy_set=Chassis->Vy_set*cos(Chassis_angle.yaw_rad)-Chassis->Vx_set*sin(Chassis_angle.yaw_rad);//С���ݣ�������ȫ��
		Chassis->Vx_set=Chassis->Vy_set*sin(Chassis_angle.yaw_rad)+Chassis->Vx_set*cos(Chassis_angle.yaw_rad);
	}

}

void Chassis_SetMode(chassis_t *Chassis)
{
	if(RC->rc.s[1]==0x03 && RC->rc.s[0]==0x03)//����ң��������������������ģʽ
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
	
	if(Chassis->Mode!=SELF_MOTION)//ʾ��ʱ����������������㣬�����߶�
	{
		if(RC->rc.ch[4]==0x294)//��
		{random_flag=1;}
		else
		{random_flag=0;}
		
		if(RC->rc.ch[4]==-0x294)//��
		{medium_flag=1;}
		 else
		{medium_flag=0;}		 
		
		if(RC->rc.ch[3]==0x294)//��
		{apex_flag=1;}
		else
		{apex_flag=0;}	
	}

}












