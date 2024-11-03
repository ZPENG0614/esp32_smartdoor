#ifndef zw101_h
#define zw101_h
#include <Arduino.h>
#include <HardwareSerial.h>
extern HardwareSerial mySerial; // 声明 mySerial 为 extern

uint8_t zw101_GetEcho();
uint8_t zw101_PS_HandShake();
uint16_t zw101_checksum(uint8_t *pakcet,size_t len);
uint8_t zw101_sendpack(uint8_t *packet,size_t len);
uint8_t zw101_recivepack();
uint8_t zw101_ControlBLN(uint8_t featuer_code,uint8_t start_color,uint8_t end_color,uint8_t cycle_time);
uint8_t zw101_waitForHandshake(uint32_t timeout);
uint32_t zw101_PS_AutoEnroll(uint16_t ID,uint8_t entry_num,uint16_t parameter);
uint8_t zw101_PS_AutoIdentify(uint8_t rating_fraction,uint16_t ID);



#endif
