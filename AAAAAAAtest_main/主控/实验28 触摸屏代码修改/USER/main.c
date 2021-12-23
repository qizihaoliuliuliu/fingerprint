#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h"
#include "sram.h"
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"
#include "touch.h"
#include "usart3.h"
#include "common.h" 
#include "stdlib.h"
#include "string.h"
#include "beep.h"

void load_ui(u16 x,u16 y)
{
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y,x+300,y+200);
	LCD_DrawLine(x,y+100,x+300,y+100);
	LCD_DrawLine(x+150,y,x+150,y+200);
	LCD_Fill(x+2,y+2,x+148,y+98,MAGENTA);//控制LED0
	LCD_Fill(x+152,y+2,x+298,y+98,CYAN);//控制LED1
	LCD_Fill(x+2,y+102,x+148,y+198,BRRED);//控制LED0 LED1开
	LCD_Fill(x+152,y+102,x+298,y+198,LGRAY);//控制LED0 LED1关
	POINT_COLOR=BLACK;
	LCD_ShowString(x+35,y+38,lcddev.width,lcddev.height,24,"LED0_ON");
	LCD_ShowString(x+182,y+38,lcddev.width,lcddev.height,24,"LED1_ON");
	LCD_ShowString(x+30,y+138,lcddev.width,lcddev.height,24,"LED0_OFF");
	LCD_ShowString(x+174,y+138,lcddev.width,lcddev.height,24,"LED1_OFF");
}

void load_ui_1(u16 x,u16 y)
{
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y,x+150,y+200);
	LCD_DrawLine(x,y+100,x+150,y+100);
	LCD_Fill(x+2,y+2,x+148,y+98,MAGENTA);
	LCD_Fill(x+2,y+102,x+148,y+198,CYAN);
	POINT_COLOR=BLACK;
	LCD_ShowString(x+35,y+38,lcddev.width,lcddev.height,24,"LED0_ON");
	LCD_ShowString(x+30,y+138,lcddev.width,lcddev.height,24,"LED0_OFF");
}

void load_ui_2(u16 x,u16 y)
{
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x+150,y,x+300,y+200);
	LCD_DrawLine(x+150,y+100,x+300,y+100);
	LCD_Fill(x+152,y+2,x+298,y+98,BRRED);
	LCD_Fill(x+152,y+102,x+298,y+198,LGRAY);
	POINT_COLOR=BLACK;
	LCD_ShowString(x+182,y+38,lcddev.width,lcddev.height,24,"LED1_ON");
	LCD_ShowString(x+174,y+138,lcddev.width,lcddev.height,24,"LED1_OFF");
}

void load_ui_light(u16 x,u16 y)
{	
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y,x+475,y+280);
	LCD_Fill(x+2,y+2,x+473,y+278,LGRAY);
	POINT_COLOR = BLACK;
	LCD_DrawLine(x+27,y+27,x+27,y+127);
	LCD_DrawLine(x+447,y+27,x+447,y+127);
	LCD_DrawLine(x+27,y+27,x+164,y+27);
	LCD_DrawLine(x+164,y+27,x+167,y+30);
	LCD_DrawLine(x+167,y+30,x+170,y+27);
	LCD_DrawLine(x+170,y+27,x+304,y+27);
	LCD_DrawLine(x+304,y+27,x+307,y+30);
	LCD_DrawLine(x+307,y+30,x+310,y+27);
	LCD_DrawLine(x+310,y+27,x+447,y+27);
	
	LCD_DrawLine(x+27,y+127,x+164,y+127);
	LCD_DrawLine(x+164,y+127,x+167,y+124);
	LCD_DrawLine(x+167,y+124,x+170,y+127);
	LCD_DrawLine(x+170,y+127,x+304,y+127);
	LCD_DrawLine(x+304,y+127,x+307,y+124);
	LCD_DrawLine(x+307,y+124,x+310,y+127);
	LCD_DrawLine(x+310,y+127,x+447,y+127);
	
	LCD_Fill(x+29,y+29,x+162,y+125,WHITE);
	LCD_Fill(x+172,y+29,x+302,y+125,WHITE);
	LCD_Fill(x+312,y+29,x+445,y+125,WHITE);
	
	POINT_COLOR = BLACK;
	LCD_ShowString(x+93,y+60,lcddev.width,lcddev.height,24,"-");
	LCD_ShowString(x+233,y+60,lcddev.width,lcddev.height,24,"@");
	LCD_ShowString(x+373,y+60,lcddev.width,lcddev.height,24,"+");
}

void load_ui_temperature(u16 x,u16 y)
{
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y,x+475,y+200);
	LCD_DrawLine(x+230,y,x+230,y+200);
	LCD_Fill(x+2,y+2,x+473,y+198,LGRAY);
	LCD_ShowString(x+35,y+38,lcddev.width,lcddev.height,24,"fengshan:");
	LCD_ShowString(x+35,y+138,lcddev.width,lcddev.height,24,"jiashiqi:");
	LCD_DrawRectangle(x+255,y+20,x+355,y+80); LCD_DrawRectangle(x+355,y+20,x+455,y+80);
	LCD_DrawRectangle(x+255,y+115,x+355,y+175); LCD_DrawRectangle(x+355,y+115,x+455,y+175);
	LCD_Fill(x+257,y+22,x+353,y+78,WHITE);
	LCD_Fill(x+357,y+22,x+453,y+78,WHITE);
	LCD_Fill(x+257,y+117,x+353,y+173,WHITE);
	LCD_Fill(x+357,y+117,x+453,y+173,WHITE);
	POINT_COLOR = GREEN;
	LCD_ShowString(x+290,y+38,lcddev.width,lcddev.height,24,"ON");
	LCD_ShowString(x+390,y+38,lcddev.width,lcddev.height,24,"OFF");
	LCD_ShowString(x+290,y+133,lcddev.width,lcddev.height,24,"ON");
	LCD_ShowString(x+390,y+133,lcddev.width,lcddev.height,24,"OFF");
	
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y-300,x+475,y-20);
	LCD_DrawLine(x+230,y-300,x+230,y-20);
	LCD_Fill(x+2,y-298,x+473,y-22,LGRAY);
	POINT_COLOR = BLACK;
	LCD_ShowString(x+215,y-270,lcddev.width,lcddev.height,24,"+16");
	LCD_ShowString(x+200,y-200,lcddev.width,lcddev.height,24,"Sunny");
	LCD_ShowString(x+1,y-150,lcddev.width,lcddev.height,24,"Beijing University of Posts and Telecommunication");
	LCD_ShowString(x+190,y-70,lcddev.width,lcddev.height,24,"Tuesday");
}

void load_ui_menu(u16 x,u16 y)
{
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(x,y,x+475,y+100);
	LCD_DrawLine(x+118,y,x+118,y+100);
	LCD_DrawLine(x+236,y,x+236,y+100);
	LCD_DrawLine(x+354,y,x+354,y+100);
	LCD_Fill(x+2,y+2,x+116,y+98,MAGENTA);//控制LED0
	LCD_Fill(x+120,y+2,x+234,y+98,CYAN);//控制LED1
	LCD_Fill(x+238,y+2,x+354,y+98,BRRED);//控制LED0 LED1开
	LCD_Fill(x+356,y+2,x+473,y+98,LGRAY);//控制LED0 LED1关
	LCD_ShowString(x+50,y+38,lcddev.width,lcddev.height,24,"T");
	LCD_ShowString(x+170,y+38,lcddev.width,lcddev.height,24,"L");
	LCD_ShowString(x+290,y+38,lcddev.width,lcddev.height,24,"F");
	LCD_ShowString(x+410,y+38,lcddev.width,lcddev.height,24,"M");
}

void key_staset_LED(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16 i = keyx/2 , j = keyx%2;
	if(keyx > 8) return;
	if(sta) LCD_Fill(x+j*150+1,y+i*100+1,x+j*150+149,y+i*100+99,GREEN);
	else LCD_Fill(x+j*150+1,y+i*100+1,x+j*150+149,y+i*100+99,GRAY);
	POINT_COLOR = BLACK;
	LCD_ShowString(x+35,y+38,lcddev.width,lcddev.height,24,"LED0_ON");
	LCD_ShowString(x+182,y+38,lcddev.width,lcddev.height,24,"LED1_ON");
	LCD_ShowString(x+30,y+138,lcddev.width,lcddev.height,24,"LED0_OFF");
	LCD_ShowString(x+174,y+138,lcddev.width,lcddev.height,24,"LED1_OFF");
}

void key_staset_menu(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16 i = 0 , j = keyx%4;
	if(keyx > 8) return;
	if(sta) LCD_Fill(x+j*118+1,y+i*100+1,x+j*118+117,y+i*100+99,GREEN);
	else LCD_Fill(x+j*118+1,y+i*100+1,x+j*118+117,y+i*100+99,GREEN);
	POINT_COLOR = BLUE;
	LCD_ShowString(x+50,y+38,lcddev.width,lcddev.height,24,"T");
	LCD_ShowString(x+170,y+38,lcddev.width,lcddev.height,24,"L");
	LCD_ShowString(x+290,y+38,lcddev.width,lcddev.height,24,"F");
	LCD_ShowString(x+410,y+38,lcddev.width,lcddev.height,24,"M");
}

void key_staset_temperature_fengshan(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16 i = 0 , j = keyx%2;
	if(keyx > 8) return;
	if(sta) LCD_Fill(x+j*100+1,y+i*60+1,x+j*100+99,y+i*60+59,CYAN);
	else LCD_Fill(x+j*100+1,y+i*60+1,x+j*100+99,y+i*60+59,WHITE);
	POINT_COLOR = GREEN;
	LCD_ShowString(x+35,y+18,lcddev.width,lcddev.height,24,"ON");
	LCD_ShowString(x+135,y+18,lcddev.width,lcddev.height,24,"OFF");
}

void key_staset_temperature_jiashiqi(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16 i = 0 , j = keyx%2;
	if(keyx > 8) return;
	if(sta) LCD_Fill(x+j*100+1,y+i*60+1,x+j*100+99,y+i*60+59,CYAN);
	else LCD_Fill(x+j*100+1,y+i*60+1,x+j*100+99,y+i*60+59,WHITE);
	POINT_COLOR = GREEN;
	LCD_ShowString(x+35,y-77,lcddev.width,lcddev.height,24,"ON");
	LCD_ShowString(x+135,y-77,lcddev.width,lcddev.height,24,"OFF");
}

void key_staset_light(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16  j = keyx%2;
	if(keyx > 8) return;
	if(sta) LCD_Fill(x+j*280+29,y+29,x+j*280+165,y+125,CYAN);
	else LCD_Fill(x+j*280+29,y+29,x+j*280+165,y+125,WHITE);
	POINT_COLOR = BLACK;
	LCD_ShowString(x+93,y+60,lcddev.width,lcddev.height,24,"-");
	LCD_ShowString(x+233,y+60,lcddev.width,lcddev.height,24,"@");
	LCD_ShowString(x+373,y+60,lcddev.width,lcddev.height,24,"+");
}

u8 get_keynum_light(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x = 0; // 0表示没有任何键按下
	u8 key = 0;
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		for(i = 0 ; i < 1 ; i++)
		{
			for(j = 0 ; j < 2 ; j++)
			{
				if(tp_dev.x[0] < (x + j*280 + 167) && tp_dev.x[0] > (x + j*280 + 27) && 
					tp_dev.y[0] < (y + 127) && tp_dev.y[0] > (y + 27))
				{
					key = i*2 + j + 1;
					break;
				}
			}
			if(key)
			{
				if(key_x == key) key = 0;
				else
				{
					key_staset_light(x,y,key_x-1,0);
					key_x=key;
					key_staset_light(x,y,key_x-1,1);
				}
				break;
			}
		}
	}
	
	else if(key_x)
	{
		key_staset_light(x,y,key_x-1,0);
		key_x = 0;
	}
	
	return key;
}

u8 get_keynum_temperature_jiashiqi(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x = 0; // 0表示没有任何键按下
	u8 key = 0;
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		for(i = 0 ; i < 1 ; i++)
		{
			for(j = 0 ; j < 2 ; j++)
			{
				if(tp_dev.x[0] < (x+255 + j*100+100) && tp_dev.x[0] > (x+255 + j*100) && 
					tp_dev.y[0] < (y+115 + i*60+60) && tp_dev.y[0] > (y+115 + i*60))
				{
					key = i*2 + j + 1;
					break;
				}
			}
			if(key)
			{
				if(key_x == key) key = 0;
				else
				{
					key_staset_temperature_fengshan(x+255,y+115,key_x-1,0);
					key_x=key;
					key_staset_temperature_fengshan(x+255,y+115,key_x-1,1);
				}
				break;
			}
		}
	}
	
	//else if(key_x)
	//{
	//	key_staset_temperature_fengshan(x+255,y+115,key_x-1,0);
	//	key_x = 0;
	//}
	
	return key;
}

u8 get_keynum_temperature_fengshan(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x = 0; // 0表示没有任何键按下
	u8 key = 0;
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		for(i = 0 ; i < 1 ; i++)
		{
			for(j = 0 ; j < 2 ; j++)
			{
				if(tp_dev.x[0] < (x+255 + j*100+100) && tp_dev.x[0] > (x+255 + j*100) && 
					tp_dev.y[0] < (y+20 + i*60+60) && tp_dev.y[0] > (y+20 + i*60))
				{
					key = i*2 + j + 1;
					break;
				}
			}
			if(key)
			{
				if(key_x == key) key = 0;
				else
				{
					key_staset_temperature_fengshan(x+255,y+20,key_x-1,0);
					key_x=key;
					key_staset_temperature_fengshan(x+255,y+20,key_x-1,1);
				}
				break;
			}
		}
	}
	
	//else if(key_x)
	//{
	//	key_staset_temperature_fengshan(x+255,y+20,key_x-1,0);
	//	key_x = 0;
	//}
	
	return key;
}

u8 get_keynum_menu(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x = 0; // 0表示没有任何键按下
	u8 key = 0;
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		for(i = 0 ; i < 1 ; i++)
		{
			for(j = 0 ; j < 4 ; j++)
			{
				if(tp_dev.x[0] < (x + j*118+118) && tp_dev.x[0] > (x + j*118) && 
					tp_dev.y[0] < (y + i*100+100) && tp_dev.y[0] > (y + i*100))
				{
					key = i*2 + j + 1;
					break;
				}
			}
			if(key)
			{
				if(key_x == key) key = 0;
				else
				{
					key_staset_menu(x,y,key_x-1,0);
					key_x=key;
					key_staset_menu(x,y,key_x-1,1);
				}
				break;
			}
		}
	}
	
	else if(key_x)
	{
		key_staset_menu(x,y,key_x-1,0);
		key_x = 0;
	}
	
	return key;
}

u8 get_keynum_LED(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x = 0; // 0表示没有任何键按下
	u8 key = 0;
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		for(i = 0 ; i < 2 ; i++)
		{
			for(j = 0 ; j < 2 ; j++)
			{
				if(tp_dev.x[0] < (x + j*150+150) && tp_dev.x[0] > (x + j*150) && 
					tp_dev.y[0] < (y + i*100+100) && tp_dev.y[0] > (y + i*100))
				{
					key = i*2 + j + 1;
					break;
				}
			}
			if(key)
			{
				if(key_x == key) key = 0;
				else
				{
					key_staset_LED(x,y,key_x-1,0);
					key_x=key;
					key_staset_LED(x,y,key_x-1,1);
				}
				break;
			}
		}
	}
	
	else if(key_x)
	{
		key_staset_LED(x,y,key_x-1,0);
		key_x = 0;
	}
	
	return key;
}

void Call_Beep()
{
	BEEP_Init();
	delay_init(168);
	
	BEEP = 1;
	delay_ms(20);
	BEEP = 0;
}

 int main(void)
{
	u8 key_menu;
	u8 key_LED0;
	u8 key_temperature_fengshan;
	u8 key_temperature_jiashiqi;
	u8 key_light;
	u8 flag_led = 0;
	u8 flag_temperature = 0;
	u8 flag_light = 0;

  u8 fontok=0; 
	u8 netpro=0;	//网络模式
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存********************************************************************************************************************
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
	usart3_init(115200);  //初始化串口3波特率为115200
	LED_Init();
	LCD_Init();
	KEY_Init();					//按键初始化
	W25QXX_Init();				//初始化W25Q128
	tp_dev.init();
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	fontok=font_init();		//检查字库是否OK
	POINT_COLOR=RED;
  
	while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
	
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑

			LCD_Clear(WHITE);
			POINT_COLOR=RED;
	
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 模式",16,240);
			Show_Str_Mid(0,50,"正在配置ATK-ESP8266模块，请稍等...",12,240);
		
		
	    p=mymalloc(SRAMIN,32);							//申请32字节内存
	    atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	    delay_ms(1000);         //延时3S等待重启成功
	    delay_ms(1000);
	    delay_ms(1000);
	    delay_ms(1000);
	    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	    sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	    while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
PRESTA:
	    netpro|=1; //选择网络模式，TCP客户端为1，此处写死
	
		  if(netpro&0X01)     //TCP Client    透传模式测试
		 {
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
			Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
			if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入****************
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器*********************************************
			while(atk_8266_send_cmd(p,"OK",200))
			{
					LCD_Clear(WHITE);
					POINT_COLOR=RED;
					Show_Str_Mid(0,40,"WK_UP:返回重选",16,240);
					Show_Str(30,80,200,12,"ATK-ESP 连接TCP失败",12,0); //连接失败	 
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
		}
	
	
			LCD_Clear(WHITE);
		  load_ui_menu(2,698);
		
		  LED0 = 1;
	    LED1 = 1;
		
			POINT_COLOR=RED;		
			atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP*****************************************************************************************
			//sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum); //******************************************************************************
		  //atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
			USART3_RX_STA=0;
			while(1)
			{
				
				delay_ms(10);
				
				key=KEY_Scan(0);
				
				key_menu = get_keynum_menu(2,698);
	
				 if(key_menu)	 
				{
					         switch(key_menu)
							{
								case 1 :
									LCD_Clear(BACK_COLOR);
									load_ui_menu(2,698);
									load_ui_temperature(2,430);
									flag_led = 0;
									flag_light = 0;
									flag_temperature = 1;
									Call_Beep();
								  atk_8266_quit_trans();
									atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
									sprintf((char*)p,"THIS IS TEMPERATURE");//测试数据
									Show_Str(30+54,100,200,12,p,12,0);
									u3_printf("%s",p);
									break;
								
								case 2 :
									LCD_Clear(BACK_COLOR);
									load_ui_menu(2,698);
									load_ui_light(2,250);
									flag_led = 0;
									flag_temperature = 0;
									flag_light = 1;
									Call_Beep();
									atk_8266_quit_trans();
									atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
									sprintf((char*)p,"THIS IS LIGHT");//测试数据
									Show_Str(30+54,100,200,12,p,12,0);
									u3_printf("%s",p);
									break;
								
								case 3 :
									LCD_Clear(BACK_COLOR);
									load_ui_menu(2,698);
									load_ui_1(85,140);
									flag_led = 1;
									flag_temperature = 0;
									flag_light = 0;
									Call_Beep();
									atk_8266_quit_trans();
									atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
									sprintf((char*)p,"THIS IS FINGURE");//测试数据
									Show_Str(30+54,100,200,12,p,12,0);
									u3_printf("%s",p);
									break;
								
								case 4 :
									LCD_Clear(BACK_COLOR);
									load_ui_menu(2,698);
									load_ui_2(85,140);
									flag_led = 1;
									flag_temperature = 0;
									flag_light = 0;
									Call_Beep();
									atk_8266_quit_trans();
									atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
									sprintf((char*)p,"THIS IS MY WORLD");//测试数据
									Show_Str(30+54,100,200,12,p,12,0);
									u3_printf("%s",p);
									break;
							}
				}else;
			
				if(timex)timex--;
				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
				t++;
				delay_ms(10);
				if(USART3_RX_STA&0X8000)		//接收到一次数据了
				{ 
					rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
					USART3_RX_BUF[rlen]=0;		//添加结束符 
					printf("%s",USART3_RX_BUF);	//发送到串口    
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
					else t=0;                   //状态为已经连接了,10秒后再检查
				}  
				if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
				{
					constate=atk_8266_consta_check();//得到连接状态
					if(constate=='+')Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
					else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
					t=0;
				}
				
				atk_8266_at_response(1);
				
	 if(flag_led)
	 {
    key_LED0 = get_keynum_LED(85,140);		 
		switch(key_LED0)
		{
			case 1: 
				LED0 = 0 ; 
			  Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LED0 ON!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
			
			case 2: 
				LED1 = 0 ; 
			  Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LED1 ON!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
			
			case 3: 
				LED0 = 1 ;
  			Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LED0 OFF!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
			
			case 4: 
				LED1 = 1 ;
  			Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LED1 OFF!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
		}
	 }
	 
	 if(flag_temperature)
	 {
		 key_temperature_fengshan = get_keynum_temperature_fengshan(2,430);
		 key_temperature_jiashiqi = get_keynum_temperature_jiashiqi(2,430);
		 switch(key_temperature_fengshan)
		 {
			 
			 case 1: 
				 LED0 = 0; 
			   Call_Beep();
			   atk_8266_quit_trans();
				 atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				 sprintf((char*)p,"THIS IS FENGSHAN ON!");//测试数据
				 Show_Str(30+54,100,200,12,p,12,0);
				 u3_printf("%s",p);
			 break;
			 
			 case 2: 
				 LED0 = 1; 
			   Call_Beep();
			   atk_8266_quit_trans();
				 atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				 sprintf((char*)p,"THIS IS FENGSHAN OFF!");//测试数据
			   Show_Str(30+54,100,200,12,p,12,0);
				 u3_printf("%s",p);
			 break;
			 
		 }
		 switch(key_temperature_jiashiqi)
		 {
			 case 1: 
				 LED1 = 0; 
			   Call_Beep();
			   atk_8266_quit_trans();
				 atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				 sprintf((char*)p,"THIS IS JIASHIQI ON!");//测试数据
				 Show_Str(30+54,100,200,12,p,12,0);
				 u3_printf("%s",p);
			 break;
			 
			 case 2: 
				 LED1 = 1; 
			   Call_Beep();
			   atk_8266_quit_trans();
				 atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				 sprintf((char*)p,"THIS IS JIASHIQI OFF!");//测试数据
				 Show_Str(30+54,100,200,12,p,12,0);
				 u3_printf("%s",p);
			 break;
			 
		 }
	 }
	 
	 if(flag_light)
	 {
    key_light = get_keynum_light(2,250);		 
		switch(key_light)
		{
			
			case 1: 
				LED0 = 1 ; 
			  Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LIGHT OFF!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
			
			case 2: 
				LED0 = 0 ; 
			  Call_Beep();
			  atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"THIS IS LIGHT ON!");//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
			break;
			
		}
	 }
	 
			}
	myfree(SRAMIN,p);		//释放内存
	}	  
} 



