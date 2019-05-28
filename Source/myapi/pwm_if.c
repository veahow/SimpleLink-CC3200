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
 ��������UpdateDutyCycle
 ��  ��: ����PWMռ�ձ� �ض���ʱ����������PWM
 ��  ����ulBase:��ʱ������ַ
         ulTimer:��ʱ��ѡ��
         ucLevel:ռ�ձ�����0-255
 ����ֵ����
*******************************************************************************/
void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer,
                     unsigned char ucLevel)
{
    //
    // Match value is updated to reflect the new dutycycle settings
    //
    MAP_TimerMatchSet(ulBase,ulTimer,(ucLevel*DUTYCYCLE_GRANULARITY));
}

//****************************************************************************
//
//! Setup the timer in PWM mode
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ulConfig is the timer configuration setting
//! \param ucInvert is to select the inversion of the output
//! 
//! This function  
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
/*******************************************************************************
 ��������SetupTimerPWMMode
 ��  ��: ���ö�ʱ��������PWMģʽ
 ��  ����ulBase:��ʱ������ַ
         ulTimer:��ʱ��ѡ��
         ulConfig:��ʱ������
         ucInvert:��ʱ�������ƽ
 ����ֵ����
*******************************************************************************/
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert)
{
    //
    // Set GPT - Configured Timer in PWM mode.
    //
    MAP_TimerConfigure(ulBase,ulConfig);        // ���ö�ʱ��
    MAP_TimerPrescaleSet(ulBase,ulTimer,0);     // ����Ԥ��Ƶ
    
    //
    // Inverting the timer output if required
    //
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);     // ���������ƽ ��ƽ��ת
    
    //
    // Load value set to ~0.5 ms time period
    //
    MAP_TimerLoadSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);     // ���ö�ʱ����ֵ
    
    //
    // Match value set so as to output level 0
    //
    MAP_TimerMatchSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);    // ���ö�ʱ��ƥ��ֵ

}

//****************************************************************************
//
//! Sets up the identified timers as PWM to drive the peripherals
//!
//! \param none
//! 
//! This function sets up the folowing 
//!    1. TIMERA2 (TIMER B) as RED of RGB light
//!    2. TIMERA3 (TIMER B) as YELLOW of RGB light
//!    3. TIMERA3 (TIMER A) as GREEN of RGB light
//!
//! \return None.
//
//****************************************************************************
/*******************************************************************************
 ��������InitPWMModules
 ��  ��: PWM��ʼ��
 ��  ������
 ����ֵ����
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
    // TIMERA3 (TIMER B) as YELLOW of RGB light. GPIO 10 --> PWM_6
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_A, 
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);
    //
    // TIMERA3 (TIMER A) as GREEN of RGB light. GPIO 11 --> PWM_7
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_B, 
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    // ������ʱ��
    MAP_TimerEnable(TIMERA2_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_B);
}


/*******************************************************************************
 ��������DeInitPWMModules
 ��  ��: PWM����
 ��  ������
 ����ֵ����
*******************************************************************************/
void DeInitPWMModules()
{
    // ��ֹPWM����
    MAP_TimerDisable(TIMERA2_BASE, TIMER_B);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_B);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
}

/*******************************************************************************
 ��������LedPwmLooper
 ��  ��: ����LED��PWMģʽ��ѭ������
 ��  ������
 ����ֵ����
*******************************************************************************/
void LedPwmLooper()
{
  int iLoopCnt;    // ѭ������
  
  // ���ϸ���ռ�ձ�
  for(;;){
        //
        // RYB - Update the duty cycle of the corresponding timers.
        // This changes the brightness of the LEDs appropriately.
        // The timers used are as per LP schematics.
        //
        for(iLoopCnt = 0; iLoopCnt < 255; iLoopCnt++)
        {
            UpdateDutyCycle(TIMERA2_BASE, TIMER_B, iLoopCnt);
            UpdateDutyCycle(TIMERA3_BASE, TIMER_B, iLoopCnt);
            UpdateDutyCycle(TIMERA3_BASE, TIMER_A, iLoopCnt);
            MAP_UtilsDelay(800000);     // ��ʱ0.6s
        }
      
    }
}