/***************************************************************************
* File         : global.h
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/
#ifndef __GLOBAL_H_
#define __GLOBAL_H_

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

// 标准库
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

// Simplelink
#include "simplelink.h"

// 驱动库
#include "hw_types.h" 
#include "hw_ints.h"
#include "hw_apps_rcm.h"
#include "hw_memmap.h"
#include "hw_i2c.h"
#include "hw_common_reg.h"
  
#include "rom.h"
#include "rom_map.h"

#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "pin.h"
#include "gpio.h"
#include "i2c.h"
#include "timer.h"
  
// 接口库
#include "pinmux.h" 
#include "network.h"
#include "gpio_if.h"
#include "uart_if.h"
#include "i2c_if.h"
#include "pwm_if.h"
#include "sensor.h"
#include "transport.h"

// FreeRTOS/TI-RTOS
#include "osi.h"

#define OSI_STACK_SIZE      2048
#define SUCCESS                 0
#define FAILURE                -1

extern void BoardInit(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __GLOBAL_H__