/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	源文件:示教任务											 		  (   )                      /)
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



#include "Teach_task.h"
#include "Chassis_task.h"
#include "usart.h"
#include "cmsis_os.h"
#include "pid.h"
#include "bsp_flash.h"
#include "tim.h"


int flag_buffer[300][2];//标记点
int flash_write_data[1502];
int flash_read_data[1502];
uint8_t special_flag_buffer[300][3];//特殊点

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


			flag_num=0;//初始化标记点数量
			special_position_num=0;//初始化特殊标记点数量
			teach_stop_flag=0;//初始化停止示教标志
			current_position=0;//初始化当前位置
			expect_position=0;//初始化期望位置
			flag_index=0;//初始化标记点索引
			
		    for(int16_t m=0;m<300;m++)//初始化标记点数组和特殊点投影
			{
				flag_buffer[m][0]=0;
				flag_buffer[m][1]=0;
				flag_data.special_flag_projection[m]=0;
			}

			for(uint8_t i=0;i<3;i++)//初始化特殊点数组
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
			
			while(random_flag==0)//滚轮下滑后才能继续执行，两个触发条件确保安全
			{
			 vTaskDelay(10);
			}
			

			
			for(int16_t a=0; a<300; a++)
			{
				if(chassis.Mode==TEACH)
			   {
				flag_buffer[a][0]=Action_now.x;//开始示教，用遥控器开始规划路径，
				flag_buffer[a][1]=Action_now.y;//每隔200ms保存一次当前位置
   
				 if(a>298)
				 {
				     teach_stop_flag=1;//标记点数量超过最大值时，触发停止标志，会在底盘控制任务中执行此操作
				 }

				 
				 if(random_flag==1)//设置垃圾桶的高度，共有三档，此时高度控制任务不执行。
				 {
					 special_flag_buffer[a][0]=1;
					 buzzer_flag=5;//蜂鸣器任务标志,di-- 提示你已完成标记，下面同理
				 }
				 else if(medium_flag==1)//中
				 {
					 special_flag_buffer[a][1]=1;
					 buzzer_flag=5;	 
				 }
				 else if(apex_flag==1)//高
				 {
					 special_flag_buffer[a][2]=1;
					 buzzer_flag=5;	 
				 }
     
				 
			     	osDelay(200);//程序会每隔200ms记录一次当前位置，所以示教时间最多为1分钟（其实已经足够长了
				 
			   }
			   
			   else if(chassis.Mode==GET_DATA)//结束示教任务
			   {							

				   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
					flag_num=a;   
					a=300;  //跳出循环 
			   }
                 
	    	}
			
			while(chassis.Mode!=GET_DATA)
			{
			  vTaskDelay(10);//标记点数量超出时，任务会在这里挂起，直至手动停止示教任务
			}

			//开始数据处理
			

			//人手不可能在示教时将拨动滚轮的时间控制在200ms以内，
			//所以需要过滤特殊位置点后多余的标记
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
			
			
			//统计特殊位置点数量并对其编号
			
		
			for(short z=0;z<300;z++)
			{	
				if(special_flag_buffer[z][0]==1) 
				{special_flag_buffer[z][0]=++special_position_num;}
				else if(special_flag_buffer[z][1]==1) 
				{special_flag_buffer[z][1]=++special_position_num;}
				else if(special_flag_buffer[z][2]==1) 
				{special_flag_buffer[z][2]=++special_position_num;}
			}
		   
            //为了方便地将数据存入flash中，需要将所有数据存放到一个数组中		
          	for(short i=0,p=0;i<300;i++,p+=2)
			{
				flash_write_data[p]=flag_buffer[i][0];
				flash_write_data[p+1]=flag_buffer[i][1];//存放标记点
            }
			for(short q=600,p=0;p<300;p++,q+=3)
			{
				flash_write_data[q]=special_flag_buffer[p][0];
				flash_write_data[q+1]=special_flag_buffer[p][1];
				flash_write_data[q+2]=special_flag_buffer[p][2];	//存放特殊位置点
		
			}
			flash_write_data[1500]=flag_num;//存放所有标记点数量
			flash_write_data[1501]=special_position_num;//存放特殊位置点数量
            
			flash_erase_address(ADDR_FLASH_SECTOR_11,1);//清除扇区11的所有数据	   
			flash_write_single_address(ADDR_FLASH_SECTOR_11,(uint32_t *)flash_write_data,1502);//往flash中写入数据
    	   
		}
		
		else if(chassis.Mode==GET_DATA)
		{
			//从flash中读取数据 
			flash_read(ADDR_FLASH_SECTOR_11,(uint32_t *)flash_read_data,1502);
			
			
            //数据分类并提取
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
			//将特殊点位置编号放在同一个数组中（投影）
			
			for(uint8_t s=0;s<3;s++)
			{
				for(short i=0;i<300;i++)
				{
					if(flag_data.special_point[i][s]!=0)
					{ flag_data.special_flag_projection[i]=flag_data.special_point[i][s];}
					
				}
	    	}
			
			//将特殊点映射到所有标记点，对应具体的坐标
			for(short i=0,q=1;i<300;i++)
			{			
				if(flag_data.special_flag_projection[i]!=0)
				{		
                    					
					flag_data.map_special_to_all[q++]=i;
				}
				
				
			}
			
			buzzer_flag=3;//蜂鸣器提示：数据读取完毕
			
			
			while(chassis.Mode!=SELF_MOTION)
			{
			vTaskDelay(20);
			}
			
		}
		
		else if(chassis.Mode==SELF_MOTION)
		{

             buzzer_flag=3;//进入语音控制模式
			

			
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
					HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);//移动过程中，灯条亮
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
			   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);//静止时，灯条灭
				  
				  flag_index=flag_data.map_special_to_all[current_position];				  
				  
				  
			   //uart_command(0x14);  //可扩展语音助手
			  } 

			   if(chassis.Mode==TEACH||chassis.Mode==REMOTE_CONTROL||chassis.Mode==STOP)
			   {
				   
				   //可扩展
				   
				   
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



