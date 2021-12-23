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
	
	u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����********************************************************************************************************************
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	W25QXX_Init();				//��ʼ��W25Q128
	tp_dev.init();				//��ʼ��������
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	fontok=font_init();		//����ֿ��Ƿ�OK
	POINT_COLOR=RED;
	
	//while(atk_8266_send_cmd("ATE0","OK",20));//�رջ���
	//���д�����ʲô��˼��
	
		
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//���ATK-ESP8266ģ�鷢�͹���������,��ʱ�ϴ�������

			LCD_Clear(WHITE);
			POINT_COLOR=RED;
	
			Show_Str_Mid(0,600,"ATK-ESP WIFI-STA ģʽ",16,240);
			Show_Str_Mid(0,620,"��������ATK-ESP8266ģ�飬���Ե�...",12,240);
			//atk_8266_wifista_test();//WIFI STA���� 
		
		 	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	
			atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
			atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
			
			delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
		
			//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
			sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
			while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
PRESTA:
	netpro|=1; //ѡ������ģʽ��TCP�ͻ���Ϊ1���˴�д��
	
	//*******************************************************************
		if(netpro&0X01)     //TCP Client    ͸��ģʽ����
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,620,"ATK-ESP WIFI-STA ����",16,240); 
			Show_Str(630,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);
			if(atk_8266_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP����****************
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������*********************************************
			//****************************************
			while(atk_8266_send_cmd(p,"OK",200))
			{
					LCD_Clear(WHITE);
					POINT_COLOR=RED;
					Show_Str_Mid(0,640,"WK_UP:������ѡ",16,240);
					Show_Str(630,80,200,12,"ATK-ESP ����TCPʧ��",12,0); //����ʧ��	 
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			//****************************************
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
		}
		//*****************************************************************
	
	
			LCD_Clear(WHITE);
			POINT_COLOR=RED;		
			atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP*****************************************************************************************
			//sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum); //******************************************************************************
		  //atk_8266_wificonf_show(30,180,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//����״̬
			USART3_RX_STA=0;
			LCD_Fill(10+54,500+115,200+239,500+330,BLUE);
		//*************************************************
			while(1)
			{
				key=KEY_Scan(0);
	
				 if(key==KEY0_PRES)	//KEY0 �������� 
				{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
						sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
					//��һ��˵���������ַ���д��p��
				  	Show_Str(630+54,100,200,12,p,12,0);//��һ�д���˵��������p���͵�LCD��Ļ��ȥ��
						u3_printf("%s",p);//��һ��˵�����ô���3������p���͵�ap
				}else;
			
				if(timex)timex--;
				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
				t++;
				delay_ms(10);
				if(USART3_RX_STA&0X8000)		//���յ�һ��������
				{ 
					LED1 = ~LED1;
					rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
					USART3_RX_BUF[rlen]=0;		//��ӽ����� 
					printf("%s",USART3_RX_BUF);	//���͵�����   
					sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
					LCD_Fill(30+54,500+115,239,500+130,GREEN);
					POINT_COLOR=BRED;
					Show_Str(30+54,500+115,156,12,p,12,0); 			//��ʾ���յ������ݳ���
					POINT_COLOR=BLUE;
					LCD_Fill(30,500+130,239,500+200,YELLOW);
					Show_Str(30,500+130,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
					else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
				}  
				if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
				{
					constate=atk_8266_consta_check();//�õ�����״̬
					if(constate=='+')Show_Str(30+30,680,200,12,"���ӳɹ�",12,0);  //����״̬
					else Show_Str(30+30,680,200,12,"����ʧ��",12,0); 	 
					t=0;
				}
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
			//***********************************************
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	}
	

}













