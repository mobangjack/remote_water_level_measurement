#include<reg52.h>
#include"delay.h"
#include"nrf24l01.h"

/*NRF24L01 IO�˿ڶ���*/
sbit CE=P2^3;
sbit CSN=P2^7;
sbit SCK=P2^2;
sbit MOSI=P2^0;
sbit MISO=P2^1;
//sbit IRQ=P1^7;

//*****************************NRF24L01�Ľ��պͷ��͵�ַ***********************************//
unsigned char const TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
unsigned char const RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ	
//****************************************״̬��־****************************************//
unsigned char bdata sta;
sbit	RX_DR=sta^6;
sbit	TX_DS=sta^5;
sbit	MAX_RT=sta^4;
/****************************************************************************************************
/*NRF24L01��SPIʱ��
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
/*NRF24L01�Ķ��Ĵ�������
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
/*NRF24L01д�Ĵ�������
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
/*д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��unsigned chars��д�����ݵĸ���
/*********************************************************************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char chars)
{
	unsigned char status,char_ctr;
	
	CSN = 0;            //SPIʹ��       
	status = SPI_RW(reg);   
	for(char_ctr=0; char_ctr<chars; char_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //�ر�SPI
	return(status);    // 
}
//********************************NRF24L01��ʼ��******************************************//
void init_NRF24L01()
{
 	CE=0;    // Ƭѡʹ��
 	CSN=1;   // SPI disable
 	SCK=0;   // SPIʱ������
	//IRQ=1;
}
//���Ӧ���ź�
bit checkAck()
{
	sta = SPI_Read_Reg(STATUS);//��ȡ״̬�Ĵ�����ֵ
	if(TX_DS||MAX_RT){
		SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
		SPI_Write_Reg(FLUSH_TX,0x00); //���tx fifo�Ĵ���
		return(1);
	}
	return(0);
}
//********** nrf�շ�����,����״̬��ʶ **********
bit nrf_RxTx(unsigned char mod_nrf,unsigned char *buf) //NRF24L01�շ�����	
{	
	//******���뷢��ģʽ******
	if(mod_nrf == 't')
	{
		CE = 0;
		SPI_Write_Buf(WRITE_REG + TX_ADDR, RX_ADDRESS, RX_ADR_WIDTH);     // װ�ؽ��ն˵�ַ
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); //дͨ��0���յ�ַ    
		SPI_Write_Buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH); 			// װ������

		//�Ĵ�������
		SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                             //ͨ��0�Զ�Ӧ�� 	
		SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                         //������յ�ַƵ��0 
		SPI_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a); 						// �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
		SPI_Write_Reg(WRITE_REG + RF_CH, 0x40);                             //�����ŵ�����Ƶ�ʣ��շ�����һ��
		SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);   		                 //���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB	
		SPI_Write_Reg(WRITE_REG + CONFIG, 0x0e);							//CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ     
		CE = 1;
		us(10); //��ʱ10us���ϣ��������ݷ���

		while(1){
			if(checkAck()){
				return(1);
			}
		}
		return(0);
	} 
	//******�������ģʽ******
	else if(mod_nrf == 'r')//����ģʽ
	{
		CE = 0;
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);    //д���ص�ַ
	
		//�Ĵ�������
		SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                             //ͨ��0�Զ�Ӧ�� 	
		SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                         //������յ�ַƵ��0 
		SPI_Write_Reg(WRITE_REG + RF_CH, 0x40);                             //�����ŵ�����Ƶ�ʣ��շ�����һ��
		SPI_Write_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);                //���ý������ݳ���
		SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);   		                 //���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB	
		SPI_Write_Reg(WRITE_REG + CONFIG, 0x0f);							//CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ  
		CE = 1;
		us(130);//��ʱ130us���ϣ�׼������

		sta = SPI_Read_Reg(STATUS); 

		if(RX_DR){
			CE = 0;//����
			SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
			SPI_Read_Buf(RD_RX_PLOAD,buf,RX_PLOAD_WIDTH);//��ȡ����
			SPI_Write_Reg(FLUSH_RX,0x00);//���rx fifo�Ĵ���
			return(1);
		}
		return(0);

	 }	

}
