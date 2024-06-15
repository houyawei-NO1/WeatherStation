#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <U8g2lib.h>
#include "time.h"
#include "esp_sntp.h"


#define FREQ        500    // 频率  500（周期1/500）
#define CHANNEL     0       // 通道
#define RESOLUTION  9       // 分辨率
#define LED         48      // LED 引脚
#define PWM1        1       //时间1
#define GPIO2        2       //gpio
#define GPIO3        3        //gpio
#define PWM4        4       //时间4
#define PWM5        5       //温度1
#define GPIO6        6       //gpio
#define GPIO7        7       //gpio
#define PWM8        13      //温度4

String url="http://apis.juhe.cn/simpleWeather/query";    //请求网址响应
String city="南阳";                                      //输入想要获得的城市
String key="添你的，我的每天只能用50次";  
bool is_get; 
bool thirtymins_get;  
int min_s_tmp;
//电机反贴，原引脚1234分别对应4321
 int motor_pwm4[48] =  {354,374,394,400,410,400,394,374,
							354,286,220,184,148,112,82,36,
							511,476,430,400,364,328,292,226,
							158,138,118,112,102,112,118,138,
							158,226,292,328,364,400,430,476,
							2,36,82,112,148,184,220,288,};
char motor_pwm3[48] = {0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,
                      1,1,1,1,1,1,1,1,
                      1,1,1,1,1,1,1,1,
                      1,1,1,1,1,1,1,1,
                      0,0,0,0,0,0,0,0,};
char motor_pwm2[48] = {0,0,0,0,0,0,0,0,
                      1,1,1,1,1,1,1,1,
                      1,1,1,1,1,1,1,1,
                      1,1,1,1,1,1,1,1,
                      0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,};

int motor_pwm1[48] =  {354,286,220,184,148,112,82,36,//1
                      511,476,430,400,364,328,292,226,//2
                      158,138,118,112,102,112,118,138,//3
                      158,226,292,328,364,400,430,476,//4
                      2,36,82,112,148,184,220,288,//5
                      354,374,394,400,410,400,394,374,};//6
int wendu_angle,wendu_angle_temp;
char wendu_counter,wendu_speed_counter,step_wendu_counter;
int hour_angle,hour_angle_temp;
char hour_counter,hour_speed_counter,step_hour_counter;
float max_angle;
// const char *ssid = "NDDZ";
// const char *password = "nd18625625999";
const char *ntpServer1 = "ntp.ntsc.ac.cn";
const char *ntpServer2 = "ntp2.aliyun.com";
const char *ntpServer3 = "ntp3.aliyun.com";
const long gmtOffset_sec = 28800;
const int daylightOffset_sec = 0;
const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
hw_timer_t *timer = NULL;//定义一个定时器对象
U8G2_ST7571_128X128_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 47, /* data=*/ 16, /* cs=*/ 14, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_ST7571_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 14, /* dc=*/ 9, /* reset=*/ 8);
// U8G2_ST7571_128X96_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 47, /* data=*/ 16, /* cs=*/ 14, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_ST7571_128X96_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
String str_time="正在获取时间";
String str_wifi="WIFI未连接";
String str_wifipasswd="";
String str_lineone="正在获取天气";
String str_linetwo="";
String str_linethree="";
String str_linefour="";

void updatelcd(String log)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setFontDirection(0);
  u8g2.setCursor(0, 10);
  u8g2.print(str_time);
  u8g2.setCursor(95, 10);
  u8g2.print(str_wifi);


  u8g2.drawRFrame(0, 12, 128, 72, 8);  // 绘制空心圆角矩形
  u8g2.setFontDirection(0);
  u8g2.setCursor(3, 27);
  u8g2.print(str_lineone);
  u8g2.setCursor(3, 44);
  u8g2.print(str_linetwo);
  u8g2.setCursor(3, 61);
  u8g2.print(str_linethree);
  u8g2.setCursor(3, 78);
  u8g2.print(str_linefour);



  u8g2.setCursor(0, 95);
  u8g2.print(log);
  u8g2.sendBuffer();
}
void SmartConfig()
{ 
  // WiFi.mode(WIFI_AP_STA);
   WiFi.mode(WIFI_STA);
   Serial.println("\r\n wait for smartconfig....");
   updatelcd("请打开一键配网小程序配置WIFI");
   WiFi.beginSmartConfig();
     while(1)
   {
    Serial.print(".");
    delay(500);
    if ( WiFi.smartConfigDone())
    {
      updatelcd("WIFI配置成功");
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n",WiFi.SSID().c_str());
      str_wifi = WiFi.SSID();
      Serial.printf("PSW:%s\r\n",WiFi.psk().c_str());   
      str_wifipasswd = WiFi.psk(); 
      break;      
    }
   } 
}
bool AutoConfig()
{
  WiFi.begin();
  for (int i=0; i<5; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED )  
       {  
          updatelcd("自动连接WIFI成功");
          esp_wifi_set_storage(WIFI_STORAGE_RAM);//自动保存wifi信息
          Serial.println("wifi smartConfig success");
          Serial.printf("SSID:%s",WiFi.SSID().c_str());
          str_wifi = WiFi.SSID();
          Serial.printf(",PWS:%s\r\n",WiFi.psk().c_str());
          str_wifipasswd = WiFi.psk();
          Serial.print("localIP:");
          Serial.println(WiFi.localIP());
          Serial.print(",GateIP:");
          Serial.println(WiFi.gatewayIP()); 
          WiFi.setAutoReconnect(true);  // 设置自动连接        
          return true;
       }
       else
       {
          updatelcd("正在尝试自动连接WIFI");
          Serial.print("WIFI AutoConfig Waiting ....");
          Serial.println(wstatus);
          delay(1000);
       }   
  }
  Serial.println("Wifi autoconfig faild!");
  return false;
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  int hour_s=timeinfo.tm_hour;
  int min_s=timeinfo.tm_min;
  // Serial.println(hour_s);
  hour_angle =  hour_s*7.5*24;

  if(min_s%30==0 && min_s_tmp!=min_s) 
    {
      thirtymins_get=true;
      is_get=false;
    }
  
  min_s_tmp = min_s;
  str_time =  String(timeinfo.tm_mon+1) + "月" + String(timeinfo.tm_mday)+ "日 "
             +String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min)+ ":" +String(timeinfo.tm_sec) ;
  // int hour = timeinfo.tm_hour;
  // Serial.println(str_time);
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void init_stepmotor(void)//上电回零240度
{
   char a=0;
	 int d;
  for(d=0;d<2880*2;d++)
	{
		if(a>=47) a=0;
		else a++;
		ledcWrite(PWM1,motor_pwm1[a] ); 
		digitalWrite(GPIO2, motor_pwm2[a] ); 
		digitalWrite(GPIO3, motor_pwm3[a] ); 
		ledcWrite(PWM4,motor_pwm4[a] ); 

    ledcWrite(PWM5,motor_pwm1[a] ); 
		digitalWrite(GPIO6, motor_pwm2[a] ); 
		digitalWrite(GPIO7, motor_pwm3[a] ); 
		ledcWrite(PWM8,motor_pwm4[a] ); 
    usleep(150);

	}
}

// 定时器中断处理函数
void timer_interrupt()
{
  //修改LED的状态，如果亮则修改为灭；如果灭则修改为亮
// digitalWrite(LED, !digitalRead(LED));
//wendu
int abs_data;
abs_data=abs(wendu_angle-wendu_angle_temp);
if(abs_data!=0)
	{
		wendu_speed_counter++;
    if(abs_data>1440) wendu_counter=1;//60
		else if(abs_data>1200) wendu_counter=1;//50
		else if(abs_data>960) wendu_counter=1;//40
		else if(abs_data>720) wendu_counter=2;//30
		else if(abs_data>480) wendu_counter=3;//20
		else if(abs_data>240) wendu_counter=4;//10
		else if(abs_data>120) wendu_counter=5;//5
		else wendu_counter=6;

		if(wendu_speed_counter>=wendu_counter)
		{
			wendu_speed_counter=0;
			if(step_wendu_counter>47) step_wendu_counter=0;
			if(wendu_angle<wendu_angle_temp)
			{
				if(step_wendu_counter==47) step_wendu_counter=0;
				else step_wendu_counter++;
				wendu_angle_temp--;					
			}
			else
			{
				if(wendu_angle>wendu_angle_temp)
				{
					if(step_wendu_counter==0) step_wendu_counter=47;
					else step_wendu_counter--;
					wendu_angle_temp++;
				}
			}

			    ledcWrite(PWM1,motor_pwm1[step_wendu_counter] ); 
		      digitalWrite(GPIO2, motor_pwm2[step_wendu_counter] ); 
		      digitalWrite(GPIO3, motor_pwm3[step_wendu_counter] ); 
		      ledcWrite(PWM4,motor_pwm4[step_wendu_counter] ); 
		}
	}
	

  //hour
  abs_data=abs(hour_angle-hour_angle_temp);
if(abs_data!=0)
	{
		hour_speed_counter++;
    if(abs_data>1440) hour_counter=1;//60
		else if(abs_data>1200) hour_counter=1;//50
		else if(abs_data>960) hour_counter=1;//40
		else if(abs_data>720) hour_counter=2;//30
		else if(abs_data>480) hour_counter=3;//20
		else if(abs_data>240) hour_counter=4;//10
		else if(abs_data>120) hour_counter=5;//5
		else hour_counter=6;

		if(hour_speed_counter>=hour_counter)
		{
			hour_speed_counter=0;
			if(step_hour_counter>47) step_hour_counter=0;
			if(hour_angle<hour_angle_temp)
			{
				if(step_hour_counter==47) step_hour_counter=0;
				else step_hour_counter++;
				hour_angle_temp--;					
			}
			else
			{
				if(hour_angle>hour_angle_temp)
				{
					if(step_hour_counter==0) step_hour_counter=47;
					else step_hour_counter--;
					hour_angle_temp++;
				}
			}

			    ledcWrite(PWM5,motor_pwm1[step_hour_counter] ); 
		      digitalWrite(GPIO6, motor_pwm2[step_hour_counter] ); 
		      digitalWrite(GPIO7, motor_pwm3[step_hour_counter] ); 
		      ledcWrite(PWM8,motor_pwm4[step_hour_counter] ); 
		}
	}
	
}

void gettemp()
{

//创建HTTPCLient 对象
  HTTPClient http;
  //发送GET请求
  http.begin(url+"?city="+city+"&key="+key);
  int httpCode=http.GET();
  //获取响应状态码
  
  Serial.printf("HTTP 状态码:%d",httpCode);
  //获取响应正文
  String response=http.getString();
  Serial.println("响应数据");
  Serial.println(response);
 
  http.end();

  if(httpCode ==200){
    is_get=true;
    thirtymins_get = false;
  //创建DynamicJsonDocument对象
  DynamicJsonDocument doc(1024);
  //解析JSON数据
  deserializeJson(doc,response);
  //从解析后的JSON文档中获取值
  unsigned int temp=doc["result"]["realtime"]["temperature"].as<unsigned int>();
  String info=doc["result"]["realtime"]["info"].as<String>();
  int aqi=doc["result"]["realtime"]["aqi"].as<int>();
 
  Serial.printf("温度：%d\n",temp);
  Serial.printf("天气：%s\n",info);
  Serial.printf("空气指数：%d\n",aqi);
  str_lineone= city +"  "+ info +"  " + String(temp)+"℃";

  String tm_info = doc["result"]["future"][0]["weather"].as<String>();
  String tm_temp = doc["result"]["future"][0]["temperature"].as<String>();
  str_linetwo = "明日 "+tm_info+" "+tm_temp;

  String three_info = doc["result"]["future"][1]["weather"].as<String>();
  String three_temp = doc["result"]["future"][1]["temperature"].as<String>();
  str_linethree = "后天 "+three_info+" "+three_temp;

  String four_info = doc["result"]["future"][2]["weather"].as<String>();
  String four_temp = doc["result"]["future"][2]["temperature"].as<String>();
  str_linefour = "大后天"+four_info+" "+four_temp;
  if(temp<0)
  wendu_angle = (temp+20)*24;
  else
  wendu_angle = (temp*4.0+20)*24;
  }
  else 
  {
    updatelcd("天气获取失败");
    is_get=false;
  }
}
void setup()
{
  u8g2.begin();
  u8g2.enableUTF8Print();

  is_get =false;
  thirtymins_get=true;
  Serial.begin(115200);
  updatelcd("串口已打开");

  //  esp_wifi_restore();  //删除保存的wifi信息
  if (!AutoConfig())
  {
    SmartConfig();
  }  
  
  // First step is to configure WiFi STA and connect in order to get the current time and date.
  // Serial.printf("Connecting to %s ", ssid);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println(" CONNECTED");
  sntp_set_time_sync_notification_cb(timeavailable);
  esp_sntp_servermode_dhcp(1);  // (optional)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2,ntpServer3);
  updatelcd("NTP校时中");
  pinMode(PWM1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(PWM4, OUTPUT);
  pinMode(PWM5, OUTPUT);
  pinMode(GPIO6, OUTPUT);
  pinMode(GPIO7, OUTPUT);
  pinMode(PWM8, OUTPUT);
  // pinMode(48,OUTPUT);
  // digitalWrite(48, 0); 
  ledcAttach(PWM1,FREQ,RESOLUTION);
  ledcAttach(PWM4,FREQ,RESOLUTION);
  ledcAttach(PWM5,FREQ,RESOLUTION);
  ledcAttach(PWM8,FREQ,RESOLUTION);
  ledcWrite(PWM1, 0); // 输出PWM
  ledcWrite(PWM4, 0); // 输出PWM
  ledcWrite(PWM5, 0); // 输出PWM
  ledcWrite(PWM8, 0); // 输出PWM
  
//   //设置LED引脚（48） 为输出模式
//   pinMode(LED, OUTPUT);
// Set timer frequency to 1Mhz
  timer = timerBegin(1000000);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer,timer_interrupt);
// Set alarm to call onTimer function every 300 usecond (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(timer, 300, true, 0);

  init_stepmotor();
  max_angle =180.00;
  wendu_angle = max_angle*24;
  hour_angle = max_angle*24;
  delay(1500);
  wendu_angle = 0.0*24;
  hour_angle = 0.0*24;
 
  // delay(4000);

}

void loop()
{
  delay(1000);
  printLocalTime();  // it will take some time to sync time :)
  if(!is_get && thirtymins_get)gettemp();
  updatelcd("侯亚威 "+str_wifipasswd);

}
