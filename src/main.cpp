#include <Arduino.h>
#include <zw101.hpp>
#include <WiFi.h>
#include <WebServer.h>
#include <servo.hpp>
#include <HXCthread.hpp>
#include <WebService.hpp>
#include <task.hpp>


void setup() {
  myservo.attach(SERVO_PIN);  // 将舵机连接到引脚 2
  Serial.begin(115200);  // 初始化串口，用于调试
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // 初始化软件串口，与指纹模块进行通信
  zw101_thread.start("zw101_thread",4096);
  web_thread.start("web_thread",4096);
}

void loop() {

}
