/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	Դ�ļ�:��������ʾ����											  (   )                      /)
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
#include "Buzzer_task.h"
#include "tim.h"
#include "cmsis_os.h"


extern TIM_HandleTypeDef htim4;

uint8_t buzzer_flag;


void Buzzer_task(void const *pvParameters)
{
	
    vTaskDelay(500);
	
	while(1)

	{
		switch (buzzer_flag)
		{
			case 0:
				buzzer_off();//ֹͣ
				break;
			case 1:
				Funky_town();//��ת���飬̫���ˣ�����һ����͵͵�棬����û�õ�
				break;
			case 2:
				trace();//������di--- di--- di---  ��һֱ�죬����buzzer_flag�ı�
				break;
			case 3:
				warning();//���ٵ�di-di-di-,  ֻ������
				break;
			case 4:
				Windows_XP();//WindowsXP�������֡����ѻ�԰��������������
			    break;
			case 5:
				di();//����˼�壬��ֻ��һ�� di--
			    break;

		}
        vTaskDelay(2);
        
	}

}



void buzzer_on(uint16_t arr)
{
  
   
    __HAL_TIM_SetAutoreload(&htim4,arr);
	 __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, arr/2);

}
void buzzer_off(void)
{
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 0);
}
int caculate_arr(int set)
{
    int out;
	out=21000000/set;
return out;
}	

void di()
{

   buzzer_on(caculate_arr(988));
   HAL_Delay(500);
   buzzer_flag=0;
}

	
	


void Windows_XP(void)
{

	buzzer_on(caculate_arr(659));
	HAL_Delay(375);
 
	buzzer_on(caculate_arr(880));  
	HAL_Delay(450);
  
	buzzer_on(caculate_arr(988));  
	HAL_Delay(500);
 
	buzzer_on(caculate_arr(1115));  
	HAL_Delay(650);	  

     buzzer_off();
	HAL_Delay(230);
	
	buzzer_on(caculate_arr(659));  
	HAL_Delay(400); 
	  
	buzzer_on(caculate_arr(988));  
	HAL_Delay(400); 
	
	buzzer_on(caculate_arr(1109));  
	HAL_Delay(430); 
	  
	buzzer_on(caculate_arr(1318));  
	HAL_Delay(450);	  
	  
	buzzer_on(caculate_arr(1480));  
	HAL_Delay(680); 
	  
    buzzer_off();
	HAL_Delay(10);
	
	buzzer_on(caculate_arr(988));   
	HAL_Delay(300);	 
	
    buzzer_off();
	HAL_Delay(10);
	
	buzzer_on(caculate_arr(831));  
	HAL_Delay(300);	
	
    buzzer_off();

    buzzer_flag=0;
}


void part1(void)
{if(buzzer_flag==1)
	{buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(30); 	  
	buzzer_on(caculate_arr(1046)); 
	HAL_Delay(250);  
	buzzer_on(caculate_arr(932));
	HAL_Delay(250);
	buzzer_on(caculate_arr(1046));
	HAL_Delay(350); 
    buzzer_off();
	HAL_Delay(70);      	  
	buzzer_on(caculate_arr(784));
	HAL_Delay(320);
    buzzer_off();
	HAL_Delay(200);  
	buzzer_on(caculate_arr(784));
	HAL_Delay(250);
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);	  
	buzzer_on(caculate_arr(1397));
	HAL_Delay(250);	  
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);		  
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);		
    buzzer_off();
	HAL_Delay(1000); 
}
	else
		buzzer_off();

}
void part2(void)
{if(buzzer_flag==1)
	{buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1568));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1568));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1568));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(1318));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(1175));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_off();
	HAL_Delay(1000);
}
	else
		buzzer_off();
}
void part3(void)
{
if(buzzer_flag==1)
{buzzer_on(caculate_arr(1175));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(1175));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(1175));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(1175));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);		
	buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
		buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
		buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
		buzzer_on(caculate_arr(1046));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(988));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(988));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(988));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);	
	buzzer_on(caculate_arr(988));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);		
	buzzer_on(caculate_arr(880));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(880));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(880));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
	buzzer_on(caculate_arr(784));
	HAL_Delay(250);
    buzzer_off();
	HAL_Delay(10);
}
else 
	buzzer_off();

}

void Funky_town(void)
{	
part1();
part1();
part2();
part1();
part2();	
part1();	
part3();
HAL_Delay(1000);
}

void trace()
{

  buzzer_on(caculate_arr(988));
  HAL_Delay(700);
  buzzer_off();
  HAL_Delay(600);

}

void warning()
{
buzzer_on(caculate_arr(1109));
HAL_Delay(150);
 buzzer_off();
HAL_Delay(30);	
buzzer_on(caculate_arr(1109));
HAL_Delay(150);
 buzzer_off();
HAL_Delay(30);
buzzer_on(caculate_arr(1109));
HAL_Delay(150);
 buzzer_off();
HAL_Delay(30);	
buzzer_flag=0;
}


