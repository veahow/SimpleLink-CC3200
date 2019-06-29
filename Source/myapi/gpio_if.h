/***************************************************************************
* File         : gpio_if.h
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/
#ifndef __GPIOIF_H__
#define __GPIOIF_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    LED1 = 0x1, /* RED LED D7/GP9/Pin64 */
    LED2 = 0x2, /* ORANGE LED D6/GP10/Pin1 */
    LED3 = 0x4  /* GREEN LED D5/GP11/Pin2 */

} ledEnum;

typedef enum
{
    MCU_RED_LED_GPIO,	/* GP09 for LED RED as per LP 3.0 红色LED */
    MCU_ORANGE_LED_GPIO,/* GP10 for LED ORANGE as per LP 3.0 橘色LED */
    MCU_GREEN_LED_GPIO, /* GP11 for LED GREEN as per LP 3.0 绿色LED */
    MCU_ALL_LED_IND     /* 所有LED */
} ledNames;



extern void GPIO_IF_GetPortNPin(unsigned char ucPin,
                     unsigned int *puiGPIOPort,
                     unsigned char *pucGPIOPin);

extern void GPIO_IF_ConfigureNIntEnable(unsigned int uiGPIOPort,
                                  unsigned char ucGPIOPin,
                                  unsigned int uiIntType,
                                  void (*pfnIntHandler)(void));					 
extern void GPIO_IF_Set(unsigned char ucPin,
             unsigned int uiGPIOPort,
             unsigned char ucGPIOPin,
             unsigned char ucGPIOValue);

extern unsigned char GPIO_IF_Get(unsigned char ucPin,
             unsigned int uiGPIOPort,
             unsigned char ucGPIOPin);
extern void GPIO_IF_LedConfigure(unsigned char ucPins);
extern void GPIO_IF_LedOn(char ledNum);
extern void GPIO_IF_LedOff(char ledNum);
extern unsigned char GPIO_IF_LedStatus(unsigned char ucGPIONum);
extern void GPIO_IF_LedToggle(unsigned char ucLedNum);

// 按键SW2 SW3中断函数
extern void GPIO_IF_ButtonInit();
extern void GPIO_IF_S2IntHandler();
extern void GPIO_IF_S3IntHandler();
extern void test();



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __GPIOIF_H__