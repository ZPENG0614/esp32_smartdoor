#include <HardwareSerial.h>
#include <zw101.h>

HardwareSerial mySerial(2); // 创建硬件串口对象

uint16_t zw101_checksum(uint8_t *packet, size_t len) {
    uint16_t checksum = 0;
    for (size_t i = 6; i < len - 2; i++) {
        checksum += packet[i];
    }
    // Serial.println(checksum,HEX);
    return checksum;
}



uint8_t zw101_sendpack(uint8_t *packet, size_t len) {
    size_t sentBytes = 0; // 记录已发送的字节数
    for (size_t i = 0; i < len; i++) {  
        if (mySerial.write(packet[i]) == 1) { // 成功发送一个字节
            sentBytes++;
        } else {
            // 如果发送失败，可以在此处添加错误处理
            Serial.println("Failed to send byte");
            return 0; // 返回错误代码
        }
    }
    mySerial.flush(); // 确保数据发送完成
    return (sentBytes == len) ? 1 : 0; // 返回1表示发送成功，0表示部分失败
}


uint8_t zw101_receivepack() {
    uint8_t receivedpack[40]; // 用于存储接收到的数据包
    unsigned long startTime = millis(); // 记录开始时间
    unsigned long timeout = 5000; // 设置超时时间为1000毫秒（1秒）

    while (millis() - startTime < timeout) { // 在超时时间内循环
        if (mySerial.available() >= 12) { // 确保至少有12字节可读
            for (int i = 0; i < 12; i++) {
                receivedpack[i] = mySerial.read();
                Serial.print(receivedpack[i],HEX);
                Serial.print("  ");
                }

            // 校验数据包内容
            if (receivedpack[9] == 0x00) {
                return 1; // 成功
            } else if (receivedpack[9] == 0x01) {
                return 0; // 失败
            } else {
                Serial.print(receivedpack[9]);
                return 2; // 未知状态
            }
        }
    }

    // 超时处理
    Serial.println("Timeout while waiting for data.");
    return 3; // 数据不足或未接收到完整包
}



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

    unsigned long startTime = millis(); // 记录开始时间
    unsigned long timeout = 5000; // 设置超时时间为 5000 毫秒（5秒）
    
    for(int i = 0;i<sizeof(packet);i++){
        Serial.print(packet[i] ,HEX);
        Serial.println("   ");
    }
    Serial.print("\n");
// 检查是否有收到 0x55
    while (1) {
        if (mySerial.available()) {
            uint8_t over = mySerial.read();
            Serial.print("Received: ");
            Serial.println(over, HEX); // 调试信息
            if (over == 0x55) {
            if (zw101_sendpack(packet, sizeof(packet))) {
                Serial.println("Send over");
                break;
            }
        }
    }
        if (millis() - startTime >= timeout) {
        Serial.println("Timeout!"); // 超时处理
        break;
    }
}

    // 接收响应
    uint8_t response = zw101_receivepack(); // 移动到正确的位置
    return response; // 确保 return 在函数内部
}




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

    unsigned long startTime = millis(); // 记录开始时间
    unsigned long timeout = 5000; // 设置超时时间为 5000 毫秒（5秒）
    // 输出调试信息
    for (int i = 0; i < sizeof(packet); i++) {
        Serial.print(packet[i], HEX);
        Serial.print(" ");
    }
        while (1) {
        if (mySerial.available()) {
            uint8_t over = mySerial.read();
            Serial.print("Received: ");
            Serial.println(over, HEX); // 调试信息
            if (over == 0x55) {
            if (zw101_sendpack(packet, sizeof(packet))) {
                Serial.println("Send over");
                break;
            }
        }
    }
        if (millis() - startTime >= timeout) {
        Serial.println("Timeout!"); // 超时处理
        
        break;
    }
}
    // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 接收响应
    uint8_t response = zw101_receivepack(); 

    return response;
}


uint32_t zw101_PS_AutoEnroll(uint16_t ID,uint8_t entry_num,uint16_t parameter){
    
    uint8_t packet[] = {
    0xEF, 0x01,              // 包头
    0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
    0x01,                    // 包标识, 代表命令包
    0x00, 0x08,              // 包长度 (需要确保这个长度正确)
    0x31,                    // 指令码
    (ID >> 8) & 0xFF, ID & 0xFF,  // ID，高字节和低字节
    entry_num,               // 数据项数量或编号（entry_num）
    (parameter >> 8) & 0xFF, parameter & 0xFF, // 参数 (parameter) 的高字节和低字节
    0x00, 0x00               // 校验和（待计算）
};


    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    unsigned long startTime = millis(); // 记录开始时间
    unsigned long timeout = 5000; // 设置超时时间为 5000 毫秒（5秒）
    // 输出调试信息
    for (int i = 0; i < sizeof(packet); i++) {
        Serial.print(packet[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");
//         while (1) {
//         if (mySerial.available()) {
//             uint8_t over = mySerial.read();
//             Serial.print("Received: ");
//             Serial.println(over, HEX); // 调试信息
//             if (over == 0x55) {
//             if (zw101_sendpack(packet, sizeof(packet))) {
//                 Serial.println("Send over");
//                 break;
//             }
//         }
//     }
//         if (millis() - startTime >= timeout) {
//         Serial.println("Timeout!"); // 超时处理
//         break;
//     }
// }

        // 发送数据包
    zw101_sendpack(packet, sizeof(packet));

    // 接收响应
    uint8_t response = zw101_receivepack();

    return response;
}

uint8_t zw101_PS_AutoIdentify(uint8_t rating_fraction,uint16_t ID){

    uint8_t packet[] = {
    0xEF, 0x01,              // 包头
    0xFF, 0xFF, 0xFF, 0xFF,  // 设备默认地址
    0x01,                    // 包标识, 代表命令包
    0x00, 0x08,              // 包长度 (需要确保这个长度正确)
    0x32,                    // 指令码
    rating_fraction,         // 分级等级
     (ID >> 8) & 0xFF, ID & 0xFF,  // ID，高字节和低字节
    0x00,0x05,               // 参数 (parameter) 的高字节和低字节
    0x00, 0x00               // 校验和（待计算）
};


    uint16_t checksum = zw101_checksum(packet, sizeof(packet));
    packet[sizeof(packet) - 2] = (checksum >> 8) & 0xFF; // 高字节
    packet[sizeof(packet) - 1] = checksum & 0xFF;        // 低字节

    unsigned long startTime = millis(); // 记录开始时间
    unsigned long timeout = 5000; // 设置超时时间为 5000 毫秒（5秒）
    // 输出调试信息
    for (int i = 0; i < sizeof(packet); i++) {
        Serial.print(packet[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");
    
//     while (1) {
//     if (mySerial.available()) {
//         uint8_t over = mySerial.read();
//         Serial.print("Received: ");
//         Serial.println(over, HEX); // 调试信息
//         if (over == 0x55) {
//         if (zw101_sendpack(packet, sizeof(packet))) {
//             Serial.println("Send over");
//             break;
//         }
//     }
//     }
//         if (millis() - startTime >= timeout) {
//         Serial.println("Timeout!"); // 超时处理
//         break;
//     }
// }

    zw101_sendpack(packet, sizeof(packet));

    // 接收响应
    uint8_t response = zw101_receivepack();

    return response;
}