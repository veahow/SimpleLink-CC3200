/***************************************************************************
* File         : main.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/


#include "global.h"

#define APPLICATION_VERSION     "1.0.0"
#define APP_NAME "Graduation Project"

void main()
{
  long lRetVal = -1;
  
    BoardInit();                // 板子初始化
    PinMuxConfig();             // 引脚配置
    InitTerm();                 // 初始化终端
    ClearTerm();                // 清除终端
    GPIO_IF_LedConfigure(LED1|LED2|LED3);    // 配置LED灯 自己做的板子要修改LED参数 默认9 10 11
    GPIO_IF_ButtonInit();       // 按键中断初始化
    InitPWMModules();    // PWM初始化

    DisplayBanner(APP_NAME);    // 终端显示应用名称    
    
    // 创建SLS任务 开启SimpleLink任务
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    
    
    // 单任务测试
    //test();

    // 创建多任务
    // AP配置
    lRetVal = osi_TaskCreate( ProvisioningAP, \
                                (const signed char*)"Provisioning AP Task", \
                                OSI_STACK_SIZE, NULL, 1, NULL );
    
    // 网络测试
//    lRetVal = osi_TaskCreate( WlanStationMode, \
//                                (const signed char*)"Wlan Station Mode Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
//
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    
    
      // 读取SHT2x温湿度
//        MAP_PinTypeI2C(PIN_01, PIN_MODE_1);
//        MAP_PinTypeI2C(PIN_02, PIN_MODE_1);
//        lRetVal = osi_TaskCreate( SHT2x_Test, \
//                                (const signed char*)"SHT2x Test", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
        
//        lRetVal = osi_TaskCreate( Board_Test, \
//                                (const signed char*)"Board Test", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
      // 读取TMP006温度
//        lRetVal = osi_TaskCreate( TMP_Read, \
//                                (const signed char*)"TMP006 Read", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
      // 读取BMA222加速度
//        lRetVal = osi_TaskCreate( BMA_Read, \
//                                (const signed char*)"BMA222 Read", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
          // Led Pwm闪烁
//        MAP_PinTypeTimer(PIN_01, PIN_MODE_3);
//        MAP_PinTypeTimer(PIN_02, PIN_MODE_3);
//        InitPWMModules();
//        lRetVal = osi_TaskCreate( LedPwmLooper, \
//                                (const signed char*)"Led Pwm", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
//      osi_TaskCreate( UdpLedStatus, \
//                                (const signed char*)"UDP LED Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
//      
//      osi_TaskCreate( UdpTmpStatus, \
//                                (const signed char*)"UDP Temperature Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
//            
//      osi_TaskCreate( UdpHumStatus, \
//                                (const signed char*)"UDP Humidity Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
    lRetVal = osi_TaskCreate( UdpOperation, \
                                (const signed char*)"UDP Operration", \
                                OSI_STACK_SIZE, NULL, 1, NULL );

    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
     
     //开启任务调度
    osi_start();
}