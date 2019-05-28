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
  
    BoardInit();                // ���ӳ�ʼ��
    PinMuxConfig();             // ��������
    InitTerm();                 // ��ʼ���ն�
    ClearTerm();                // ����ն�
    GPIO_IF_LedConfigure(LED1|LED2|LED3);    // ����LED�� �Լ����İ���Ҫ�޸�LED���� Ĭ��9 10 11
    GPIO_IF_LedOff(MCU_ALL_LED_IND);    // �ѵ�ȫ����
    DisplayBanner(APP_NAME);    // �ն���ʾӦ������    
    
    // ����SLS���� ����SimpleLink����
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    
    // ���������
    //test();

    // ����������
    // AP����
//    lRetVal = osi_TaskCreate( ProvisioningAP, \
//                                (const signed char*)"Provisioning AP Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
    // �������
    lRetVal = osi_TaskCreate( WlanStationMode, \
                                (const signed char*)"Wlan Station Mode Task", \
                                OSI_STACK_SIZE, NULL, 1, NULL );
//    WlanStationMode();
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    
//    WlanStationMode();
    
      // ��ȡSHT2x��ʪ��
//        lRetVal = osi_TaskCreate( SHT2x_Test, \
//                                (const signed char*)"SHT2x Test", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
      // ��ȡTMP006�¶�
//        lRetVal = osi_TaskCreate( TMP_Read, \
//                                (const signed char*)"TMP006 Read", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
      // ��ȡBMA222���ٶ�
//        lRetVal = osi_TaskCreate( BMA_Read, \
//                                (const signed char*)"BMA222 Read", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
          // Led Pwm��˸
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
     
    // �����������
    osi_start();
}