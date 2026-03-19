#ifndef __GPIO_H
#define __GPIO_H

#define RTHW PBin(11)

#define BEEP PAout(15)

void KEY_GPIO_Init(void); // 引脚初始化
void BEEP_Init(void);     // 引脚初始化
void BY8001_Init(void);
#endif
