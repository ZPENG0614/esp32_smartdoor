#include <Arduino.h>
#include <zw101.h>
#include <ESP32Servo.h>

#define SERVO_PIN 2        // 舵机连接引脚
#define DEGREES 50        // 舵机旋转的最大角度
#define DELAY_TIME 15      // 控制舵机移动的延时

Servo myservo;  // 创建一个舵机对象
uint8_t state;

void setup() {
  Serial.begin(115200);  // 初始化串口，用于调试
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // 初始化软件串口，与指纹模块进行通信
  myservo.attach(SERVO_PIN);  // 将舵机连接到引脚 2
  Serial.println("System initialized.");
}

void loop() {
  state = zw101_PS_AutoIdentify(0x02, 0xFFFF); // 自动识别指纹
  if (state == 1) {
    // 指纹识别成功，执行开门操作
    Serial.println("Fingerprint matched, opening door.");
    
    // 控制舵机从 0 度转到指定角度（例如 50 度）
    for (int pos = 0; pos <= DEGREES; pos++) {
      myservo.write(pos);
      delay(DELAY_TIME);  // 适当延时确保舵机完成移动
    }

    // 舵机从指定角度转回 0 度
    for (int pos = DEGREES; pos >= 0; pos--) {
      myservo.write(pos);
      delay(DELAY_TIME);  // 适当延时确保舵机完成移动
    }

  } else if (state == 2) {
    // 指纹识别失败，指纹不匹配
    Serial.println("Fingerprint mismatch, door not opened.");
  } else if (state == 0) {
    // 指纹识别失败，指纹未检测到
    Serial.println("No fingerprint detected, door not opened.");
  }

  // 输出识别状态（十六进制）
  Serial.print("State: ");
  Serial.println(state, HEX);

  // 延时，避免过于频繁地查询指纹识别
  delay(500);  
}
