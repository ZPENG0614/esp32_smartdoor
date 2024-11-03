#include <Arduino.h>
#include<zw101.h>

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, 16, 17); // 确保串口初始化
  uint8_t state_test = zw101_PS_HandShake();
  // uint32_t state = zw101_PS_AutoEnroll(0x0001,0x04,0x1D);

  
}

void loop() {
    uint8_t state = zw101_PS_AutoIdentify(0x02,0xFFFF);
  if(state == 1){
    Serial.println("ok");
  }
  else if(state == 2){
    Serial.println("wrong");
  }
  else if(state == 0){
    Serial.println("wrong");
  }
  Serial.println(state,HEX);
}


