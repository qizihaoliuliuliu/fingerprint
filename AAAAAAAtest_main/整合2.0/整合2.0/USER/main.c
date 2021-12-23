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
#include "usart2.h"
#include "AS608.h"
#include "timer.h"
#include "beep.h"


//ALIENTEK探索者STM32F407开发板 扩展实验5
//ATK-RM04 WIFI模块测试实验   -库函数版本 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

#define usart2_baund  57600//串口2波特率，根据指纹模块波特率更改

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数
//u8** kbd_tbl;
const  u8* kbd_menu[15]={"删指纹"," : ","录指纹","1","2","3","4","5","6","7","8","9","DEL","0","Enter"};//按键表
const  u8* kbd_delFR[15]={"返回"," : ","清空指纹","1","2","3","4","5","6","7","8","9","DEL","0","Enter"};//按键表

void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
int press_FR(void);//刷指纹
void ShowErrMessage(u8 ensure);//显示确认码错误信息
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//加载虚拟键盘
u8  AS608_get_keynum(u16 x,u16 y);//获取键盘数
u16 GET_NUM(void);//获取数值


int main(void)
{      
  //8266改配置的
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
	
	
	//指纹模块配置的
	u8 ensure;
	u8 key_num;
	char *str;
	int aa;
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);  //初始化串口3波特率为115200
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	BEEP_Init();  			//初始化蜂鸣器
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
	
	while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
		
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑

			LCD_Clear(WHITE);
			POINT_COLOR=RED;
	
					Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 模式",16,240);
					Show_Str_Mid(0,50,"正在配置ATK-ESP8266模块，请稍等...",12,240);
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
			POINT_COLOR=RED;		
			atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP*****************************************************************************************
			//sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum); //******************************************************************************
		  //atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
			USART3_RX_STA=0;
		
		
		  //显示指纹的页面
			while(font_init()) 			//检查字库
			{	    
				LCD_ShowString(60,50,240,16,16,"Font Error!");
				delay_ms(200);				  
				LCD_Fill(60,50,240,66,WHITE);//清除显示
				delay_ms(200);	
			}
			if(!(tp_dev.touchtype&0x80))//如果是电阻屏
			{
				Show_Str_Mid(0,30,"是否进行触摸屏校准",16,240);
				POINT_COLOR=BLUE;
				Show_Str_Mid(0,60,"是:KEY2   否:KEY0",16,240);	
				while(1)
				{
					key_num=KEY_Scan(0);
					if(key_num==KEY0_PRES)
						break;
					if(key_num==KEY2_PRES)
					{
						LCD_Clear(WHITE);
						TP_Adjust();  	 //屏幕校准 
						TP_Save_Adjdata();//保存校准参数
						break;				
					}
				}
			}
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
					aa=press_FR();//刷指纹
					LCD_Fill(0,100,lcddev.width,160,WHITE);	
					str=mymalloc(SRAMIN,20);
					sprintf(str,"指纹为%d",aa);
					Show_Str_Mid(0,140,(u8*)str,16,240);
					myfree(SRAMIN,str);

					if(aa!=0)
					{
							atk_8266_quit_trans();
							atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
							sprintf((char*)p,"开门指纹ID%d\r\n",aa);//测试数据
							Show_Str(30+54,100,200,12,p,12,0);
							u3_printf("%s",p);
					}else;
					
				}		
				
				
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
				//	sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
				//	LCD_Fill(30+54,115,239,130,WHITE);
				//	POINT_COLOR=BRED;
				//	Show_Str(30+54,115,156,12,p,12,0); 			//显示接收到的数据长度
				//	POINT_COLOR=BLUE;
				//	LCD_Fill(30,130,239,319,WHITE);
				//	Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
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
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
	myfree(SRAMIN,p);		//释放内存
	}
	
	
}







