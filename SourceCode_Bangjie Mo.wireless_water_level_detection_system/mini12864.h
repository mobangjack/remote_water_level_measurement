#ifndef _MINI12864_H
#define _MINI12864_H

/*写指令到LCD模块*/
void transfer_command_lcd(int data1);
/*写数据到LCD模块*/
void transfer_data_lcd(int data1);
/*LCD模块初始化*/
void initial_lcd();
/*LCD地址*/
void lcd_address(unsigned int page,unsigned int column);
/*全屏清屏*/
void clear_screen();
/*显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标*/
void display_graphic_16x16(unsigned int page,unsigned int column,unsigned char *dp);
/*显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标*/
void display_graphic_8x16(unsigned int page,unsigned char column,unsigned char *dp);
/*显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标*/
void display_graphic_5x7(unsigned int page,unsigned char column,unsigned char *dp);
/****送指令到字库IC***/
void send_command_to_ROM(unsigned char datu);
/****从字库IC中取汉字或字符数据（1个字节）***/
unsigned char get_data_from_ROM();
/*从相关地址（addrHigh：地址高字节,addrMid：地址中字节,addrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址,连续读取*/
void get_n_bytes_data_from_ROM(unsigned char addrHigh,unsigned char addrMid,unsigned char addrLow,unsigned char *pBuff,unsigned char DataLen);
/*显示国标字符串*/
void display_GB2312_string(unsigned char y,unsigned char x,unsigned char *text);
/*显示5x7字符串*/
void display_string_5x7(unsigned char y,unsigned char x,unsigned char *text);

#endif