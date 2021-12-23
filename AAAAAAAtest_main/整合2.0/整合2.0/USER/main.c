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


//ALIENTEK̽����STM32F407������ ��չʵ��5
//ATK-RM04 WIFIģ�����ʵ��   -�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 

#define usart2_baund  57600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
//u8** kbd_tbl;
const  u8* kbd_menu[15]={"ɾָ��"," : ","¼ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter"};//������
const  u8* kbd_delFR[15]={"����"," : ","���ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter"};//������

void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
int press_FR(void);//ˢָ��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//�����������
u8  AS608_get_keynum(u16 x,u16 y);//��ȡ������
u16 GET_NUM(void);//��ȡ��ֵ


int main(void)
{      
  //8266�����õ�
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
	
	
	//ָ��ģ�����õ�
	u8 ensure;
	u8 key_num;
	char *str;
	int aa;
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	BEEP_Init();  			//��ʼ��������
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
	
	while(atk_8266_send_cmd("ATE0","OK",20));//�رջ���
		
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//���ATK-ESP8266ģ�鷢�͹���������,��ʱ�ϴ�������

			LCD_Clear(WHITE);
			POINT_COLOR=RED;
	
					Show_Str_Mid(0,30,"ATK-ESP WIFI-STA ģʽ",16,240);
					Show_Str_Mid(0,50,"��������ATK-ESP8266ģ�飬���Ե�...",12,240);
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
	
		if(netpro&0X01)     //TCP Client    ͸��ģʽ����
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA ����",16,240); 
			Show_Str(30,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);
			if(atk_8266_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP����****************
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������*********************************************
			while(atk_8266_send_cmd(p,"OK",200))
			{
					LCD_Clear(WHITE);
					POINT_COLOR=RED;
					Show_Str_Mid(0,40,"WK_UP:������ѡ",16,240);
					Show_Str(30,80,200,12,"ATK-ESP ����TCPʧ��",12,0); //����ʧ��	 
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
		}
	
	
			LCD_Clear(WHITE);
			POINT_COLOR=RED;		
			atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP*****************************************************************************************
			//sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum); //******************************************************************************
		  //atk_8266_wificonf_show(30,180,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			//Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//����״̬
			USART3_RX_STA=0;
		
		
		  //��ʾָ�Ƶ�ҳ��
			while(font_init()) 			//����ֿ�
			{	    
				LCD_ShowString(60,50,240,16,16,"Font Error!");
				delay_ms(200);				  
				LCD_Fill(60,50,240,66,WHITE);//�����ʾ
				delay_ms(200);	
			}
			if(!(tp_dev.touchtype&0x80))//����ǵ�����
			{
				Show_Str_Mid(0,30,"�Ƿ���д�����У׼",16,240);
				POINT_COLOR=BLUE;
				Show_Str_Mid(0,60,"��:KEY2   ��:KEY0",16,240);	
				while(1)
				{
					key_num=KEY_Scan(0);
					if(key_num==KEY0_PRES)
						break;
					if(key_num==KEY2_PRES)
					{
						LCD_Clear(WHITE);
						TP_Adjust();  	 //��ĻУ׼ 
						TP_Save_Adjdata();//����У׼����
						break;				
					}
				}
			}
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
					aa=press_FR();//ˢָ��
					LCD_Fill(0,100,lcddev.width,160,WHITE);	
					str=mymalloc(SRAMIN,20);
					sprintf(str,"ָ��Ϊ%d",aa);
					Show_Str_Mid(0,140,(u8*)str,16,240);
					myfree(SRAMIN,str);

					if(aa!=0)
					{
							atk_8266_quit_trans();
							atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
							sprintf((char*)p,"����ָ��ID%d\r\n",aa);//��������
							Show_Str(30+54,100,200,12,p,12,0);
							u3_printf("%s",p);
					}else;
					
				}		
				
				
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
				//	sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
				//	LCD_Fill(30+54,115,239,130,WHITE);
				//	POINT_COLOR=BRED;
				//	Show_Str(30+54,115,156,12,p,12,0); 			//��ʾ���յ������ݳ���
				//	POINT_COLOR=BLUE;
				//	LCD_Fill(30,130,239,319,WHITE);
				//	Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
					else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
				}  
				if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
				{
					constate=atk_8266_consta_check();//�õ�����״̬
					if(constate=='+')Show_Str(30+30,80,200,12,"���ӳɹ�",12,0);  //����״̬
					else Show_Str(30+30,80,200,12,"����ʧ��",12,0); 	 
					t=0;
				}
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	}
	
	
}







