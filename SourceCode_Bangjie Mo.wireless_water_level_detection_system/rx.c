//接收设备
#include<stdio.h>
#include"rx.h"

/*按键及状态灯(P3^0~7)*/
sbit UPPER_TH_PLUS_KEY=P3^5;//上限阈值增按键
sbit UPPER_TH_MINUS_KEY=P3^6;//上限阈值减按键
sbit LOWER_TH_PLUS_KEY=P3^4;//下限阈值增按键
sbit LOWER_TH_MINUS_KEY=P3^7;//下限阈值减按键
sbit SENSOR_HEIGHT_PLUS_KEY=P1^3;//传感器安装高度增按键
sbit SENSOR_HEIGHT_MINUS_KEY=P1^1;//传感器安装高度减按键
sbit MODE_SETTING_KEY=P1^2;//测量模式设置按键
sbit BEEP_SETTING_KEY=P1^0;//蜂鸣器设置按键
sbit FULL_YELLOW_LED=P1^5;//状态指示:水位高于阈值上限
sbit EMPTY_RED_LED=P1^4;//状态指示:水位低于阈值下限
sbit NORMAL_GREEN_LED=P1^6;//状态指示:水位正常
sbit CURRENT_POWER_STATE=P1^7;//当前电源状态
sbit BEEP=P3^3;//蜂鸣器

#define ON 1	//定义电源状态开
#define OFF 0	//定义电源状态关

#define KEY_UP 1	//定义按键弹起
#define KEY_DOWN 0	//定义按键按下

#define STATE_FULL 		1		//定义满状态
#define STATE_EMPTY 	2		//定义空状态
#define STATE_NORMAL 	3		//定义正常状态

unsigned char CurrentState = STATE_NORMAL;//当前水位状态
unsigned int UpperTh = 25;//当前上限阈值
unsigned int LowerTh = 3;//当前下限阈值
unsigned int SensorHeight = 40;//当前传感器安装高度
bit CurrentMode = 0;//初始测量模式为非直接测量模式
bit BeepState = ON;//蜂鸣器初始状态：开

unsigned char key = 0x00;//键值

#define KEY_UTP 	0x0C
#define KEY_UTM		0x42
#define KEY_LTP		0x18
#define KEY_LTM		0x52
#define KEY_SHP		0x5E
#define KEY_SHM 	0x4A
#define KEY_MODE 	0x46
#define KEY_BEEP 	0x47

//接收缓存
unsigned char RxBuf[RX_PLOAD_WIDTH];

//当前获取到的数据
unsigned int current = 0;

//前一帧数据
unsigned int prev = 0;

//显示缓存
unsigned char buf1[17];
unsigned char buf3[17];
unsigned char buf5[17];
unsigned char buf7[17];

//点亮状态灯
void light(bit full,bit empty,bit normal){
	FULL_YELLOW_LED = full;
	EMPTY_RED_LED = empty;
	NORMAL_GREEN_LED = normal;
}
//初始化
void init(){
	//初始化电源状态
	CURRENT_POWER_STATE = OFF;

	//初始化蜂鸣器
	BEEP = OFF;

	//初始化状态指示灯
	light(OFF,OFF,OFF);

	//初始化NRF24L01
	init_NRF24L01();

	//初始化液晶
	initial_lcd();	

	//初始化红外
	IR_init();
	
}
//显示logo
void logo(){
	clear_screen();    //clear all dots		
	display_GB2312_string(1,1,"水位集中控制系统");
	display_string_5x7(5,1,"Created By MobangJack");
	display_GB2312_string(7,1,"     启动中     ");
	s(2);
}
//显示缓冲数据
void display(){	
	display_GB2312_string(1,1,buf1);
	display_GB2312_string(3,1,buf3);
	display_GB2312_string(5,1,buf5);
	display_GB2312_string(7,1,buf7);
}
//按键音
void beepKey(){
	BEEP = ON;
	ms(60);
	BEEP = OFF;
}
char code Tab[16]="0123456789ABCDEF";
//响应按键操作
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
		sprintf(buf3,"%16s","当前水位阈值上限");
		sprintf(buf5,"%14dcm",UpperTh);
	}else if(key==KEY_LTP||key==KEY_LTM){
		if(key==KEY_LTP){
			LowerTh++;
		}else{
			LowerTh--;
		}
		sprintf(buf3,"%16s","当前水位阈值下限");
		sprintf(buf5,"%14dcm",LowerTh);
	}else if(key==KEY_SHP||key==KEY_SHM){
		if(key==KEY_SHP){
			SensorHeight++;
		}else{
			SensorHeight--;
		}
		sprintf(buf3,"%16s","当前声呐探头高度");
		sprintf(buf5,"%14dcm",SensorHeight);
	}else if(key==KEY_MODE){
		//切换模式
		CurrentMode=!CurrentMode;
		sprintf(buf3,"%16s","当前测量模式为：");
		sprintf(buf5,"%16s",CurrentMode==1?"        直接测量":"        间接测量");
	}else if(key==KEY_BEEP){
		//打开或关闭蜂鸣器
		BeepState=!BeepState;
		sprintf(buf3,"%16s","当前蜂鸣器状态：");
		sprintf(buf5,"%16s",BeepState==ON?"            打开":"            关闭");
	}
	beepKey();
	display();
	key = 0x00;//清空键值
	ms(40);//维持显示40ms
}

//检测按键
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
//解析数据
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
//报警
void beep(){
	BEEP = ON;
	ms(100);
	BEEP = OFF;
}
//全局更新
void update(){
	sprintf(buf1,"%16s",CURRENT_POWER_STATE==ON?"电源:         开":"电源:         关");
	sprintf(buf3,"水位:%9dcm",current);
	sprintf(buf5,"占比:%10.0f%%",(((float)current*100)/UpperTh));
	if(current>=UpperTh-1&&prev>=UpperTh){
		//切断电源
		if(CURRENT_POWER_STATE==ON)
			CURRENT_POWER_STATE = OFF;

		//黄灯亮，表示满
		light(ON,OFF,OFF);

		sprintf(buf7,"%16s","状态:         满");

		CurrentState = STATE_FULL;

	}else if(current<=LowerTh&&prev<=LowerTh){
		//开启电源
		if(CURRENT_POWER_STATE==OFF)
			CURRENT_POWER_STATE = ON;

		//红灯亮，表示空
		light(OFF,ON,OFF);

		sprintf(buf7,"%16s","状态:         空");

		CurrentState = STATE_EMPTY;

	}else{
		light(OFF,OFF,ON);
		sprintf(buf7,"%16s","状态:       正常");
		CurrentState = STATE_NORMAL;
	}
	display();
	if(BeepState==ON&&CurrentState!=STATE_NORMAL){
		beep();
	}
	
}

void rx()
{
	//临时交换数据
	unsigned int tmp = 0;

	init();		//初始化

	logo();		//显示logo

	while(1)
	{
		if(checkKeys()){
			responseKeys();
			continue;
		}	
		
		//如果接收成功则刷新显示
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
		
		ms(100);//延时决定采样速率
		
	}
}