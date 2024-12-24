/*这是一个esp32用于创建网页的例程，这里以宿舍门锁系统为例，后面可以移植到其他的例程当中，后续会更新其他的功能*/

#ifndef Web_Service_hpp
#define Web_Service_hpp
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <servo.hpp>

servo_func servo2;

void esp32_ap_setup();   // 创建WiFi并打印设备的MAC地址
void esp32_web_create(); // 创建Web服务，这里用门锁系统为例
void web_callback_func();    // 处理按钮点击的回调函数

// WiFi配置
const char* ssid = "esp32";  // 自定义WiFi名称
const char* password = "11111111";  // 自定义WiFi密码

// 创建Web服务器，端口号为80
WebServer server(80);

void esp32_ap_setup(){
    WiFi.softAP(ssid, password);  // 创建WiFi网络
    Serial.print("MAC Address: ");
    Serial.println(WiFi.softAPmacAddress());  // 打印MAC地址
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());  // 打印WiFi的IP地址
}

void esp32_web_create(){
    server.on("/", [](){
        String html = "<html><head><meta charset='UTF-8'></head><body style='text-align:center'>";
        html += "<h1 style='font-size:50px;'>欢迎来到重庆邮电大学26507的智能门锁系统!</h1>";
        html += "<p style='font-size:40px;'>点击下面的按钮即可开门</p>";
        html += "<button onclick=\"window.location.href='/open'\" style='font-size:60px;'>开门</button>";
        html += "</body></html>";
        server.send(200, "text/html", html);  // 发送网页内容
    });

    server.on("/open", HTTP_GET, web_callback_func);  // 监听 /open 路径并调用 callback_func
    server.begin();  // 启动服务器

    while (1) {  // 持续处理客户端请求,必须在循环里面才能不断地处理客户端的请求
        server.handleClient();
    }
}

void web_callback_func() {
    servo2.servo_opendoor();//开门的控制代码
    // 在这里可以添加开门控制的代码，比如控制继电器或其他硬件
    Serial.println("Door is opened!");  // 这里只是打印信息，实际可以控制硬件

    // 响应客户端的请求
    String html = "<html><head><meta charset='UTF-8'></head><body style='text-align:center'>";
    html += "<h1 style='font-size:50px;'>门已打开！</h1>";
    html += "<a href='/' style='font-size:40px;'>返回主页</a>";
    html += "</body></html>";
    server.send(200, "text/html", html);  // 发送响应内容
}



#endif;
