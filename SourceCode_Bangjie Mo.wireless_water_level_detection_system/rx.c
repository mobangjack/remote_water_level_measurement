//�����豸
#include<stdio.h>
#include"rx.h"

/*������״̬��(P3^0~7)*/
sbit UPPER_TH_PLUS_KEY=P3^5;//������ֵ������
sbit UPPER_TH_MINUS_KEY=P3^6;//������ֵ������
sbit LOWER_TH_PLUS_KEY=P3^4;//������ֵ������
sbit LOWER_TH_MINUS_KEY=P3^7;//������ֵ������
sbit SENSOR_HEIGHT_PLUS_KEY=P1^3;//��������װ�߶�������
sbit SENSOR_HEIGHT_MINUS_KEY=P1^1;//��������װ�߶ȼ�����
sbit MODE_SETTING_KEY=P1^2;//����ģʽ���ð���
sbit BEEP_SETTING_KEY=P1^0;//���������ð���
sbit FULL_YELLOW_LED=P1^5;//״ָ̬ʾ:ˮλ������ֵ����
sbit EMPTY_RED_LED=P1^4;//״ָ̬ʾ:ˮλ������ֵ����
sbit NORMAL_GREEN_LED=P1^6;//״ָ̬ʾ:ˮλ����
sbit CURRENT_POWER_STATE=P1^7;//��ǰ��Դ״̬
sbit BEEP=P3^3;//������

#define ON 1	//�����Դ״̬��
#define OFF 0	//�����Դ״̬��

#define KEY_UP 1	//���尴������
#define KEY_DOWN 0	//���尴������

#define STATE_FULL 		1		//������״̬
#define STATE_EMPTY 	2		//�����״̬
#define STATE_NORMAL 	3		//��������״̬

unsigned char CurrentState = STATE_NORMAL;//��ǰˮλ״̬
unsigned int UpperTh = 25;//��ǰ������ֵ
unsigned int LowerTh = 3;//��ǰ������ֵ
unsigned int SensorHeight = 40;//��ǰ��������װ�߶�
bit CurrentMode = 0;//��ʼ����ģʽΪ��ֱ�Ӳ���ģʽ
bit BeepState = ON;//��������ʼ״̬����

unsigned char key = 0x00;//��ֵ

#define KEY_UTP 	0x0C
#define KEY_UTM		0x42
#define KEY_LTP		0x18
#define KEY_LTM		0x52
#define KEY_SHP		0x5E
#define KEY_SHM 	0x4A
#define KEY_MODE 	0x46
#define KEY_BEEP 	0x47

//���ջ���
unsigned char RxBuf[RX_PLOAD_WIDTH];

//��ǰ��ȡ��������
unsigned int current = 0;

//ǰһ֡����
unsigned int prev = 0;

//��ʾ����
unsigned char buf1[17];
unsigned char buf3[17];
unsigned char buf5[17];
unsigned char buf7[17];

//����״̬��
void light(bit full,bit empty,bit normal){
	FULL_YELLOW_LED = full;
	EMPTY_RED_LED = empty;
	NORMAL_GREEN_LED = normal;
}
//��ʼ��
void init(){
	//��ʼ����Դ״̬
	CURRENT_POWER_STATE = OFF;

	//��ʼ��������
	BEEP = OFF;

	//��ʼ��״ָ̬ʾ��
	light(OFF,OFF,OFF);

	//��ʼ��NRF24L01
	init_NRF24L01();

	//��ʼ��Һ��
	initial_lcd();	

	//��ʼ������
	IR_init();
	
}
//��ʾlogo
void logo(){
	clear_screen();    //clear all dots		
	display_GB2312_string(1,1,"ˮλ���п���ϵͳ");
	display_string_5x7(5,1,"Created By MobangJack");
	display_GB2312_string(7,1,"     ������     ");
	s(2);
}
//��ʾ��������
void display(){	
	display_GB2312_string(1,1,buf1);
	display_GB2312_string(3,1,buf3);
	display_GB2312_string(5,1,buf5);
	display_GB2312_string(7,1,buf7);
}
//������
void beepKey(){
	BEEP = ON;
	ms(60);
	BEEP = OFF;
}
char code Tab[16]="0123456789ABCDEF";
//��Ӧ��������
void responseKeys(){
	sprintf(buf1,"key:          %c%c",Tab[key/16],Tab[key%16]);
	sprintf(buf3,"%16s","                ");
	sprintf(buf5,"%16s","                ");
	sprintf(buf7,"%16s","                ");
	if(key==KEY_UTP||key==KEY_UTM){
		if(key==KEY_UTP){
			UpperTh++;
		}else{
			UpperTh--;
		}
		sprintf(buf3,"%16s","��ǰˮλ��ֵ����");
		sprintf(buf5,"%14dcm",UpperTh);
	}else if(key==KEY_LTP||key==KEY_LTM){
		if(key==KEY_LTP){
			LowerTh++;
		}else{
			LowerTh--;
		}
		sprintf(buf3,"%16s","��ǰˮλ��ֵ����");
		sprintf(buf5,"%14dcm",LowerTh);
	}else if(key==KEY_SHP||key==KEY_SHM){
		if(key==KEY_SHP){
			SensorHeight++;
		}else{
			SensorHeight--;
		}
		sprintf(buf3,"%16s","��ǰ����̽ͷ�߶�");
		sprintf(buf5,"%14dcm",SensorHeight);
	}else if(key==KEY_MODE){
		//�л�ģʽ
		CurrentMode=!CurrentMode;
		sprintf(buf3,"%16s","��ǰ����ģʽΪ��");
		sprintf(buf5,"%16s",CurrentMode==1?"        ֱ�Ӳ���":"        ��Ӳ���");
	}else if(key==KEY_BEEP){
		//�򿪻�رշ�����
		BeepState=!BeepState;
		sprintf(buf3,"%16s","��ǰ������״̬��");
		sprintf(buf5,"%16s",BeepState==ON?"            ��":"            �ر�");
	}
	beepKey();
	display();
	key = 0x00;//��ռ�ֵ
	ms(40);//ά����ʾ40ms
}

//��ⰴ��
bit checkKeys(){
	key = scanIr();
	if(key)
		return(1);
	else if(UPPER_TH_PLUS_KEY==KEY_DOWN||UPPER_TH_MINUS_KEY==KEY_DOWN||LOWER_TH_PLUS_KEY==KEY_DOWN||LOWER_TH_MINUS_KEY==KEY_DOWN||SENSOR_HEIGHT_PLUS_KEY==KEY_DOWN||SENSOR_HEIGHT_MINUS_KEY==KEY_DOWN||MODE_SETTING_KEY==KEY_DOWN||BEEP_SETTING_KEY==KEY_DOWN){
		ms(5);
		if(UPPER_TH_PLUS_KEY==KEY_DOWN||UPPER_TH_MINUS_KEY==KEY_DOWN||LOWER_TH_PLUS_KEY==KEY_DOWN||LOWER_TH_MINUS_KEY==KEY_DOWN||SENSOR_HEIGHT_PLUS_KEY==KEY_DOWN||SENSOR_HEIGHT_MINUS_KEY==KEY_DOWN||MODE_SETTING_KEY==KEY_DOWN||BEEP_SETTING_KEY==KEY_DOWN){
			if(UPPER_TH_PLUS_KEY==KEY_DOWN)
				key = KEY_UTP;
			else if(UPPER_TH_MINUS_KEY==KEY_DOWN)
				key = KEY_UTM;
			else if(LOWER_TH_PLUS_KEY==KEY_DOWN)
				key = KEY_LTP;
			else if(LOWER_TH_MINUS_KEY==KEY_DOWN)
				key = KEY_LTM;
			else if(SENSOR_HEIGHT_PLUS_KEY==KEY_DOWN)
				key = KEY_SHP;
			else if(SENSOR_HEIGHT_MINUS_KEY==KEY_DOWN)
				key = KEY_SHM;
			else if(MODE_SETTING_KEY==KEY_DOWN)
				key = KEY_MODE;
			else if(BEEP_SETTING_KEY==KEY_DOWN)
				key = KEY_BEEP;
			return(1);
		}  
	}
	return(0);
}
//��������
unsigned int parse(unsigned char* str,unsigned char len){
	unsigned int tmp=0;
	unsigned int k=1;
	while(len--){
		if(str[len]>='0'&&str[len]<='9'){
			tmp+=(str[len]-'0')*k;
			k*=10;
		}
	}
	return tmp;
}
//����
void beep(){
	BEEP = ON;
	ms(100);
	BEEP = OFF;
}
//ȫ�ָ���
void update(){
	sprintf(buf1,"%16s",CURRENT_POWER_STATE==ON?"��Դ:         ��":"��Դ:         ��");
	sprintf(buf3,"ˮλ:%9dcm",current);
	sprintf(buf5,"ռ��:%10.0f%%",(((float)current*100)/UpperTh));
	if(current>=UpperTh-1&&prev>=UpperTh){
		//�жϵ�Դ
		if(CURRENT_POWER_STATE==ON)
			CURRENT_POWER_STATE = OFF;

		//�Ƶ�������ʾ��
		light(ON,OFF,OFF);

		sprintf(buf7,"%16s","״̬:         ��");

		CurrentState = STATE_FULL;

	}else if(current<=LowerTh&&prev<=LowerTh){
		//������Դ
		if(CURRENT_POWER_STATE==OFF)
			CURRENT_POWER_STATE = ON;

		//���������ʾ��
		light(OFF,ON,OFF);

		sprintf(buf7,"%16s","״̬:         ��");

		CurrentState = STATE_EMPTY;

	}else{
		light(OFF,OFF,ON);
		sprintf(buf7,"%16s","״̬:       ����");
		CurrentState = STATE_NORMAL;
	}
	display();
	if(BeepState==ON&&CurrentState!=STATE_NORMAL){
		beep();
	}
	
}

void rx()
{
	//��ʱ��������
	unsigned int tmp = 0;

	init();		//��ʼ��

	logo();		//��ʾlogo

	while(1)
	{
		if(checkKeys()){
			responseKeys();
			continue;
		}	
		
		//������ճɹ���ˢ����ʾ
		if(nrf_RxTx('r',RxBuf)){
			tmp = parse(RxBuf,RX_PLOAD_WIDTH);
			if(tmp>0){
				if(CurrentMode==0){
					tmp = SensorHeight-tmp;
					tmp = tmp<0?SensorHeight:tmp;
				}
				prev = current;
				current = tmp;
				update();

			}else{
				continue;
			}
		}
		
		ms(100);//��ʱ������������
		
	}
}