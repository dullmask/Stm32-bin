#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include "Delay.h"
#include "OLED.h"
#include "stmflash.h"
#include "gpio.h"
#include "Timer.h"
#include "HC_SR04.h"
#include "MOTOR.h"
#include "esp8266.h"

#define FLASH_SAVE_ADDR  ((u32)0x0800F000) 				//设置FLASH 保存地址(必须为偶数)

char display[16];
u8 setn = 0;//设置标识
u8 keyNum = 99;//按键标识
u8 count;
u8 mode = 1;//模式标识
u8 start=0;//计时开始标识
u8 BEEP_flag=0;//蜂鸣器控制标记

u16 Distance;
u16 DMAX=50;
u8 opcl=10;//垃圾桶打开关闭状态WiFi数据发送
u8 full=10;//垃圾桶是否装满WiFi数据发送

extern unsigned char MotorLed;
/*******************************函数声明*********************************/
void Init(void);
void DisplayData(void);
void DisplaySetValue(void);//显示设置页面
void ControlFun(void);

void Store_Data(void);//存储数据
void Read_Data(void);//读出数据void Store_Data(void) //存储数据
void CheckNewMcu(void);// 检查是否是新的单片机，是的话清空存储区，否则保留

void KeyScan(void);//获取当前按键值
void KeyScanHandle(void);//按键扫描
void DisplaySetValue(void);//显示设置页面
/************************************************************************/

int main(void)
{
	Init();
	
	while (1)
	{
			DisplayData();
			ControlFun();
	}
}

/************************************************************************/
void Init(void)
{
	u8 i;
	
	OLED_Init();
	OLED_CLS();
	CheckNewMcu();
	KEY_GPIO_Init();
	BEEP_Init();
	
	for(i=0;i<4;i++)OLED_ShowCN(i*16+32,3,i+24,0);//显示中文：欢迎使用
	Delay_ms(2000);
	
	TIM2_Init(7199,0);    //以10KHz计数,定时100us：1-pwm占空比调节;2-计算超声波距离
	TIM4_Init(999,719);//定时器初始化，定时10ms,检测按键按下
	TIM3_Init(9999,7199);//定时器初始化，定时1S
	HC_SR04_IO_Init();  //超声波传感器初始化
	MOTOR_GPIO_Init();  //电机初始化
	ESP8266_Init();  //esp8266WiFi模块初始化
	
	OLED_CLS();
	TIM_Cmd(TIM3,ENABLE);  //使能TIMx
}

void Store_Data(void) //存储数据
{
	u16 DATA_BUF[3];

//	DATA_BUF[0] = stepNum;

	STMFLASH_Write(FLASH_SAVE_ADDR + 0x20,(u16*)DATA_BUF,6); //写入数据

	Delay_ms(50);
}

void Read_Data(void) //读出数据
{
	u16 DATA_BUF[3];

	STMFLASH_Read(FLASH_SAVE_ADDR + 0x20,(u16*)DATA_BUF,6); //读取数据

//	stepNum = DATA_BUF[0];  
}

void CheckNewMcu(void)  // 检查是否是新的单片机，是的话清空存储区，否则保留
{
	u8 comper_str[6];

	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)comper_str,5);
	comper_str[5] = '\0';
	if(strstr((char *)comper_str,"FDYDZ") == NULL)  //新的单片机
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)"FDYDZ",5); //写入“FDYDZ”，方便下次校验
		Delay_ms(50);
		Store_Data();//存储下初始数据
		Delay_ms(50);
	}
	Read_Data(); //开机读取下相关数据
}

void ControlFun(void)
{
	u8 i;
	
	// 计算超声波测出的距离
	Distance = (Get_SR04_Distance() * 331) / 1000; 
	Distance = Distance > 4500 ? 4500 : Distance;
	
	if (Distance < 100)
	{
		full=20;
		BEEP=1;
	}else
	{
		full=10;
		BEEP=0;
	}
	if(RTHW == 0)
	{
		opcl=20;
		for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+69,0);//显示中文：打开
		if(MotorLed==0)//如果是关闭的
		{
			MotorCW();//控制电机正转开
		}
	}else 
	{
		opcl=10;
		for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+71,0);//显示中文：关闭
		if(MotorLed==1) //如果是打开的
		{
			MotorCCW();//控制电机反转关
		}
	}
}

void TIM4_IRQHandler(void)
{ 
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			count++;
			KeyScan();
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //清除中断标志位
		}
}
