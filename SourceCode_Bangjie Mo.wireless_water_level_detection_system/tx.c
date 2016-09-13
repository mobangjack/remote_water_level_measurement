//�����豸

#include"tx.h"

void tx()
{
	unsigned int s=0;
	unsigned char TxBuf[TX_PLOAD_WIDTH];//���ͻ���

	init_NRF24L01();		//nrf24l01��ʼ��
	LCD_Init();    //��ʼ��LCD

	while(1)
	{
		s = (unsigned int)measureDistance();

		TxBuf[0] = s/100%10+'0';
		TxBuf[1] = s/10%10+'0';
		TxBuf[2] = s%10+'0';

		LCD_Write_String(0,0,TxBuf,TX_PLOAD_WIDTH); //��ʾ�������

		nrf_RxTx('t',TxBuf);//��������

		ms(250);//��ʱ������������
		ms(250);//��ʱ������������

	}
}