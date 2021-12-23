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

int main(void)
{        
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);  //初始化串口3波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	tp_dev.init();				//初始化触摸屏
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	fontok=font_init();		//检查字库是否OK
	POINT_COLOR=RED;
	
	//while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
	//这行代码是什么意思？
	
		
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑

			LCD_Clear(WHITE);
			POINT_COLOR=RED;
	
			Show_Str_Mid(0,600,"ATK-ESP WIFI-STA 模式",16,240);
			Show_Str_Mid(0,620,"正在配置ATK-ESP8266模块，请稍等...",12,240);
			//atk_8266_wifista_test();//WIFI STA测试 
		
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
	
	//*******************************************************************
		if(netpro&0X01)     //TCP Client    透传模式测试
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,620,"ATK-ESP WIFI-STA 测试",16,240); 
			Show_Str(630,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
			if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入****************
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器*********************************************
			//****************************************
			while(atk_8266_send_cmd(p,"OK",200))
			{
					LCD_Clear(WHITE);
					POINT_COLOR=RED;
					Show_Str_Mid(0,640,"WK_UP:返回重选",16,240);
					Show_Str(630,80,200,12,"ATK-ESP 连接TCP失败",12,0); //连接失败	 
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			//****************************************
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
		}
		//*****************************************************************
	
	
			LCD_Clear(WHITE);
			POINT_COLOR=RED;		
			atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP*****************************************************************************************
			//sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum); //******************************************************************************
		  //atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
			USART3_RX_STA=0;
			LCD_Fill(10+54,500+115,200+239,500+330,BLUE);
		//*************************************************
			while(1)
			{
				key=KEY_Scan(0);
	
				 if(key==KEY0_PRES)	//KEY0 发送数据 
				{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
						sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
					//这一行说明蒋上述字符串写在p中
				  	Show_Str(630+54,100,200,12,p,12,0);//这一行代码说明吧内容p发送到LCD屏幕上去。
						u3_printf("%s",p);//这一行说明利用串口3把内容p发送到ap
				}else;
			
				if(timex)timex--;
				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
				t++;
				delay_ms(10);
				if(USART3_RX_STA&0X8000)		//接收到一次数据了
				{ 
					LED1 = ~LED1;
					rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
					USART3_RX_BUF[rlen]=0;		//添加结束符 
					printf("%s",USART3_RX_BUF);	//发送到串口   
					sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
					LCD_Fill(30+54,500+115,239,500+130,GREEN);
					POINT_COLOR=BRED;
					Show_Str(30+54,500+115,156,12,p,12,0); 			//显示接收到的数据长度
					POINT_COLOR=BLUE;
					LCD_Fill(30,500+130,239,500+200,YELLOW);
					Show_Str(30,500+130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
					else t=0;                   //状态为已经连接了,10秒后再检查
				}  
				if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
				{
					constate=atk_8266_consta_check();//得到连接状态
					if(constate=='+')Show_Str(30+30,680,200,12,"连接成功",12,0);  //连接状态
					else Show_Str(30+30,680,200,12,"连接失败",12,0); 	 
					t=0;
				}
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
			//***********************************************
	myfree(SRAMIN,p);		//释放内存
	}
	

}













