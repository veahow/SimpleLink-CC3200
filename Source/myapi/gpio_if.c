/***************************************************************************
* File         : gpio_if.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"


static unsigned long ulReg[]=
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE
};


unsigned int g_uiLED1Port = 0,g_uiLED2Port = 0,g_uiLED3Port = 0;
unsigned char g_ucLED1Pin,g_ucLED2Pin,g_ucLED3Pin;

#define GPIO_LED1 9     /* ��ɫ */
#define GPIO_LED2 10    /* ��ɫ */
#define GPIO_LED3 11    /* ��ɫ */

static unsigned char
GetPeripheralIntNum(unsigned int uiGPIOPort)
{

    switch(uiGPIOPort)
    {
       case GPIOA0_BASE:
          return INT_GPIOA0;
       case GPIOA1_BASE:
          return INT_GPIOA1;
       case GPIOA2_BASE:
          return INT_GPIOA2;
       case GPIOA3_BASE:
          return INT_GPIOA3;
       default:
          return INT_GPIOA0;
    }

}

/*******************************************************************************
 ��������GPIO_IF_LedConfigure
 ��  ��: ����LED���� 
 ��  ����ucPins ���Ŷ���
 ����ֵ����
*******************************************************************************/
void
GPIO_IF_LedConfigure(unsigned char ucPins)
{
  // ͨ������GPIO_IF_GetPortNPin���ʹ��GPIO�Ķ˿ڻ���ַ������λȨ ����LED����
  if(ucPins & LED1)
  {
    GPIO_IF_GetPortNPin(GPIO_LED1,
                        &g_uiLED1Port,
                        &g_ucLED1Pin);
  }

  if(ucPins & LED2)
  {
    GPIO_IF_GetPortNPin(GPIO_LED2,
                  &g_uiLED2Port,
          &g_ucLED2Pin);
  }

  if(ucPins & LED3)
  {
    GPIO_IF_GetPortNPin(GPIO_LED3,
                      &g_uiLED3Port,
                      &g_ucLED3Pin);

  }

}

/*******************************************************************************
 ��������GPIO_IF_LedOn
 ��  ��: ����LED
 ��  ����ledNum LED��ʶ
 ����ֵ����
*******************************************************************************/
void
GPIO_IF_LedOn(char ledNum)
{
  // ͨ��GPIO_IF_Set����LED
    switch(ledNum)
    {
        case MCU_GREEN_LED_GPIO:
        {
          /* ����ɫLED */
          GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 1);
          break;
        }
        case MCU_ORANGE_LED_GPIO:
        {
          /* ����ɫLED */
          GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 1);
          break;
        }
        case MCU_RED_LED_GPIO:
        {
          /* �򿪺�ɫLED */
          GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 1);
          break;
        }
        case MCU_ALL_LED_IND:
        {
          /* ������LED */
          GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 1);
          GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 1);
          GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 1);
          break;
        }
        default:
          break;
    }
}

/*******************************************************************************
 ��������GPIO_IF_LedOff
 ��  ��: Ϩ��LED
 ��  ����ledNum LED��ʶ
 ����ֵ����
*******************************************************************************/
void
GPIO_IF_LedOff(char ledNum)
{
  // ͨ��GPIO_IF_SetϨ��LED
  switch(ledNum)
  {
    case MCU_GREEN_LED_GPIO:
    {
      /* �ر���ɫLED */
      GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 0);
      break;
    }
    case MCU_ORANGE_LED_GPIO:
    {
      /* �ر���ɫLED */
      GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 0);
      break;
    }
    case MCU_RED_LED_GPIO:
    {
      /* �رպ�ɫED */
      GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 0);
      break;
    }
    case MCU_ALL_LED_IND:
    {
      /* �ر�����LED */
      GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 0);
      GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 0);
      GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 0);
      break;
    }
    default:
      break;
  }
}


/*******************************************************************************
 ��������GPIO_IF_LedStatus
 ��  ��: ��ȡLED״̬
 ��  ����ucGPIONum LED��ʶ
 ����ֵ��ucLEDStatus LED״̬ 1:�� 0:��
*******************************************************************************/
unsigned char
GPIO_IF_LedStatus(unsigned char ucGPIONum)
{
  unsigned char ucLEDStatus;
  switch(ucGPIONum)
  {
    case MCU_GREEN_LED_GPIO:
    {
      ucLEDStatus = GPIO_IF_Get(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin);
      break;
    }
    case MCU_ORANGE_LED_GPIO:
    {
      ucLEDStatus = GPIO_IF_Get(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin);
      break;
    }
    case MCU_RED_LED_GPIO:
    {
      ucLEDStatus = GPIO_IF_Get(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin);
      break;
    }
    default:
        ucLEDStatus = 0;
  }
  return ucLEDStatus;
}

//*****************************************************************************
//
//! Toggle the Led state
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Toggles a board LED
//
//*****************************************************************************
/*******************************************************************************
 ��������GPIO_IF_LedToggle
 ��  ��: LED״̬��ת
 ��  ����ledNum LED��ʶ
 ����ֵ����
*******************************************************************************/
void GPIO_IF_LedToggle(unsigned char ucLedNum)
{

    unsigned char ucLEDStatus = GPIO_IF_LedStatus(ucLedNum);
    if(ucLEDStatus == 1)
    {
        GPIO_IF_LedOff(ucLedNum);
    }
    else
    {
        GPIO_IF_LedOn(ucLedNum);
    }
}

/*******************************************************************************
 ��������GPIO_IF_GetPortNPin
 ��  ��: �����������ƺŻ�ȡ�˿ڻ���ַ������λȨ
 ��  ����ucPins �������ƺ�
         *puiGPIOPort GPIO�˿ڻ���ַָ��
         *pucGPIOPin GPIO����λȨָ��
 ����ֵ����
*******************************************************************************/
void
GPIO_IF_GetPortNPin(unsigned char ucPin,
                unsigned int *puiGPIOPort,
                    unsigned char *pucGPIOPin)
{
    // ���ⲿ���źŻ��GPIO����
    *pucGPIOPin = 1 << (ucPin % 8);
    
    // ���ⲿ���źŻ��GPIO�˿�
    *puiGPIOPort = (ucPin / 8);
    *puiGPIOPort = ulReg[*puiGPIOPort];
}

//****************************************************************************
//
//! Configures the GPIO selected as input to generate interrupt on activity
//!
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//! \param uiIntType is the type of the interrupt (refer gpio.h)
//! \param pfnIntHandler is the interrupt handler to register
//! 
//! This function  
//!    1. Sets GPIO interrupt type
//!    2. Registers Interrupt handler
//!    3. Enables Interrupt
//!
//! \return None
//
//****************************************************************************
void
GPIO_IF_ConfigureNIntEnable(unsigned int uiGPIOPort,
                                  unsigned char ucGPIOPin,
                                  unsigned int uiIntType,
                                  void (*pfnIntHandler)(void))
{
    //
    // Set GPIO interrupt type
    //
    MAP_GPIOIntTypeSet(uiGPIOPort,ucGPIOPin,uiIntType);

    //
    // Register Interrupt handler
    //
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED) 
    // USE_TIRTOS: if app uses TI-RTOS (either networking/non-networking)
    // USE_FREERTOS: if app uses Free-RTOS (either networking/non-networking)
    // SL_PLATFORM_MULTI_THREADED: if app uses any OS + networking(simplelink)
    osi_InterruptRegister(GetPeripheralIntNum(uiGPIOPort),
                                        pfnIntHandler, INT_PRIORITY_LVL_1);
                
#else
	MAP_IntPrioritySet(GetPeripheralIntNum(uiGPIOPort), INT_PRIORITY_LVL_1);
    MAP_GPIOIntRegister(uiGPIOPort,pfnIntHandler);
#endif

    //
    // Enable Interrupt
    //
    MAP_GPIOIntClear(uiGPIOPort,ucGPIOPin);
    MAP_GPIOIntEnable(uiGPIOPort,ucGPIOPin);
}

/*******************************************************************************
 ��������GPIO_IF_Set
 ��  ��: �����������ƺš��˿ڻ���ַ������λȨ����GPIO���ŵ�ֵ
 ��  ����ucPins �������ƺ� GPIO0~31
         uiGPIOPort GPIO�˿ڻ���ַ
         ucGPIOPin GPIO����λȨ
         ucGPIOValue GPIO����ֵ
 ����ֵ����
*******************************************************************************/
void 
GPIO_IF_Set(unsigned char ucPin,
             unsigned int uiGPIOPort,
             unsigned char ucGPIOPin,
             unsigned char ucGPIOValue)
{
    //
    // Set the corresponding bit in the bitmask
    //
    ucGPIOValue = ucGPIOValue << (ucPin % 8);

    // ��GPIO���Ž���д����
    MAP_GPIOPinWrite(uiGPIOPort,ucGPIOPin,ucGPIOValue);
}

//****************************************************************************
//
//! Set a value to the specified GPIO pin
//!
//! \param ucPin is the GPIO pin to be set (0:39)
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//!
//! This function
//!    1. Gets a value of the specified GPIO pin
//!
//! \return value of the GPIO pin
//
//****************************************************************************
unsigned char
GPIO_IF_Get(unsigned char ucPin,
             unsigned int uiGPIOPort,
             unsigned char ucGPIOPin)
{
    unsigned char ucGPIOValue;
    long lGPIOStatus;

    //
    // Invoke the API to Get the value
    //
    lGPIOStatus =  MAP_GPIOPinRead(uiGPIOPort,ucGPIOPin);

    //
    // Set the corresponding bit in the bitmask
    //
    ucGPIOValue = lGPIOStatus >> (ucPin % 8);
    return ucGPIOValue;
}

//*****************************************************************************
//
//! GPIO Interrupt Handler for S2 button
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
/*******************************************************************************
 ��������GPIO_IF_ButtonInit
 ��  ��: �����жϳ�ʼ�� ��ʼ��SW2��SW3����
 ��  ������
 ����ֵ����
*******************************************************************************/
void GPIO_IF_ButtonInit()
{
    // SW2�����жϳ�ʼ�� GPIO22
    GPIOIntTypeSet(GPIOA2_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIOA2_BASE, GPIO_IF_S2IntHandler);
    GPIOIntEnable(GPIOA2_BASE, GPIO_INT_PIN_6);
  
    // SW3�����жϳ�ʼ�� GPIO13
    GPIOIntTypeSet(GPIOA1_BASE, GPIO_PIN_5, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIOA1_BASE, GPIO_IF_S3IntHandler);
    GPIOIntEnable(GPIOA1_BASE, GPIO_INT_PIN_5);
   
}

void GPIO_IF_S2IntHandler()
{
    unsigned long ulPinState =  GPIOIntStatus(GPIOA2_BASE,1);
    if(ulPinState & GPIO_PIN_6)
    {
      GPIO_IF_LedToggle(MCU_RED_LED_GPIO);
//      GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_3,GPIO_PIN_3);
      GPIOIntClear(GPIOA2_BASE,GPIO_INT_PIN_6);
    }
}

void GPIO_IF_S3IntHandler()
{
  unsigned long ulPinState =  GPIOIntStatus(GPIOA1_BASE,1);
    if(ulPinState & GPIO_PIN_5)
    {
      GPIO_IF_LedToggle(MCU_RED_LED_GPIO);
//      GPIOPinWrite(GPIOA1_BASE,GPIO_PIN_3, 0);
      GPIOIntClear(GPIOA1_BASE,GPIO_INT_PIN_5);
    }
}
