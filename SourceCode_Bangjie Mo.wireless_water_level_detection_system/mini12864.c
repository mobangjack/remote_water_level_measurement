/* ���Һ��ģ���ͺţ�mini12864 
   ���нӿڣ�
   �������ֿ�IC�����ú���
   ����IC��:ST7565R(or compatible)
*/
#include <reg52.h>
#include "delay.h"
#include "mini12864.h"

/*mini12864 IO�˿ڶ���*/
sbit lcd_sclk=P0^0;  /*�ӿڶ���:lcd_sclk����LCD��sclk*/
sbit lcd_sid=P0^1;   /*�ӿڶ���:lcd_sid����LCD��sid*/
sbit lcd_rs=P2^6;    /*�ӿڶ���:lcd_rs����LCD��A0*/
sbit lcd_reset=P2^5; /*�ӿڶ���:lcd_reset����LCD��reset*/
sbit lcd_cs1=P2^4;   /*�ӿڶ���:lcd_cs1����LCD��cs1*/

sbit Rom_IN=P0^3;    /*�ֿ�IC�ӿڶ���:Rom_IN�����ֿ�IC��SI*/
sbit Rom_OUT=P0^2;   /*�ֿ�IC�ӿڶ���:Rom_OUT�����ֿ�IC��SO*/
sbit Rom_SCK=P0^4;   /*�ֿ�IC�ӿڶ���:Rom_SCK�����ֿ�IC��SCK*/
sbit Rom_CS=P0^5;    /*�ֿ�IC�ӿڶ���Rom_CS�����ֿ�IC��CS*/

/*дָ�LCDģ��*/
void transfer_command_lcd(int data1)   
{
	char i;
	lcd_rs=0;
	for(i=0;i<8;i++)
   {
		lcd_sclk=0;
		if(data1&0x80) lcd_sid=1;
		else lcd_sid=0;
		lcd_sclk=1;
	 	data1=data1<<=1;
   }
}

/*д���ݵ�LCDģ��*/
void transfer_data_lcd(int data1)
{
	char i;
	lcd_rs=1;
	for(i=0;i<8;i++)
   {
		lcd_sclk=0;
		if(data1&0x80) lcd_sid=1;
		else lcd_sid=0;
		lcd_sclk=1;
	 	data1=data1<<=1;
   }
}

/*LCDģ���ʼ��*/
void initial_lcd()
{
	lcd_cs1=0;
	Rom_CS = 1;
	lcd_reset=0;        /*�͵�ƽ��λ*/
	ms(20);
	lcd_reset=1;		    /*��λ���*/
	ms(20);        
	transfer_command_lcd(0xe2);	 /*��λ*/
	ms(5);
	transfer_command_lcd(0x2c);  /*��ѹ����1*/
	ms(5);	
	transfer_command_lcd(0x2e);  /*��ѹ����2*/
	ms(5);
	transfer_command_lcd(0x2f);  /*��ѹ����3*/
	ms(5);
	transfer_command_lcd(0x23);  /*�ֵ��Աȶȣ������÷�Χ0x20��0x27*/
	transfer_command_lcd(0x81);  /*΢���Աȶ�*/
	transfer_command_lcd(0x2f);  /*0x28,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f*/
	transfer_command_lcd(0xa2);  /*1/9ƫѹ�ȣ�bias��*/
	transfer_command_lcd(0xc8);  /*��ɨ��˳�򣺴��ϵ���*/
	transfer_command_lcd(0xa0);  /*��ɨ��˳�򣺴�����*/
	transfer_command_lcd(0x40);  /*��ʼ�У���һ�п�ʼ*/
	transfer_command_lcd(0xaf);  /*����ʾ*/
	lcd_cs1=1;
}

void lcd_address(unsigned int page,unsigned int column)
{

	column=column-0x01;
	transfer_command_lcd(0xb0+page-1);   /*����ҳ��ַ*/
	transfer_command_lcd(0x10+(column>>4&0x0f));	/*�����е�ַ�ĸ�4λ*/
	transfer_command_lcd(column&0x0f);	/*�����е�ַ�ĵ�4λ*/	
}

/*ȫ������*/
void clear_screen()
{
	unsigned char i,j;
	lcd_cs1=0;
	Rom_CS = 1;	
	for(i=0;i<9;i++)
	{
		transfer_command_lcd(0xb0+i);
		transfer_command_lcd(0x10);
		transfer_command_lcd(0x00);
		for(j=0;j<132;j++)
		{
		  	transfer_data_lcd(0x00);
		}
	}
 	lcd_cs1=1;
}



/*��ʾ16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��*/
void display_graphic_16x16(unsigned int page,unsigned int column,unsigned char *dp)
{
	unsigned int i,j;
 	lcd_cs1=0;
	Rom_CS = 1; 	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{	
			transfer_data_lcd(*dp);					/*д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1*/
			dp++;
		}
	}
	lcd_cs1=1;
}


/*��ʾ8x16����ͼ��ASCII, ��8x16����������ַ�������ͼ��*/
void display_graphic_8x16(unsigned int page,unsigned char column,unsigned char *dp)
{
	unsigned int i,j;
	lcd_cs1=0;	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{	
			transfer_data_lcd(*dp);					/*д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1*/
			dp++;
		}
	}
	lcd_cs1=1;
}


/*��ʾ5*7����ͼ��ASCII, ��5x7����������ַ�������ͼ��*/
void display_graphic_5x7(unsigned int page,unsigned char column,unsigned char *dp)
{
	unsigned int col_cnt;
	unsigned char page_address;
	unsigned char column_address_L,column_address_H;
	page_address = 0xb0+page-1;
	
	lcd_cs1=0;	
	
	column_address_L =(column&0x0f)-1;
	column_address_H =((column>>4)&0x0f)+0x10;
	
	transfer_command_lcd(page_address); 		/*Set Page Address*/
	transfer_command_lcd(column_address_H);	/*Set MSB of column Address*/
	transfer_command_lcd(column_address_L);	/*Set LSB of column Address*/
	
	for (col_cnt=0;col_cnt<6;col_cnt++)
	{	
		transfer_data_lcd(*dp);
		dp++;
	}
	lcd_cs1=1;
}

/****��ָ��ֿ�IC***/
void send_command_to_ROM( unsigned char datu )
{
	unsigned char i;
	for(i=0;i<8;i++ )
	{
		if(datu&0x80)
			Rom_IN = 1;
		else
			Rom_IN = 0;
			datu = datu<<1;
			Rom_SCK=0;
			Rom_SCK=1;
	}
}

/****���ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/
unsigned char get_data_from_ROM()
{
	
	unsigned char i;
	unsigned char ret_data=0;
	Rom_SCK=1;
	for(i=0;i<8;i++)
	{
		Rom_OUT=1;
		Rom_SCK=0;
		ret_data=ret_data<<1;
		if( Rom_OUT )
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		Rom_SCK=1;
	}
	return(ret_data);
}



/*����ص�ַ��addrHigh����ַ���ֽ�,addrMid����ַ���ֽ�,addrLow����ַ���ֽڣ�����������DataLen���ֽڵ����ݵ� pBuff�ĵ�ַ*/
/*������ȡ*/
void get_n_bytes_data_from_ROM(unsigned char addrHigh,unsigned char addrMid,unsigned char addrLow,unsigned char *pBuff,unsigned char DataLen )
{
	unsigned char i;
	Rom_CS = 0;
	lcd_cs1=1;	
	Rom_SCK=0;
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();
	Rom_CS = 1;
}


/******************************************************************/

unsigned long  fontaddr=0;
void display_GB2312_string(unsigned char y,unsigned char x,unsigned char *text)
{
	unsigned char i= 0;
	unsigned char addrHigh,addrMid,addrLow ;
	unsigned char fontbuf[32];			
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			/*������壨GB2312���������ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ*/
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (unsigned long)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			addrMid = (fontaddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			addrLow = fontaddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*ȡ32���ֽڵ����ݣ��浽"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*��ʾ���ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=2;
			x+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			/*������壨GB2312��15x16����ַ����ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ*/
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (unsigned long)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			addrMid = (fontaddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			addrLow = fontaddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*ȡ32���ֽڵ����ݣ��浽"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*��ʾ���ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=2;
			x+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );/*ȡ16���ֽڵ����ݣ��浽"fontbuf[32]"*/
			
			display_graphic_8x16(y,x,fontbuf);/*��ʾ8x16��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=1;
			x+=8;
		}
		else
			i++;	
	}
	
}

void display_string_5x7(unsigned char y,unsigned char x,unsigned char *text)
{
	unsigned char i= 0;
	unsigned char addrHigh,addrMid,addrLow ;
	while((text[i]>0x00))
	{
		
		if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[8];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);/*ȡ8���ֽڵ����ݣ��浽"fontbuf[32]"*/
			
			display_graphic_5x7(y,x,fontbuf);/*��ʾ5x7��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=1;
			x+=6;
		}
		else
		i++;	
	}
	
}


