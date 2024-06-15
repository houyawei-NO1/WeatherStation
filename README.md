<!--
 * @Author: houyawei-NO1 30817974+houyawei-NO1@users.noreply.github.com
 * @Date: 2024-06-15 11:59:42
 * @LastEditors: houyawei-NO1 30817974+houyawei-NO1@users.noreply.github.com
 * @LastEditTime: 2024-06-15 12:28:38
 * @FilePath: \undefinede:\HouYawei\研发资料\桌面天气指针款\WeatherStation\README.md
 * @Description: 
 * 
 * Copyright (c) 2024 by 侯亚威, All Rights Reserved. 
-->
# WeatherStation
 桌面天气指针款

 目前主要实现了

1.pwm电机控制

2.ESP32配网，天气请求，Wi-Fi信息保存

3.u8g2驱动一个屏幕，绘制一个圆角矩形，中文字体显示，log显示在屏幕下方
（水平不行，前面两个屏幕被我焊坏了，只能驱动最后一个屏幕）

 

附源码，视频，视频展示的功能已验证

 

源码只需要改天气地址和apikey,用的聚合天气接口

String url="http://apis.juhe.cn/simpleWeather/query";    //请求网址响应
String city="南阳";                                      //输入想要获得的城市
String key="填你的，我的每天只能用50次";  

![正面](https://github.com/houyawei-NO1/WeatherStation/blob/main/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20240615112655(1).jpg)

![屏幕](https://github.com/houyawei-NO1/WeatherStation/blob/main/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20240615114943.jpg)

![天气](https://github.com/houyawei-NO1/WeatherStation/blob/main/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20240615115013.jpg)