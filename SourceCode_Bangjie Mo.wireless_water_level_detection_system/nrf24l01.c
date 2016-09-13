#include<reg52.h>
#include"delay.h"
#include"nrf24l01.h"

/*NRF24L01 IO端口定义*/
sbit CE=P2^3;
sbit CSN=P2^7;
sbit SCK=P2^2;
sbit MOSI=P2^0;
sbit MISO=P2^1;
//sbit IRQ=P1^7;

//*****************************NRF24L01的接收和发送地址***********************************//
unsigned char const TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//本地地址
unsigned char const RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//接收地址	
//****************************************状态标志****************************************//
unsigned char bdata sta;
sbit	RX_DR=sta^6;
sbit	TX_DS=sta^5;
sbit	MAX_RT=sta^4;
/****************************************************************************************************
/*NRF24L01的SPI时序
/****************************************************************************************************/
unsigned char SPI_RW(unsigned char num)
{
	unsigned char bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  // output 8-bit
   	{
		MOSI = (num & 0x80);            // output 'unsigned char', MSB to MOSI
		num <<= 1;                      // shift next bit into MSB..
		SCK = 1;                        // Set SCK high..
		num |= MISO;       		        // capture current MISO bit
		SCK = 0;            		    // ..then set SCK low again
   	}
    return(num);           		        // return read unsigned char
}
/****************************************************************************************************
/*NRF24L01的读寄存器函数
****************************************************************************************************/
unsigned char SPI_Read_Reg(unsigned char reg)
{
	unsigned char reg_val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
}
/****************************************************************************************************/
/*NRF24L01写寄存器函数
/****************************************************************************************************/
unsigned char SPI_Write_Reg(unsigned char reg, unsigned char value)
{
	unsigned char status;
	CSN = 0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	CSN = 1;                   // CSN high again
	return(status);            // return nRF24L01 status unsigned char
}
/****************************************************************************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;
	CSN = 0; // Set CSN low, init SPI tranaction
	status = SPI_RW(reg); // Select register to write to and read status byte
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
	pBuf[byte_ctr] = SPI_RW(0); // Perform SPI_RW to read byte from nRF24L01
	CSN = 1; // Set CSN high again
	return(status); // return nRF24L01 status byte
}
/*********************************************************************************************************
/*写数据：为寄存器地址，pBuf：为待写入数据地址，unsigned chars：写入数据的个数
/*********************************************************************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char chars)
{
	unsigned char status,char_ctr;
	
	CSN = 0;            //SPI使能       
	status = SPI_RW(reg);   
	for(char_ctr=0; char_ctr<chars; char_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //关闭SPI
	return(status);    // 
}
//********************************NRF24L01初始化******************************************//
void init_NRF24L01()
{
 	CE=0;    // 片选使能
 	CSN=1;   // SPI disable
 	SCK=0;   // SPI时钟拉低
	//IRQ=1;
}
//检测应答信号
bit checkAck()
{
	sta = SPI_Read_Reg(STATUS);//读取状态寄存器的值
	if(TX_DS||MAX_RT){
		SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
		SPI_Write_Reg(FLUSH_TX,0x00); //清除tx fifo寄存器
		return(1);
	}
	return(0);
}
//********** nrf收发程序,返回状态标识 **********
bit nrf_RxTx(unsigned char mod_nrf,unsigned char *buf) //NRF24L01收发程序	
{	
	//******进入发射模式******
	if(mod_nrf == 't')
	{
		CE = 0;
		SPI_Write_Buf(WRITE_REG + TX_ADDR, RX_ADDRESS, RX_ADR_WIDTH);     // 装载接收端地址
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); //写通道0接收地址    
		SPI_Write_Buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH); 			// 装载数据

		//寄存器配置
		SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                             //通道0自动应答 	
		SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                         //允许接收地址频道0 
		SPI_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a); 						// 自动重发延时等待250us+86us，自动重发10次
		SPI_Write_Reg(WRITE_REG + RF_CH, 0x40);                             //设置信道工作频率，收发必须一致
		SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);   		                 //设置发射速率为1MHZ，发射功率为最大值0dB	
		SPI_Write_Reg(WRITE_REG + CONFIG, 0x0e);							//CRC使能，16位CRC校验，上电，接收模式     
		CE = 1;
		us(10); //延时10us以上，激发数据发送

		while(1){
			if(checkAck()){
				return(1);
			}
		}
		return(0);
	} 
	//******进入接收模式******
	else if(mod_nrf == 'r')//接收模式
	{
		CE = 0;
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);    //写本地地址
	
		//寄存器配置
		SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                             //通道0自动应答 	
		SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                         //允许接收地址频道0 
		SPI_Write_Reg(WRITE_REG + RF_CH, 0x40);                             //设置信道工作频率，收发必须一致
		SPI_Write_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);                //设置接收数据长度
		SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);   		                 //设置发射速率为1MHZ，发射功率为最大值0dB	
		SPI_Write_Reg(WRITE_REG + CONFIG, 0x0f);							//CRC使能，16位CRC校验，上电，接收模式  
		CE = 1;
		us(130);//延时130us以上，准备接收

		sta = SPI_Read_Reg(STATUS); 

		if(RX_DR){
			CE = 0;//待机
			SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
			SPI_Read_Buf(RD_RX_PLOAD,buf,RX_PLOAD_WIDTH);//读取数据
			SPI_Write_Reg(FLUSH_RX,0x00);//清除rx fifo寄存器
			return(1);
		}
		return(0);

	 }	

}
