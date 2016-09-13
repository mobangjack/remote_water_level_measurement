#ifndef _MINI12864_H
#define _MINI12864_H

/*дָ�LCDģ��*/
void transfer_command_lcd(int data1);
/*д���ݵ�LCDģ��*/
void transfer_data_lcd(int data1);
/*LCDģ���ʼ��*/
void initial_lcd();
/*LCD��ַ*/
void lcd_address(unsigned int page,unsigned int column);
/*ȫ������*/
void clear_screen();
/*��ʾ16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��*/
void display_graphic_16x16(unsigned int page,unsigned int column,unsigned char *dp);
/*��ʾ8x16����ͼ��ASCII, ��8x16����������ַ�������ͼ��*/
void display_graphic_8x16(unsigned int page,unsigned char column,unsigned char *dp);
/*��ʾ5*7����ͼ��ASCII, ��5x7����������ַ�������ͼ��*/
void display_graphic_5x7(unsigned int page,unsigned char column,unsigned char *dp);
/****��ָ��ֿ�IC***/
void send_command_to_ROM(unsigned char datu);
/****���ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/
unsigned char get_data_from_ROM();
/*����ص�ַ��addrHigh����ַ���ֽ�,addrMid����ַ���ֽ�,addrLow����ַ���ֽڣ�����������DataLen���ֽڵ����ݵ� pBuff�ĵ�ַ,������ȡ*/
void get_n_bytes_data_from_ROM(unsigned char addrHigh,unsigned char addrMid,unsigned char addrLow,unsigned char *pBuff,unsigned char DataLen);
/*��ʾ�����ַ���*/
void display_GB2312_string(unsigned char y,unsigned char x,unsigned char *text);
/*��ʾ5x7�ַ���*/
void display_string_5x7(unsigned char y,unsigned char x,unsigned char *text);

#endif