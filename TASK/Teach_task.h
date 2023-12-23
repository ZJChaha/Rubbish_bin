/*****************************************************************************************************
*																		  ,~.
*	右边是我家哥哥，能保佑程序不出bug，千万别动，小心它来啄你--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	头文件:示教任务											 		  (   )                      /)
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






typedef struct
{

  short all_point_num;//标记点的数量
  short special_point_num;//特殊点的数量
  int all_point[300][2];//标记点的坐标
  int special_point[300][3];//特殊点的坐标
  short map_special_to_all[30];//特殊点映射到标记点的序号
  uint8_t special_flag_projection[300];//特殊点的投影
  	
}flag_data_t;//此结构体装的是自动运行时需要的所有变量




void Teach_task(void const *pvParameters);
int abs_int(int random);

extern int16_t flag_num,speed_x,speed_y;
extern int flag_buffer[300][2];
extern uint8_t teach_stop_flag,random_flag,medium_flag,apex_flag;
extern int16_t flag_index;
extern uint8_t current_position,expect_position;
extern flag_data_t flag_data;


