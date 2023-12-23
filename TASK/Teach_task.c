/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Դ�ļ�:ʾ������											 		  (   )                      /)
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



#include "Teach_task.h"
#include "Chassis_task.h"
#include "usart.h"
#include "cmsis_os.h"
#include "pid.h"
#include "bsp_flash.h"
#include "tim.h"


int flag_buffer[300][2];//��ǵ�
int flash_write_data[1502];
int flash_read_data[1502];
uint8_t special_flag_buffer[300][3];//�����

int16_t flag_num,speed_x,speed_y;
uint8_t teach_stop_flag;
uint8_t random_flag,medium_flag,apex_flag;
uint8_t current_position,expect_position;
int16_t flag_index,special_position_num;

extern uint8_t buzzer_flag;
extern chassis_t chassis;

flag_data_t flag_data;
 
extern uint8_t Action_init_finish;




void Teach_task(void const *pvParameters)
{


	
	while(!Action_init_finish)
	{vTaskDelay(10);}
	
	
	  while(1)		
	{
		
		if(chassis.Mode==TEACH )
		{	


			flag_num=0;//��ʼ����ǵ�����
			special_position_num=0;//��ʼ�������ǵ�����
			teach_stop_flag=0;//��ʼ��ֹͣʾ�̱�־
			current_position=0;//��ʼ����ǰλ��
			expect_position=0;//��ʼ������λ��
			flag_index=0;//��ʼ����ǵ�����
			
		    for(int16_t m=0;m<300;m++)//��ʼ����ǵ�����������ͶӰ
			{
				flag_buffer[m][0]=0;
				flag_buffer[m][1]=0;
				flag_data.special_flag_projection[m]=0;
			}

			for(uint8_t i=0;i<3;i++)//��ʼ�����������
			{
				for(int16_t d=0;d<300;d++)
				{				
				    special_flag_buffer[d][i]=0;		
				}
			}
			
            for(uint8_t i=0;i<30;i++)
			{
				flag_data.map_special_to_all[i]=0;
			}
			
			
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
			
			while(random_flag==0)//�����»�����ܼ���ִ�У�������������ȷ����ȫ
			{
			 vTaskDelay(10);
			}
			

			
			for(int16_t a=0; a<300; a++)
			{
				if(chassis.Mode==TEACH)
			   {
				flag_buffer[a][0]=Action_now.x;//��ʼʾ�̣���ң������ʼ�滮·����
				flag_buffer[a][1]=Action_now.y;//ÿ��200ms����һ�ε�ǰλ��
   
				 if(a>298)
				 {
				     teach_stop_flag=1;//��ǵ������������ֵʱ������ֹͣ��־�����ڵ��̿���������ִ�д˲���
				 }

				 
				 if(random_flag==1)//��������Ͱ�ĸ߶ȣ�������������ʱ�߶ȿ�������ִ�С�
				 {
					 special_flag_buffer[a][0]=1;
					 buzzer_flag=5;//�����������־,di-- ��ʾ������ɱ�ǣ�����ͬ��
				 }
				 else if(medium_flag==1)//��
				 {
					 special_flag_buffer[a][1]=1;
					 buzzer_flag=5;	 
				 }
				 else if(apex_flag==1)//��
				 {
					 special_flag_buffer[a][2]=1;
					 buzzer_flag=5;	 
				 }
     
				 
			     	osDelay(200);//�����ÿ��200ms��¼һ�ε�ǰλ�ã�����ʾ��ʱ�����Ϊ1���ӣ���ʵ�Ѿ��㹻����
				 
			   }
			   
			   else if(chassis.Mode==GET_DATA)//����ʾ������
			   {							

				   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
					flag_num=a;   
					a=300;  //����ѭ�� 
			   }
                 
	    	}
			
			while(chassis.Mode!=GET_DATA)
			{
			  vTaskDelay(10);//��ǵ���������ʱ����������������ֱ���ֶ�ֹͣʾ������
			}

			//��ʼ���ݴ���
			

			//���ֲ�������ʾ��ʱ���������ֵ�ʱ�������200ms���ڣ�
			//������Ҫ��������λ�õ�����ı��
			for(uint8_t s=0;s<3;s++)
			{
				for(short i=0;i<300;i++)
				{
					if(special_flag_buffer[i][s]==1)
					{
						for(uint8_t d=1;d<5;d++)
						{
							special_flag_buffer[i+d][s]=0;
						}
					}
				} 				 
		    }
			
			
			//ͳ������λ�õ�������������
			
		
			for(short z=0;z<300;z++)
			{	
				if(special_flag_buffer[z][0]==1) 
				{special_flag_buffer[z][0]=++special_position_num;}
				else if(special_flag_buffer[z][1]==1) 
				{special_flag_buffer[z][1]=++special_position_num;}
				else if(special_flag_buffer[z][2]==1) 
				{special_flag_buffer[z][2]=++special_position_num;}
			}
		   
            //Ϊ�˷���ؽ����ݴ���flash�У���Ҫ���������ݴ�ŵ�һ��������		
          	for(short i=0,p=0;i<300;i++,p+=2)
			{
				flash_write_data[p]=flag_buffer[i][0];
				flash_write_data[p+1]=flag_buffer[i][1];//��ű�ǵ�
            }
			for(short q=600,p=0;p<300;p++,q+=3)
			{
				flash_write_data[q]=special_flag_buffer[p][0];
				flash_write_data[q+1]=special_flag_buffer[p][1];
				flash_write_data[q+2]=special_flag_buffer[p][2];	//�������λ�õ�
		
			}
			flash_write_data[1500]=flag_num;//������б�ǵ�����
			flash_write_data[1501]=special_position_num;//�������λ�õ�����
            
			flash_erase_address(ADDR_FLASH_SECTOR_11,1);//�������11����������	   
			flash_write_single_address(ADDR_FLASH_SECTOR_11,(uint32_t *)flash_write_data,1502);//��flash��д������
    	   
		}
		
		else if(chassis.Mode==GET_DATA)
		{
			//��flash�ж�ȡ���� 
			flash_read(ADDR_FLASH_SECTOR_11,(uint32_t *)flash_read_data,1502);
			
			
            //���ݷ��ಢ��ȡ
			flag_data.all_point_num=flash_read_data[1500];
			flag_data.special_point_num=flash_read_data[1501];

			 for(short i=0,p=0;i<300;i++,p+=2)
			{     
				flag_data.all_point[i][0]=flash_read_data[p];
				flag_data.all_point[i][1]=flash_read_data[p+1];
			}
			
			for(short i=0,p=600;i<300;i++,p+=3)
			{
				flag_data.special_point[i][0]=flash_read_data[p];
				flag_data.special_point[i][1]=flash_read_data[p+1];
				flag_data.special_point[i][2]=flash_read_data[p+2];	
			}
			//�������λ�ñ�ŷ���ͬһ�������У�ͶӰ��
			
			for(uint8_t s=0;s<3;s++)
			{
				for(short i=0;i<300;i++)
				{
					if(flag_data.special_point[i][s]!=0)
					{ flag_data.special_flag_projection[i]=flag_data.special_point[i][s];}
					
				}
	    	}
			
			//�������ӳ�䵽���б�ǵ㣬��Ӧ���������
			for(short i=0,q=1;i<300;i++)
			{			
				if(flag_data.special_flag_projection[i]!=0)
				{		
                    					
					flag_data.map_special_to_all[q++]=i;
				}
				
				
			}
			
			buzzer_flag=3;//��������ʾ�����ݶ�ȡ���
			
			
			while(chassis.Mode!=SELF_MOTION)
			{
			vTaskDelay(20);
			}
			
		}
		
		else if(chassis.Mode==SELF_MOTION)
		{

             buzzer_flag=3;//������������ģʽ
			

			
			for(uint8_t i=1;i<=flag_data.special_point_num;i++)
			{	
				if(abs_int(flag_data.all_point[flag_data.map_special_to_all[i]][0]-Action_now.x)<100 && abs_int(flag_data.all_point[flag_data.map_special_to_all[i]][1]-Action_now.y)<100)
				{
					current_position=i;
					
				}
			}
			
			expect_position=current_position;
			
			
			
			while(1)
			{
			
				if(current_position<expect_position)
				{
					HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);//�ƶ������У�������
					for(flag_index=flag_data.map_special_to_all[current_position];flag_index<flag_data.map_special_to_all[expect_position];)
					{
					   if((abs_int(flag_data.all_point[flag_index][0]-Action_now.x)<70) && (abs_int(flag_data.all_point[flag_index][1]-Action_now.y)<70))
					   {
						   flag_index++;
					   }
					   osDelay(20);
					}
				    current_position=expect_position;
				}
				
			    else if(current_position>expect_position)
			   {
				   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
					for(flag_index=flag_data.map_special_to_all[current_position];flag_index>flag_data.map_special_to_all[expect_position];)
					{
					   if((abs_int(flag_data.all_point[flag_index][0]-Action_now.x)<70) && (abs_int(flag_data.all_point[flag_index][1]-Action_now.y)<70))
					   {
						   flag_index--;
					   }
					   osDelay(20);
					}
				    current_position=expect_position;

			   }
			   else if(current_position==expect_position)
			  {
			   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);//��ֹʱ��������
				  
				  flag_index=flag_data.map_special_to_all[current_position];				  
				  
				  
			   //uart_command(0x14);  //����չ��������
			  } 

			   if(chassis.Mode==TEACH||chassis.Mode==REMOTE_CONTROL||chassis.Mode==STOP)
			   {
				   
				   //����չ
				   
				   
			      break;
			   }
			
			   
			   
			   
				
	           vTaskDelay(2);
			}
			
			

		}

			
	     	

		
	 	
		vTaskDelay(2);
		
	
		
	

	}

}




int abs_int(int random)
{

	int x;
	x=(random>0) ? random:-random;
	return x;

}



