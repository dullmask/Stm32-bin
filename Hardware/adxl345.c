/***********************
加速度传感器ADXL345数据读取程序
IIC总线接口
SDA：PA5
SCL：PA6
************************/

#include "adxl345.h"
#include "Delay.h"

//IIC总线接口配置
void I2C_SDA_OUT(void)//SDA输出方向配置
{
    GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//SDA推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure); 						

}

void I2C_SDA_IN(void)//SDA输入方向配置
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//SCL上拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}
//以下为模拟IIC总线函数
void IIC_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PD端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_5;	//PD6配置为推挽输出,SCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD
	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_5); 
//	I2C_SCL_H;
//	I2C_SDA_H;//均拉高
}
void IIC_start()
{
	I2C_SDA_OUT();
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	Delay_us(5);
	IIC_SDA=0;
	Delay_us(5);
	IIC_SCL=0;
}
void IIC_stop()
{
	I2C_SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;
	Delay_us(5);
	IIC_SCL=1; 
	IIC_SDA=1;
	Delay_us(5);
}
//主机产生一个应答信号
void IIC_ack()
{
	IIC_SCL=0;
	I2C_SDA_OUT();
  IIC_SDA=0;
   Delay_us(2);
   IIC_SCL=1;
   Delay_us(5);
   IIC_SCL=0;	
}
//主机不产生应答信号
void IIC_noack()
{
	IIC_SCL=0;
	I2C_SDA_OUT();
   IIC_SDA=1;
   Delay_us(2);
   IIC_SCL=1;
   Delay_us(2);
   IIC_SCL=0;
}
//等待从机应答信号
//返回值：1 接收应答失败
//		  0 接收应答成功
u8 IIC_wait_ack()
{
	u8 tempTime=0;
	I2C_SDA_IN();
	IIC_SDA=1;
	Delay_us(1);
	IIC_SCL=1;
	Delay_us(1);

	while(READ_SDA)
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_stop();
			return 1;
		}	 
	}

	IIC_SCL=0;
	return 0;
}
void IIC_send_byte(u8 txd)
{
	u8 i=0;
	I2C_SDA_OUT();
	IIC_SCL=0;;//拉低时钟开始数据传输
	for(i=0;i<8;i++)
	{
		IIC_SDA=(txd&0x80)>>7;//读取字节
		txd<<=1;
		IIC_SCL=1;
		Delay_us(2); //发送数据
		IIC_SCL=0;
		Delay_us(2);
	}
}
//读取一个字节
u8 IIC_read_byte(u8 ack)
{
	u8 i=0,receive=0;
	I2C_SDA_IN();
   for(i=0;i<8;i++)
   {
   		IIC_SCL=0;
		Delay_us(2);
		IIC_SCL=1;
		receive<<=1;//左移
		if(READ_SDA)
		   receive++;//连续读取八位
		Delay_us(1);	
   }

   	if(!ack)
	   	IIC_noack();
	else
		IIC_ack();

	return receive;//返回读取到的字节
}


//向指定寄存器地址写入指定数据
void Write_ADXL345(u8 REG_Address,u8 REG_data)
{
    IIC_start();                  //起始信号
    IIC_send_byte(SlaveAddress);   //发送设备地址+写信号 
	  IIC_wait_ack();	  
    IIC_send_byte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
	  IIC_wait_ack();	
    IIC_send_byte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
	  IIC_wait_ack();	
    IIC_stop();                   //发送停止信号
}

//向指定寄存器地址读出一个字节数据
unsigned char Read_ADXL345_Byte(u8 REG_Address)
{  
	  u8 REG_data;
	
    IIC_start();                          //起始信号
    IIC_send_byte(SlaveAddress);           //发送设备地址+写信号
	  IIC_wait_ack();	
    IIC_send_byte(REG_Address);            //发送存储单元地址，从0开始	
	  IIC_wait_ack();	
    IIC_start();                          //起始信号
    IIC_send_byte(SlaveAddress+1);         //发送设备地址+读信号
	  IIC_wait_ack();	  
    REG_data=IIC_read_byte(0);              //读出寄存器数据  
	  IIC_stop();                           //停止信号
    return REG_data; 
}

//读取ADXL345 x,y,z轴数据函数
void ADXLl345_Read_Data(short *x,short *y,short *z)
{
	u8 BUF[6];
	u8 i;
	
	IIC_start();  				 
	IIC_send_byte(SlaveAddress);	//发送写器件指令	 
	IIC_wait_ack();	   
  IIC_send_byte(0x32);   		//发送寄存器地址(数据缓存的起始地址为0X32)
	IIC_wait_ack(); 	 										  		   
 
 	IIC_start();  	 	   		//重新启动
	IIC_send_byte(SlaveAddress+1);	//发送读器件指令
	IIC_wait_ack();
	for(i=0;i<6;i++)
	{
		if(i==5)BUF[i]=IIC_read_byte(0);//读取一个字节,不继续再读,发送NACK  
		else    BUF[i]=IIC_read_byte(1);	//读取一个字节,继续读,发送ACK 
 	}	        	   
  IIC_stop();					//产生一个停止条件
	*x=(short)(((u16)BUF[1]<<8)+BUF[0]); 	//合成数据    
	*y=(short)(((u16)BUF[3]<<8)+BUF[2]); 	    
	*z=(short)(((u16)BUF[5]<<8)+BUF[4]); 
}

//连读读取几次取平均值函数
//times 取平均值的次数
void ADXLl345_Read_Average(float *x,float *y,float *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//读取次数不为0
	{
		for(i=0;i<times;i++)//连续读取times次
		{
			ADXLl345_Read_Data(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			Delay_ms(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345(void)
{
	 IIC_init();
	 Delay_ms(100);
   Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Write_ADXL345(0x2C,0x08);   //速率设定为12.5 参考pdf13页
   Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}


