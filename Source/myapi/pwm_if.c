/***************************************************************************
* File         : pwm_if.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"

// The PWM works based on the following settings:
//     Timer reload interval -> determines the time period of one cycle
//     Timer match value -> determines the duty cycle 
//                          range [0, timer reload interval]
// The computation of the timer reload interval and dutycycle granularity
// is as described below:
// Timer tick frequency = 80 Mhz = 80000000 cycles/sec
// For a time period of 0.5 ms, 
//      Timer reload interval = 80000000/2000 = 40000 cycles
// To support steps of duty cycle update from [0, 255]
//      duty cycle granularity = ceil(40000/255) = 157
// Based on duty cycle granularity,
//      New Timer reload interval = 255*157 = 40035
//      New time period = 0.5004375 ms
//      Timer match value = (update[0, 255] * duty cycle granularity)
//
#define TIMER_INTERVAL_RELOAD   40035 /* =(255*157) */
#define DUTYCYCLE_GRANULARITY   157


//void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer,
//                     unsigned char ucLevel);
//void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
//                       unsigned long ulConfig, unsigned char ucInvert);
//void InitPWMModules();
//void DeInitPWMModules();
//void LedPwmLooper();


/*******************************************************************************
 函数名：UpdateDutyCycle
 功  能: 更新PWM占空比 特定定时器用来操作PWM
 参  数：ulBase:定时器基地址
         ulTimer:定时器选择
         ucLevel:占空比设置0-255
 返回值：无
*******************************************************************************/
void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer,
                     unsigned char ucLevel)
{
    //
    // Match value is updated to reflect the new dutycycle settings
    //
    MAP_TimerMatchSet(ulBase,ulTimer,(ucLevel*DUTYCYCLE_GRANULARITY));
}


/*******************************************************************************
 函数名：SetupTimerPWMMode
 功  能: 设置定时器工作在PWM模式
 参  数：ulBase:定时器基地址
         ulTimer:定时器选择 TIMER_A或TIME_B
         ulConfig:定时器配置
         ucInvert:定时器输出电平
 返回值：无
*******************************************************************************/
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert)
{

    MAP_TimerConfigure(ulBase,ulConfig);        // 配置定时器
    MAP_TimerPrescaleSet(ulBase,ulTimer,0);     // 设置预分频
    
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);     // 控制输出电平 电平翻转

    MAP_TimerLoadSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);     // 设置定时器初值 0.5ms时间
    
    MAP_TimerMatchSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);    // 设置定时器匹配值

}


/*******************************************************************************
 函数名：InitPWMModules
 功  能: PWM初始化
 参  数：无
 返回值：无
*******************************************************************************/
void InitPWMModules()
{
    //
    // Initialization of timers to generate PWM output
    //
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);

    //
    // TIMERA2 (TIMER B) as RED of RGB light. GPIO 9 --> PWM_5
    //
    SetupTimerPWMMode(TIMERA2_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM), 1);
    //
    // TIMERA3 (TIMER B) as YELLOW of RGB light. GPIO 10 --> PWM_6 引脚1
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_A, 
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);
    //
    // TIMERA3 (TIMER A) as GREEN of RGB light. GPIO 11 --> PWM_7 引脚2
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_B, 
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // 允许定时器
    MAP_TimerEnable(TIMERA2_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_B);
}


/*******************************************************************************
 函数名：DeInitPWMModules
 功  能: PWM销毁
 参  数：无
 返回值：无
*******************************************************************************/
void DeInitPWMModules()
{
    // 禁止PWM外设
    MAP_TimerDisable(TIMERA2_BASE, TIMER_B);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_B);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
}

/*******************************************************************************
 函数名：LedPwmLooper
 功  能: 测试LED在PWM模式下循环点亮
 参  数：无
 返回值：无
*******************************************************************************/
void LedPwmLooper()
{
  int iLoopCnt;    // 循环变量
  
  // 不断更新占空比
  for(;;){
        //
        // RYB - Update the duty cycle of the corresponding timers.
        // This changes the brightness of the LEDs appropriately.
        // The timers used are as per LP schematics.
        //
        for(iLoopCnt = 0; iLoopCnt < 255; iLoopCnt++)
        {
//            UpdateDutyCycle(TIMERA2_BASE, TIMER_B, iLoopCnt);
//            UpdateDutyCycle(TIMERA3_BASE, TIMER_B, iLoopCnt);    // 引脚1 黄灯
            UpdateDutyCycle(TIMERA3_BASE, TIMER_A, iLoopCnt);    // 引脚2 绿灯
            MAP_UtilsDelay(800000);     // 延时0.6s
        }
      
    }
}
