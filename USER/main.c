#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "touch.h"	  
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usart2.h"
#include "AS608.h"
#include "timer.h"

//添加wifi模块代码头文件
#include "led.h"
#include "sram.h"
#include "usmart.h"
#include "usart3.h"
#include "common.h"
#include "stdlib.h"

//ALIENTEK 探索者STM32F407开发板 扩展实验
//ATK-AS608指纹识别模块实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

#define usart2_baund  115200//串口2波特率，根据指纹模块波特率更改

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数
u8** kbd_tbl_AS608;
const  u8* kbd_menu[15]={"删指纹"," : ","录指纹","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//按键表
const  u8* kbd_delFR[15]={"返回"," : ","清空指纹","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//按键表

void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
void press_FR(void);//刷指纹
void ShowErrMessage(u8 ensure);//显示确认码错误信息
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//加载虚拟键盘
u8  AS608_get_keynum(u16 x,u16 y);//获取键盘数
u16 GET_NUM(void);//获取数值

int main(void)
{    
	u8 ensure;
	u8 key_num;
	char *str;	
	
	//*********************************************************************
	//8266 add 声明
	
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

	//*********************************************************************

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  	//初始化延时函数
	uart_init(115200);	//初始化串口1波特率为115200，用于支持USMART
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	usart3_init(115200);  //初始化串口3波特率为115200 add
	LED_Init(); //add
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	BEEP_Init();  			//初始化蜂鸣器
	KEY_Init();					//按键初始化 
 	LCD_Init();					//LCD初始化 	
	W25QXX_Init();			//初始化W25Q128
	tp_dev.init();			//初始化触摸屏
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();			    //为fatfs相关变量申请内存  
	
	f_mount(fs[0],"0:",1); 		//挂载SD卡  add
	
 	f_mount(fs[1],"1:",1);  //挂载FLASH.
	
	fontok=font_init();		//检查字库是否OK add
	POINT_COLOR=RED;
	
	/*加载指纹识别实验界面*/
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,0,"AS608指纹识别模块测试程序",16,240);	    			    	 
	Show_Str_Mid(0,20,"正点原子 @ALIENTEK",16,240);				    	 
 	POINT_COLOR=BLUE;
	Show_Str_Mid(0,40,"与AS608模块握手....",16,240);	
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(400);
		LCD_Fill(0,40,240,80,WHITE);
		Show_Str_Mid(0,40,"未检测到模块!!!",16,240);
		delay_ms(800);
		LCD_Fill(0,40,240,80,WHITE);
		Show_Str_Mid(0,40,"尝试连接模块...",16,240);		  
	}
	LCD_Fill(30,40,240,100,WHITE);
	Show_Str_Mid(0,40,"通讯成功!!!",16,240);
	str=mymalloc(SRAMIN,30);
	sprintf(str,"波特率:%d   地址:%x",usart2_baund,AS608Addr);
	Show_Str(0,60,240,16,(u8*)str,16,0);
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息	
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	if(ensure==0x00)
		{
		mymemset(str,0,50);
		sprintf(str,"库容量:%d     对比等级: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		Show_Str(0,80,240,16,(u8*)str,16,0);
		}
	else
		ShowErrMessage(ensure);	
		myfree(SRAMIN,str);
		AS608_load_keyboard(0,170,(u8**)kbd_menu);//加载虚拟键盘
	
	
while(1)
	{
		
		key_num=AS608_get_keynum(0,170);	
		if(key_num)
			{
			if(key_num==1)Del_FR();		//删指纹
			if(key_num==3)Add_FR();		//录指纹									
			}
		if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
			{
			press_FR();//刷指纹			
			}		

	}	
//		while(1)
//	{
//		delay_ms(10); 
//		atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑

//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;
//	
//			Show_Str_Mid(0,600,"ATK-ESP WIFI-STA 模式",16,240);
//			Show_Str_Mid(0,620,"正在配置ATK-ESP8266模块，请稍等...",12,240);
//			//atk_8266_wifista_test();//WIFI STA测试 
//		
//		 	p=mymalloc(SRAMIN,32);							//申请32字节内存
//	
//			atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
//			atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
//			
//			delay_ms(1000);         //延时3S等待重启成功
//			delay_ms(1000);
//			delay_ms(1000);
//			delay_ms(1000);
//		
//			//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
//			sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
//			while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
//PRESTA:
//	netpro|=1; //选择网络模式，TCP客户端为1，此处写死
//	
//	//*******************************************************************
//		if(netpro&0X01)     //TCP Client    透传模式测试
//		{
//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;
//			Show_Str_Mid(0,620,"ATK-ESP WIFI-STA 测试",16,240); 
//			Show_Str(630,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
//			if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入****************
//			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
//			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器*********************************************
//			//****************************************
//			while(atk_8266_send_cmd(p,"OK",200))
//			{
//					LCD_Clear(WHITE);
//					POINT_COLOR=RED;
//					Show_Str_Mid(0,640,"WK_UP:返回重选",16,240);
//					Show_Str(630,80,200,12,"ATK-ESP 连接TCP失败",12,0); //连接失败	 
//					key=KEY_Scan(0);
//					if(key==WKUP_PRES)goto PRESTA;
//			}	
//			//****************************************
//			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
//		}
//		//*****************************************************************
//	
//	
//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;		
//			atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP*****************************************************************************************
//			//sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum); //******************************************************************************
//		  //atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
//			POINT_COLOR=BLUE;
//			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
//			USART3_RX_STA=0;
//			LCD_Fill(10+54,500+115,200+239,500+330,BLUE);
//		//*************************************************
//			while(1)
//			{
//				key=KEY_Scan(0);
//	
//				 if(key==KEY0_PRES)	//KEY0 发送数据 
//				{
//						atk_8266_quit_trans();
//						atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
//						sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
//					//这一行说明蒋上述字符串写在p中
//				  	Show_Str(630+54,100,200,12,p,12,0);//这一行代码说明吧内容p发送到LCD屏幕上去。
//						u3_printf("%s",p);//这一行说明利用串口3把内容p发送到ap
//				}else;
//			
//				if(timex)timex--;
//				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
//				t++;
//				delay_ms(10);
//				if(USART3_RX_STA&0X8000)		//接收到一次数据了
//				{ 
//					LED1 = ~LED1;
//					rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
//					USART3_RX_BUF[rlen]=0;		//添加结束符 
//					printf("%s",USART3_RX_BUF);	//发送到串口   
//					sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
//					LCD_Fill(30+54,500+115,239,500+130,GREEN);
//					POINT_COLOR=BRED;
//					Show_Str(30+54,500+115,156,12,p,12,0); 			//显示接收到的数据长度
//					POINT_COLOR=BLUE;
//					LCD_Fill(30,500+130,239,500+200,YELLOW);
//					Show_Str(30,500+130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
//					USART3_RX_STA=0;
//					if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
//					else t=0;                   //状态为已经连接了,10秒后再检查
//				}  
//				if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
//				{
//					constate=atk_8266_consta_check();//得到连接状态
//					if(constate=='+')Show_Str(30+30,680,200,12,"连接成功",12,0);  //连接状态
//					else Show_Str(30+30,680,200,12,"连接失败",12,0); 	 
//					t=0;
//				}
//				if((t%20)==0)LED0=!LED0;
//				atk_8266_at_response(1);
//			}
//			//***********************************************
//	myfree(SRAMIN,p);		//释放内存
//	}
//	

//} 	



//加载按键界面（尺寸x,y为240*150）
//x,y:界面起始坐标（240*320分辨率的时候，x必须为0）
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl)
{
	u16 i;
	POINT_COLOR=RED;
	kbd_tbl_AS608=kbtbl;
	LCD_Fill(x,y,x+240,y+150,WHITE);
	LCD_DrawRectangle(x,y,x+240,y+150);						   
	LCD_DrawRectangle(x+80,y,x+160,y+150);	 
	LCD_DrawRectangle(x,y+30,x+240,y+60);
	LCD_DrawRectangle(x,y+90,x+240,y+120);
	POINT_COLOR=BLUE;
	for(i=0;i<15;i++)
	{
		if(i==1)//按键表第2个‘:’不需要中间显示
			Show_Str(x+(i%3)*80+2,y+7+30*(i/3),80,30,(u8*)kbd_tbl_AS608[i],16,0);	
		else
			Show_Str_Mid(x+(i%3)*80,y+7+30*(i/3),(u8*)kbd_tbl_AS608[i],16,80);
	} 
}
//按键状态设置
//x,y:键盘坐标
//key:键值(0~14)
//sta:状态，0，松开；1，按下；
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//按键表第2个‘:’不需要清除
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)//不是‘：’
		Show_Str_Mid(x+j*80,y+7+30*i,(u8*)kbd_tbl_AS608[keyx],16,80);	
}
//得到触摸屏的输入
//x,y:键盘坐标
//返回值:（1~15,对应按键表）
u8 AS608_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,没有任何按键按下
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)//触摸屏被按下
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*30+30)&&tp_dev.y[0]>(y+i*30))
				{	
					key=i*3+j+1;	 
					break;	 		   
				}
			}
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					AS608_key_staset(x,y,key_x-1,0);
					key_x=key;
					AS608_key_staset(x,y,key_x-1,1);
				}
				break;
			}
	  }  
	}else if(key_x) 
	{
		AS608_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}
//获取键盘数值
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=AS608_get_keynum(0,170);	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//‘返回’键
			if(key_num==3)return 0xFF00;//		
			if(key_num>3&&key_num<13&&num<99)//‘1-9’键(限制输入3位数)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//‘Del’键			
			if(key_num==14&&num<99)num =num*10;//‘0’键
			if(key_num==15)return num;  //‘Enter’键
		}
		LCD_ShowNum(80+15,170+7,num,6,16);
	}	
}
//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
	LCD_Fill(0,120,lcddev.width,160,WHITE);
	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
}
//录指纹
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"请按指纹",16,240);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"指纹正常",16,240);
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"请按再按一次指纹",16,240);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"指纹正常",16,240);
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"对比两次指纹",16,240);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					LCD_Fill(0,120,lcddev.width,160,WHITE);
					Show_Str_Mid(0,120,"对比成功,两次指纹一样",16,240);
					processnum=3;//跳到第四步
				}
				else 
				{
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"对比失败，请重新录入指纹",16,240);
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				delay_ms(1200);
				break;

			case 3:
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"生成指纹模板",16,240);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					LCD_Fill(0,120,lcddev.width,160,WHITE);
					Show_Str_Mid(0,120,"生成指纹模板成功",16,240);
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"请输入储存ID,按Enter保存",16,240);
				Show_Str_Mid(0,120,"0=< ID <=299",16,240);
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//输入ID必须小于模块容量最大的数值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				if(ensure==0x00) 
				{			
					LCD_Fill(0,100,lcddev.width,160,WHITE);					
					Show_Str_Mid(0,120,"录入指纹成功",16,240);
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					LCD_ShowNum(56,80,AS608Para.PS_max-ValidN,3,16);//显示剩余指纹容量
					delay_ms(1500);
					LCD_Fill(0,100,240,160,WHITE);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(400);
		if(i==5)//超过5次没有按手指则退出
		{
			LCD_Fill(0,100,lcddev.width,160,WHITE);
			break;	
		}				
	}
}

//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{				
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"刷指纹成功",16,240);		
				
				str=mymalloc(SRAMIN,2000);
				sprintf(str,"确有此人,ID:%d  匹配得分:%d",seach.pageID,seach.mathscore);
				Show_Str_Mid(0,140,(u8*)str,16,240);
				myfree(SRAMIN,str);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
	 BEEP=0;//关闭蜂鸣器
	 delay_ms(600);
	 LCD_Fill(0,100,lcddev.width,160,WHITE);
	}
		
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"删除指纹",16,240);
	Show_Str_Mid(0,120,"请输入指纹ID按Enter发送",16,240);
	Show_Str_Mid(0,140,"0=< ID <=299",16,240);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//获取返回的数值
	if(num==0xFFFF)
		goto MENU ; //返回主页面
	else if(num==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(num,1);//删除单个指纹
	if(ensure==0)
	{
		LCD_Fill(0,120,lcddev.width,160,WHITE);
		Show_Str_Mid(0,140,"删除指纹成功",16,240);		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	LCD_ShowNum(56,80,AS608Para.PS_max-ValidN,3,16);
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_menu);
}







