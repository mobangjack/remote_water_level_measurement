//发射设备

#include"tx.h"

void tx()
{
	unsigned int s=0;
	unsigned char TxBuf[TX_PLOAD_WIDTH];//发送缓存

	init_NRF24L01();		//nrf24l01初始化
	LCD_Init();    //初始化LCD

	while(1)
	{
		s = (unsigned int)measureDistance();

		TxBuf[0] = s/100%10+'0';
		TxBuf[1] = s/10%10+'0';
		TxBuf[2] = s%10+'0';

		LCD_Write_String(0,0,TxBuf,TX_PLOAD_WIDTH); //显示测量结果

		nrf_RxTx('t',TxBuf);//发送数据

		ms(250);//延时决定采样速率
		ms(250);//延时决定采样速率

	}
}