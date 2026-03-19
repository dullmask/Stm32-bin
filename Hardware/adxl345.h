#ifndef __ADXL345_H
#define __ADXL345_H
#include "sys.h"

#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改

#define IIC_SCL    PAout(6) //SCL
#define IIC_SDA    PAout(5) //SDA	 
#define READ_SDA   PAin(5)  //输入SDA 

void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void IIC_init(void);
void IIC_start(void);
void IIC_stop(void);
void IIC_ack(void);
void IIC_noack(void);
u8 IIC_wait_ack(void);
void IIC_send_byte(u8 txd);
u8 IIC_read_byte(u8 ack);

void Init_ADXL345(void);
unsigned char Read_ADXL345_Byte(u8 REG_Address);
void ADXLl345_Read_Data(short *x,short *y,short *z);
void ADXLl345_Read_Average(float *x,float *y,float *z,u8 times);

#endif
