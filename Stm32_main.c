#include "delay.h"
 
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 
 
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 输出高
}
//产生IIC起始信号		SCL在高电平模式时，SDA由高变低
void IIC_Start(void)
{
	SDA_OUT();    	//sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;		//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;		//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号		SCL在高电平时，SDA由低变高
void IIC_Stop(void)
{
	SDA_OUT();		//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;		//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;		//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来，SCL在高电平时，SDA为低电平则接受应答成功
//返回值:	1，接收应答失败
//       	0，接收应答成功
u8 IIC_Wait_Ack(void)	//发送一个数据之后，从机必须产生应答，如果没应答，代表数据传输失败
{
	u8 ucErrTime=0;
	SDA_IN();     		//SDA设置为输入  
	IIC_SDA=1;			//放掉SDA控制权，使从机能控制SDA
	delay_us(1);	   
	IIC_SCL=1;			//SCL在高电平时，SDA为低电平则接受应答成功
	delay_us(1);	 
	while(READ_SDA)		//如果返回1，则接受失败
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();		//发送停止信号
			return 1;
		}
	}
	IIC_SCL=0;			//时钟输出0 	   
	return 0;  
} 
//产生ACK应答			在SCL时钟由低变高之前将SDA拉低，并保持至SCL由高变低时
void IIC_Ack(void)		
{
	IIC_SCL=0;		
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//不产生ACK应答		   在SCL时钟由低变高之前将SDA拉高，并保持至SCL由高变低时    
void IIC_NAck(void)		
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}		

//IIC发送一个字节 	从高位开始传输
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT();		//SDA设为输出模式 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























