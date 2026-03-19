#include "stm32f10x.h"                  // Device header
#include "gpio.h"

void KEY_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能PABC端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                                           // 关闭JTAG模式,将A15,B3,B4这些引脚配置成普通IO口

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                                        // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    // IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PA端口时钟

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//GPIO复用功能时钟使能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //将A15,B3,B4这些引脚配置成普通IO口

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //继电器输出 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15); 						
}

