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

#define GPIO_LED1 9     /* 红色 */
#define GPIO_LED2 10    /* 橘色 */
#define GPIO_LED3 11    /* 绿色 */

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
 函数名：GPIO_IF_LedConfigure
 功  能: 配置LED函数 
 参  数：ucPins 引脚定义
 返回值：无
*******************************************************************************/
void
GPIO_IF_LedConfigure(unsigned char ucPins)
{
  // 通过调用GPIO_IF_GetPortNPin获得使用GPIO的端口基地址和引脚位权 用于LED操作
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
 函数名：GPIO_IF_LedOn
 功  能: 点亮LED
 参  数：ledNum LED标识
 返回值：无
*******************************************************************************/
void
GPIO_IF_LedOn(char ledNum)
{
  // 通过GPIO_IF_Set点亮LED
    switch(ledNum)
    {
        case MCU_GREEN_LED_GPIO:
        {
          /* 打开绿色LED */
          GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 1);
          break;
        }
        case MCU_ORANGE_LED_GPIO:
        {
          /* 打开橘色LED */
          GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 1);
          break;
        }
        case MCU_RED_LED_GPIO:
        {
          /* 打开红色LED */
          GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 1);
          break;
        }
        case MCU_ALL_LED_IND:
        {
          /* 打开所有LED */
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
 函数名：GPIO_IF_LedOff
 功  能: 熄灭LED
 参  数：ledNum LED标识
 返回值：无
*******************************************************************************/
void
GPIO_IF_LedOff(char ledNum)
{
  // 通过GPIO_IF_Set熄灭LED
  switch(ledNum)
  {
    case MCU_GREEN_LED_GPIO:
    {
      /* 关闭绿色LED */
      GPIO_IF_Set(GPIO_LED3, g_uiLED3Port, g_ucLED3Pin, 0);
      break;
    }
    case MCU_ORANGE_LED_GPIO:
    {
      /* 关闭橘色LED */
      GPIO_IF_Set(GPIO_LED2, g_uiLED2Port, g_ucLED2Pin, 0);
      break;
    }
    case MCU_RED_LED_GPIO:
    {
      /* 关闭红色ED */
      GPIO_IF_Set(GPIO_LED1, g_uiLED1Port, g_ucLED1Pin, 0);
      break;
    }
    case MCU_ALL_LED_IND:
    {
      /* 关闭所有LED */
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
 函数名：GPIO_IF_LedStatus
 功  能: 获取LED状态
 参  数：ucGPIONum LED标识
 返回值：ucLEDStatus LED状态 1:开 0:关
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
 函数名：GPIO_IF_LedToggle
 功  能: LED状态反转
 参  数：ledNum LED标识
 返回值：无
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
 函数名：GPIO_IF_GetPortNPin
 功  能: 根据引脚名称号获取端口基地址和引脚位权
 参  数：ucPins 引脚名称号
         *puiGPIOPort GPIO端口基地址指针
         *pucGPIOPin GPIO引脚位权指针
 返回值：无
*******************************************************************************/
void
GPIO_IF_GetPortNPin(unsigned char ucPin,
                unsigned int *puiGPIOPort,
                    unsigned char *pucGPIOPin)
{
    // 从外部引脚号获得GPIO引脚
    *pucGPIOPin = 1 << (ucPin % 8);
    
    // 从外部引脚号获得GPIO端口
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
 函数名：GPIO_IF_Set
 功  能: 根据引脚名称号、端口基地址和引脚位权设置GPIO引脚的值
 参  数：ucPins 引脚名称号 GPIO0~31
         uiGPIOPort GPIO端口基地址
         ucGPIOPin GPIO引脚位权
         ucGPIOValue GPIO引脚值
 返回值：无
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

    // 对GPIO引脚进行写操作
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
 函数名：GPIO_IF_ButtonInit
 功  能: 按键中断初始化 初始化SW2和SW3按键
 参  数：无
 返回值：无
*******************************************************************************/
void GPIO_IF_ButtonInit()
{
    // SW2按键中断初始化 GPIO22
    GPIOIntTypeSet(GPIOA2_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIOA2_BASE, GPIO_IF_S2IntHandler);
    GPIOIntEnable(GPIOA2_BASE, GPIO_INT_PIN_6);
  
    // SW3按键中断初始化 GPIO13
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
