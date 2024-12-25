#ifndef task_hpp
#define task_hpp
#include <HXCthread.hpp>
#include <servo.hpp>
#include <WebService.hpp>
#include <zw101.hpp>
#include <WebService.hpp>

servo_func myservo_996r;
uint8_t state;

HXC::thread<void> zw101_thread([](){
    while(true){
        state = zw101_PS_AutoIdentify(0x02, 0xFFFF); // 自动识别指纹
        if (state == 1) {
            myservo_996r.servo_opendoor();
        } 
        // 延时，避免过于频繁地查询指纹识别
        delay(500);  
    }
});

//在这里创建一个网页的线程，并且执行相应的函数
HXC::thread<void> web_thread([](){
    esp32_ap_setup();//wifi创建初始化
    esp32_web_create();//创建网页
});









#endif;