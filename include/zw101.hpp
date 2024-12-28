#ifndef zw101_hpp
#define zw101_hpp
#include <Arduino.h>
#include <HardwareSerial.h>
#include <HXCthread.hpp>

extern HardwareSerial mySerial; // 声明 mySerial 为 extern

uint8_t zw101_GetEcho();
uint8_t zw101_PS_HandShake();
uint16_t zw101_checksum(uint8_t *pakcet,size_t len);
uint8_t zw101_sendpack(uint8_t *packet,size_t len);
uint8_t zw101_recivepack();
uint8_t zw101_ControlBLN(uint8_t featuer_code,uint8_t start_color,uint8_t end_color,uint8_t cycle_time);
uint8_t zw101_waitForHandshake(uint32_t timeout);
uint32_t zw101_PS_AutoEnroll(uint16_t ID,uint8_t entry_num,uint16_t parameter);
uint8_t zw101_PS_AutoIdentify(uint8_t rating_fraction,uint16_t ID);//zw101自动验证指令


HardwareSerial mySerial(2); // 创建硬件串口对象

// 计算校验和
uint16_t zw101_checksum(uint8_t *packet, size_t len) {
    uint16_t checksum = 0;
    for (size_t i = 6; i < len - 2; i++) {  // 从第6个字节开始累加
        checksum += packet[i];
    }
    return checksum;
}

// 发送数据包
uint8_t zw101_sendpack(uint8_t *packet, size_t len) {
    size_t sentBytes = 0;
    for (size_t i = 0; i < len; i++) {  
        if (mySerial.write(packet[i]) == 1) {
            sentBytes++;
        } else {
            return 0; // 发送失败
        }
    }
    mySerial.flush(); // 确保数据发送完成
    return (sentBytes == len) ? 1 : 0; // 返回1表示发送成功，0表示部分失败
}

// 接收数据包
uint8_t zw101_receivepack() {
    uint8_t receivedpack[40]; // 用于存储接收到的数据包
    if (mySerial.available() >= 12) { // 确保至少有12字节可读
        for (int i = 0; i < 12; i++) {
            receivedpack[i] = mySerial.read();
            }

            // 校验数据包内容
            if (receivedpack[9] == 0x00&&receivedpack[0] == 0xEF&&receivedpack[1] == 0x01) {
                return 1; // 成功
            } else if (receivedpack[9] == 0x01) {
                return 0; // 失败
            } else {
                return 2; // 未知状态
            }
        }
}

// 指纹模块握手
uint8_t zw101_PS_HandShake() {
    // 初始化串口
    mySerial.begin(57600, SERIAL_8N1, 16, 17); // 确保串口初始化

    uint8_t packet[] = {
        0xEF, 0x01, // 包头
        0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
        0x01,       // 包标识,代表命令包
        0x00, 0x03, // 包长度
        0x35,       // 指令码
        0x00, 0x39  // 校验和（待计算）
    };

    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 等待响应
    uint8_t response = zw101_receivepack();
    return response; // 返回响应结果
}

// 控制背光
uint8_t zw101_ControlBLN(uint8_t featuer_code, uint8_t start_color, uint8_t end_color, uint8_t cycle_time) {
    uint8_t packet[] = {
        0xEF, 0x01, // 包头
        0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
        0x01,       // 包标识,代表命令包
        0x00, 0x07, // 包长度
        0x3C,       // 指令码
        featuer_code,       // 功能码
        start_color, end_color,   // 开始与结束颜色
        cycle_time,       // 设置循环次数为无限循环   
        0x00, 0x00  // 校验和（待计算）
    };

    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 等待响应
    uint8_t response = zw101_receivepack();
    return response;
}

// 自动注册
uint32_t zw101_PS_AutoEnroll(uint16_t ID, uint8_t entry_num, uint16_t parameter) {
    uint8_t packet[] = {
        0xEF, 0x01,              // 包头
        0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
        0x01,                    // 包标识, 代表命令包
        0x00, 0x08,              // 包长度 (确保长度正确)
        0x31,                    // 指令码
        (ID >> 8) & 0xFF, ID & 0xFF,  // ID，高字节和低字节
        entry_num,               // 数据项数量或编号（entry_num）
        (parameter >> 8) & 0xFF, parameter & 0xFF, // 参数的高字节和低字节
        0x00, 0x00               // 校验和（待计算）
    };

    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 等待响应
    uint8_t response = zw101_receivepack();
    return response;
}

// 自动识别
uint8_t zw101_PS_AutoIdentify(uint8_t rating_fraction, uint16_t ID) {
    uint8_t packet[] = {
        0xEF, 0x01,              // 包头
        0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
        0x01,                    // 包标识, 代表命令包
        0x00, 0x08,              // 包长度
        0x32,                    // 指令码
        rating_fraction,         // 分级等级
        (ID >> 8) & 0xFF, ID & 0xFF,  // ID，高字节和低字节
        0x00, 0x05,              // 参数 (parameter)
        0x00, 0x00               // 校验和
    };

    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 等待响应
    uint8_t response = zw101_receivepack();
    return response;
}


#endif
