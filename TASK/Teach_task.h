/*****************************************************************************************************
*																		  ,~.
*	�ұ����ҼҸ�磬�ܱ��ӳ��򲻳�bug��ǧ��𶯣�С����������--------->  ,-'__ `-,
*																	  {,-'  `. }              ,')
*																	 ,( a )   `-.__         ,',')~,
*																	<=.) (         `-.__,==' ' ' '}
*	ͷ�ļ�:ʾ������											 		  (   )                      /)
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






typedef struct
{

  short all_point_num;//��ǵ������
  short special_point_num;//����������
  int all_point[300][2];//��ǵ������
  int special_point[300][3];//����������
  short map_special_to_all[30];//�����ӳ�䵽��ǵ�����
  uint8_t special_flag_projection[300];//������ͶӰ
  	
}flag_data_t;//�˽ṹ��װ�����Զ�����ʱ��Ҫ�����б���




void Teach_task(void const *pvParameters);
int abs_int(int random);

extern int16_t flag_num,speed_x,speed_y;
extern int flag_buffer[300][2];
extern uint8_t teach_stop_flag,random_flag,medium_flag,apex_flag;
extern int16_t flag_index;
extern uint8_t current_position,expect_position;
extern flag_data_t flag_data;


