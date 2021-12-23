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

//���wifiģ�����ͷ�ļ�
#include "led.h"
#include "sram.h"
#include "usmart.h"
#include "usart3.h"
#include "common.h"
#include "stdlib.h"

//ALIENTEK ̽����STM32F407������ ��չʵ��
//ATK-AS608ָ��ʶ��ģ��ʵ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 

#define usart2_baund  115200//����2�����ʣ�����ָ��ģ�鲨���ʸ���

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
u8** kbd_tbl_AS608;
const  u8* kbd_menu[15]={"ɾָ��"," : ","¼ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//������
const  u8* kbd_delFR[15]={"����"," : ","���ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//������

void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
void press_FR(void);//ˢָ��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//�����������
u8  AS608_get_keynum(u16 x,u16 y);//��ȡ������
u16 GET_NUM(void);//��ȡ��ֵ

int main(void)
{    
	u8 ensure;
	u8 key_num;
	char *str;	
	
	//*********************************************************************
	//8266 add ����
	
	u8 fontok=0; 
	u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����********************************************************************************************************************
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬

	//*********************************************************************

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  	//��ʼ����ʱ����
	uart_init(115200);	//��ʼ������1������Ϊ115200������֧��USMART
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	usart3_init(115200);  //��ʼ������3������Ϊ115200 add
	LED_Init(); //add
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	BEEP_Init();  			//��ʼ��������
	KEY_Init();					//������ʼ�� 
 	LCD_Init();					//LCD��ʼ�� 	
	W25QXX_Init();			//��ʼ��W25Q128
	tp_dev.init();			//��ʼ��������
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();			    //Ϊfatfs��ر��������ڴ�  
	
	f_mount(fs[0],"0:",1); 		//����SD��  add
	
 	f_mount(fs[1],"1:",1);  //����FLASH.
	
	fontok=font_init();		//����ֿ��Ƿ�OK add
	POINT_COLOR=RED;
	
	/*����ָ��ʶ��ʵ�����*/
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,0,"AS608ָ��ʶ��ģ����Գ���",16,240);	    			    	 
	Show_Str_Mid(0,20,"����ԭ�� @ALIENTEK",16,240);				    	 
 	POINT_COLOR=BLUE;
	Show_Str_Mid(0,40,"��AS608ģ������....",16,240);	
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		delay_ms(400);
		LCD_Fill(0,40,240,80,WHITE);
		Show_Str_Mid(0,40,"δ��⵽ģ��!!!",16,240);
		delay_ms(800);
		LCD_Fill(0,40,240,80,WHITE);
		Show_Str_Mid(0,40,"��������ģ��...",16,240);		  
	}
	LCD_Fill(30,40,240,100,WHITE);
	Show_Str_Mid(0,40,"ͨѶ�ɹ�!!!",16,240);
	str=mymalloc(SRAMIN,30);
	sprintf(str,"������:%d   ��ַ:%x",usart2_baund,AS608Addr);
	Show_Str(0,60,240,16,(u8*)str,16,0);
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	if(ensure==0x00)
		{
		mymemset(str,0,50);
		sprintf(str,"������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		Show_Str(0,80,240,16,(u8*)str,16,0);
		}
	else
		ShowErrMessage(ensure);	
		myfree(SRAMIN,str);
		AS608_load_keyboard(0,170,(u8**)kbd_menu);//�����������
	
	
while(1)
	{
		
		key_num=AS608_get_keynum(0,170);	
		if(key_num)
			{
			if(key_num==1)Del_FR();		//ɾָ��
			if(key_num==3)Add_FR();		//¼ָ��									
			}
		if(PS_Sta)	 //���PS_Sta״̬���������ָ����
			{
			press_FR();//ˢָ��			
			}		

	}	
//		while(1)
//	{
//		delay_ms(10); 
//		atk_8266_at_response(1);//���ATK-ESP8266ģ�鷢�͹���������,��ʱ�ϴ�������

//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;
//	
//			Show_Str_Mid(0,600,"ATK-ESP WIFI-STA ģʽ",16,240);
//			Show_Str_Mid(0,620,"��������ATK-ESP8266ģ�飬���Ե�...",12,240);
//			//atk_8266_wifista_test();//WIFI STA���� 
//		
//		 	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
//	
//			atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
//			atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
//			
//			delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
//			delay_ms(1000);
//			delay_ms(1000);
//			delay_ms(1000);
//		
//			//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
//			sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
//			while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
//PRESTA:
//	netpro|=1; //ѡ������ģʽ��TCP�ͻ���Ϊ1���˴�д��
//	
//	//*******************************************************************
//		if(netpro&0X01)     //TCP Client    ͸��ģʽ����
//		{
//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;
//			Show_Str_Mid(0,620,"ATK-ESP WIFI-STA ����",16,240); 
//			Show_Str(630,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);
//			if(atk_8266_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP����****************
//			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
//			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������*********************************************
//			//****************************************
//			while(atk_8266_send_cmd(p,"OK",200))
//			{
//					LCD_Clear(WHITE);
//					POINT_COLOR=RED;
//					Show_Str_Mid(0,640,"WK_UP:������ѡ",16,240);
//					Show_Str(630,80,200,12,"ATK-ESP ����TCPʧ��",12,0); //����ʧ��	 
//					key=KEY_Scan(0);
//					if(key==WKUP_PRES)goto PRESTA;
//			}	
//			//****************************************
//			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
//		}
//		//*****************************************************************
//	
//	
//			LCD_Clear(WHITE);
//			POINT_COLOR=RED;		
//			atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP*****************************************************************************************
//			//sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum); //******************************************************************************
//		  //atk_8266_wificonf_show(30,180,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
//			POINT_COLOR=BLUE;
//			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//����״̬
//			USART3_RX_STA=0;
//			LCD_Fill(10+54,500+115,200+239,500+330,BLUE);
//		//*************************************************
//			while(1)
//			{
//				key=KEY_Scan(0);
//	
//				 if(key==KEY0_PRES)	//KEY0 �������� 
//				{
//						atk_8266_quit_trans();
//						atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
//						sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
//					//��һ��˵���������ַ���д��p��
//				  	Show_Str(630+54,100,200,12,p,12,0);//��һ�д���˵��������p���͵�LCD��Ļ��ȥ��
//						u3_printf("%s",p);//��һ��˵�����ô���3������p���͵�ap
//				}else;
//			
//				if(timex)timex--;
//				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
//				t++;
//				delay_ms(10);
//				if(USART3_RX_STA&0X8000)		//���յ�һ��������
//				{ 
//					LED1 = ~LED1;
//					rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
//					USART3_RX_BUF[rlen]=0;		//��ӽ����� 
//					printf("%s",USART3_RX_BUF);	//���͵�����   
//					sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
//					LCD_Fill(30+54,500+115,239,500+130,GREEN);
//					POINT_COLOR=BRED;
//					Show_Str(30+54,500+115,156,12,p,12,0); 			//��ʾ���յ������ݳ���
//					POINT_COLOR=BLUE;
//					LCD_Fill(30,500+130,239,500+200,YELLOW);
//					Show_Str(30,500+130,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
//					USART3_RX_STA=0;
//					if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
//					else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
//				}  
//				if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
//				{
//					constate=atk_8266_consta_check();//�õ�����״̬
//					if(constate=='+')Show_Str(30+30,680,200,12,"���ӳɹ�",12,0);  //����״̬
//					else Show_Str(30+30,680,200,12,"����ʧ��",12,0); 	 
//					t=0;
//				}
//				if((t%20)==0)LED0=!LED0;
//				atk_8266_at_response(1);
//			}
//			//***********************************************
//	myfree(SRAMIN,p);		//�ͷ��ڴ�
//	}
//	

//} 	



//���ذ������棨�ߴ�x,yΪ240*150��
//x,y:������ʼ���꣨240*320�ֱ��ʵ�ʱ��x����Ϊ0��
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
		if(i==1)//�������2����:������Ҫ�м���ʾ
			Show_Str(x+(i%3)*80+2,y+7+30*(i/3),80,30,(u8*)kbd_tbl_AS608[i],16,0);	
		else
			Show_Str_Mid(x+(i%3)*80,y+7+30*(i/3),(u8*)kbd_tbl_AS608[i],16,80);
	} 
}
//����״̬����
//x,y:��������
//key:��ֵ(0~14)
//sta:״̬��0���ɿ���1�����£�
void AS608_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//�������2����:������Ҫ���
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		LCD_Fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)//���ǡ�����
		Show_Str_Mid(x+j*80,y+7+30*i,(u8*)kbd_tbl_AS608[keyx],16,80);	
}
//�õ�������������
//x,y:��������
//����ֵ:��1~15,��Ӧ������
u8 AS608_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,û���κΰ�������
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)//������������
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
//��ȡ������ֵ
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=AS608_get_keynum(0,170);	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//�����ء���
			if(key_num==3)return 0xFF00;//		
			if(key_num>3&&key_num<13&&num<99)//��1-9����(��������3λ��)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//��Del����			
			if(key_num==14&&num<99)num =num*10;//��0����
			if(key_num==15)return num;  //��Enter����
		}
		LCD_ShowNum(80+15,170+7,num,6,16);
	}	
}
//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
	LCD_Fill(0,120,lcddev.width,160,WHITE);
	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
}
//¼ָ��
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
				Show_Str_Mid(0,100,"�밴ָ��",16,240);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"ָ������",16,240);
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"�밴�ٰ�һ��ָ��",16,240);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						LCD_Fill(0,120,lcddev.width,160,WHITE);
						Show_Str_Mid(0,120,"ָ������",16,240);
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"�Ա�����ָ��",16,240);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					LCD_Fill(0,120,lcddev.width,160,WHITE);
					Show_Str_Mid(0,120,"�Աȳɹ�,����ָ��һ��",16,240);
					processnum=3;//�������Ĳ�
				}
				else 
				{
					LCD_Fill(0,100,lcddev.width,160,WHITE);
					Show_Str_Mid(0,100,"�Ա�ʧ�ܣ�������¼��ָ��",16,240);
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				break;

			case 3:
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"����ָ��ģ��",16,240);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					LCD_Fill(0,120,lcddev.width,160,WHITE);
					Show_Str_Mid(0,120,"����ָ��ģ��ɹ�",16,240);
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"�����봢��ID,��Enter����",16,240);
				Show_Str_Mid(0,120,"0=< ID <=299",16,240);
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//����ID����С��ģ������������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					LCD_Fill(0,100,lcddev.width,160,WHITE);					
					Show_Str_Mid(0,120,"¼��ָ�Ƴɹ�",16,240);
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					LCD_ShowNum(56,80,AS608Para.PS_max-ValidN,3,16);//��ʾʣ��ָ������
					delay_ms(1500);
					LCD_Fill(0,100,240,160,WHITE);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(400);
		if(i==5)//����5��û�а���ָ���˳�
		{
			LCD_Fill(0,100,lcddev.width,160,WHITE);
			break;	
		}				
	}
}

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"ˢָ�Ƴɹ�",16,240);		
				
				str=mymalloc(SRAMIN,2000);
				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d",seach.pageID,seach.mathscore);
				Show_Str_Mid(0,140,(u8*)str,16,240);
				myfree(SRAMIN,str);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
	 BEEP=0;//�رշ�����
	 delay_ms(600);
	 LCD_Fill(0,100,lcddev.width,160,WHITE);
	}
		
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"ɾ��ָ��",16,240);
	Show_Str_Mid(0,120,"������ָ��ID��Enter����",16,240);
	Show_Str_Mid(0,140,"0=< ID <=299",16,240);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
		goto MENU ; //������ҳ��
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		LCD_Fill(0,120,lcddev.width,160,WHITE);
		Show_Str_Mid(0,140,"ɾ��ָ�Ƴɹ�",16,240);		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	LCD_ShowNum(56,80,AS608Para.PS_max-ValidN,3,16);
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_menu);
}







