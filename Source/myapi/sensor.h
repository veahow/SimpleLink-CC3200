/***************************************************************************
* File         : sensor.h
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/
#ifndef __SENSOR_H__
#define __SENSOR_H__

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

//*****************************************************************************
//                             µÿ÷∑∫Í∂®“Â
//*****************************************************************************
#define TMP_ADD                 0x41
#define BMA_ADD                 0x18
#define SHT2x_ADD               0x40
   

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void SHT2x_Test();
extern int SHT2x_Read_Tempature(unsigned int *temp_int, unsigned int *temp_dec);
extern int SHT2x_Read_Humidity(unsigned int *humi_int, unsigned int *humi_dec);
extern void Board_Test();
extern void TMP_Read(unsigned int *temp_int, unsigned int *temp_dec);
extern void BMA_Read(unsigned char *ax, unsigned char *ay, unsigned char *az);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __SENSOR_H__
