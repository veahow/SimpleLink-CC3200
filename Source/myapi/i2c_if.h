/***************************************************************************
* File         : i2c_if.h
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/
#ifndef __I2C_IF_H__
#define __I2C_IF_H__

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

#define I2C_MRIS_CLKTOUT        0x2
//*****************************************************************************
//
// I2C transaction time-out value. 
// Set to value 0x7D. (@100KHz it is 20ms, @400Khz it is 5 ms)
//
//*****************************************************************************
#define I2C_TIMEOUT_VAL         0x7D

//*****************************************************************************
//
// Values that can be passed to I2COpen as the ulMode parameter.
//
//*****************************************************************************
#define I2C_MASTER_MODE_STD     0
#define I2C_MASTER_MODE_FST     1
   

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern int I2C_IF_Open(unsigned long ulMode);
extern int I2C_IF_Close();
extern int I2C_IF_Write(unsigned char ucDevAddr,
             unsigned char *pucData,
             unsigned char ucLen, 
             unsigned char ucStop);
extern int I2C_IF_Read(unsigned char ucDevAddr,
            unsigned char *pucData,
            unsigned char ucLen);
extern int I2C_IF_ReadFrom(unsigned char ucDevAddr,
            unsigned char *pucWrDataBuf,
            unsigned char ucWrLen,
            unsigned char *pucRdDataBuf,
            unsigned char ucRdLen);
extern void SHT2x_Test();
extern void TMP_Read();
extern void BMA_Read();

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __I2C_IF_H__
