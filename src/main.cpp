#include <Arduino.h>
#include <zw101.hpp>
#include <WiFi.h>
#include <WebServer.h>
#include <servo.hpp>
#include <HXCthread.hpp>
#include <web.hpp>

uint8_t state;
servo_func servo;

void setup() {
  Serial.begin(115200);  // 初始化串口，用于调试
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // 初始化软件串口，与指纹模块进行通信
  servo.servo_setup();
}

void loop() {
  state = zw101_PS_AutoIdentify(0x02, 0xFFFF); // 自动识别指纹
  if (state == 1) {
    servo.servo_loop();
  } 
  // 延时，避免过于频繁地查询指纹识别
  delay(500);  
}
