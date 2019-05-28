/***************************************************************************
* File         : sensor.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"

/*******************************************************************************
 函数名：SHT2x_Test
 功  能：sht2x测试程序 验证自己的猜想 根据需要编写
 参  数：无
 返回值：无
*******************************************************************************/
void SHT2x_Test()
{
    // 温度测量测试
//    I2C_IF_Open(I2C_MASTER_MODE_FST);  
//  
//    unsigned char ucRegOffset;
//    unsigned char aucRdDataBuf[5];
//    unsigned char tem[2];
//    unsigned int temp, temp_int, temp_dec;
//    int iRetVal = 0;
//    int flag = 0;
//    
//    ucRegOffset = 0xf3;    // 非主机模式触发温度测量
//    
//    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // 地址指向SHT20 地址为0x40
//
//    if(iRetVal < 0)
//    {
//        Report("Can not I2C Write!\n");
//        return ;
//    }
//       
//    MAP_UtilsDelay(8000000); // 0.6s
//   
//    iRetVal = I2C_IF_Read(SHT2x_ADD, aucRdDataBuf, 2);
//
//    if(iRetVal < 0)
//    {
//      Report("Can not I2C Read!\n");
//        return ;
//    }
//        
//     
//    tem[0] = aucRdDataBuf[0];
//    tem[1] = aucRdDataBuf[1];
//    
//    
//    
//    // 温度换算公式
//    temp = tem[0];
//    temp = (temp<<8)|(tem[1]&0xfc);
//    temp = temp*17572;
//    temp= (unsigned int)(temp/65536);             //温度计算
//    if(temp>=4685){
//      temp=temp-4685;
//      flag = 0;
//    }
//    else
//    {
//      temp=4685-temp;
//      flag = 1;
//    }
//    
//    
//    
//    // 温度值整数与温度值小数
//    temp_dec = (unsigned int)(temp%100);
//    temp_int = (unsigned int)(temp/100);
//    
//    if(flag==1)
//    {
//       temp_int = temp_int|0x80; 
//    }
//    
//    Report("temp:%d.%d\n", temp_int, temp_dec);
//    
//    I2C_IF_Close();    // 关闭I2C外设
  
  
      // 湿度测量测试
    I2C_IF_Open(I2C_MASTER_MODE_FST);  
  
    unsigned char ucRegOffset;
    unsigned char aucRdDataBuf[5];
    unsigned char hum[2];
    unsigned int humi, humi_int, humi_dec;
    int iRetVal = 0;
    
    ucRegOffset = 0xf5;    // 非主机模式触发湿度测量
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // 地址指向SHT20 地址为0x40

    if(iRetVal < 0)
    {
        Report("SHT2x Humidity can not I2C Write!\n");
        return ;
    }
       
    MAP_UtilsDelay(8000000); // 0.6s
   
    iRetVal = I2C_IF_Read(SHT2x_ADD, aucRdDataBuf, 2);

    if(iRetVal < 0)
    {
        Report("SHT2x Humidity can not I2C Read!\n");
        return ;
    }
     
    hum[0] = aucRdDataBuf[0];
    hum[1] = aucRdDataBuf[1];
    
    // 湿度换算公式
    humi = hum[0];                     //温度计算
    humi = (humi<<8)|(hum[1]&0xf0);
    humi = humi*12500;
    humi = (uint16_t)((uint32_t)(humi/65536));
    
    if(humi>=600) humi = humi-600;
    else humi=0;

    // 湿度值整数与湿度值小数
    humi_int = (unsigned int)(humi/100); 
    humi_dec = (unsigned int)(humi%100);  

    
    Report("Humidity:%d.%d\n", humi_int, humi_dec);
    
    I2C_IF_Close();    // 关闭I2C外设
}

/*******************************************************************************
 函数名：SHT2x_Read_Tempature
 功  能：sht2x读温度
 参  数：*temp_int 温度整数
         *temp_dec 温度小数
 返回值：0 读取正常  -1读取失败
*******************************************************************************/
int SHT2x_Read_Tempature(unsigned int *temp_int, unsigned int *temp_dec)
{
    
    I2C_IF_Open(I2C_MASTER_MODE_FST);  
  
    unsigned char ucRegOffset;
    unsigned char aucRdDataBuf[5];
    unsigned char tem[2];
    unsigned int temp;
    int iRetVal = 0;
    int flag = 0;
    
    ucRegOffset = 0xf3;    // 非主机模式触发温度测量
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // 地址指向SHT20 地址为0x40

    if(iRetVal < 0)
    {
        Report("SHT2x Temperature can not I2C Write!\n");
        return iRetVal;
    }
       
    MAP_UtilsDelay(8000000); // 0.6s
   
    iRetVal = I2C_IF_Read(SHT2x_ADD, aucRdDataBuf, 2);

    if(iRetVal < 0)
    {
      Report("SHT2x Temperature can not I2C Read!\n");
        return iRetVal;
    }
        
     
    tem[0] = aucRdDataBuf[0];
    tem[1] = aucRdDataBuf[1];
    
    // 温度换算公式
    temp = tem[0];
    temp = (temp<<8)|(tem[1]&0xfc);
    temp = temp*17572;
    temp= (unsigned int)(temp/65536);             //温度计算
    if(temp >= 4685){
      temp = temp - 4685;
      flag = 0;
    }
    else
    {
      temp = 4685 - temp;
      flag = 1;
    }
    
    
    
    // 温度值整数与温度值小数
    *temp_dec = (unsigned int)(temp%100);
    *temp_int = (unsigned int)(temp/100);
    
    if(flag==1)
    {
       *temp_int = *temp_int|0x80; 
    }
    
    Report("Temperature:%d.%d \n\r", *temp_int, *temp_dec);
    
    I2C_IF_Close();    // 关闭I2C外设
  
    return iRetVal; 
}

/*******************************************************************************
 函数名：SHT2x_Read_Humidity
 功  能：sht2x读湿度
 参  数：*humi_int 湿度整数值
         *humi_dec 湿度小数值
 返回值：0 读取正常  1读取失败
*******************************************************************************/
int SHT2x_Read_Humidity(unsigned int *humi_int, unsigned int *humi_dec)
{
    I2C_IF_Open(I2C_MASTER_MODE_FST);  
  
    unsigned char ucRegOffset;
    unsigned char aucRdDataBuf[5];
    unsigned char hum[2];
    unsigned int humi;
    int iRetVal = 0;
    
    ucRegOffset = 0xf5;    // 非主机模式触发湿度测量
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // 地址指向SHT20 地址为0x40

    if(iRetVal < 0)
    {
        Report("SHT2x Humidity can not I2C Write!\n");
        return iRetVal;
    }
       
    MAP_UtilsDelay(8000000); // 0.6s
   
    iRetVal = I2C_IF_Read(SHT2x_ADD, aucRdDataBuf, 2);

    if(iRetVal < 0)
    {
        Report("SHT2x Humidity can not I2C Read!\n");
        return iRetVal;
    }
     
    hum[0] = aucRdDataBuf[0];
    hum[1] = aucRdDataBuf[1];
    
    // 湿度换算公式
    humi = hum[0];                     //湿度计算
    humi = (humi<<8)|(hum[1]&0xf0);
    humi = humi*12500;
    humi = (unsigned int)(humi/65536);
    
    if(humi>=600) humi = humi-600;
    else humi=0;

    // 湿度值整数与湿度值小数
    *humi_int = (unsigned int)(humi/100); 
    *humi_dec = (unsigned int)(humi%100);  

    
    Report("Humidity:%d.%d \n\r", *humi_int, *humi_dec);
    
    I2C_IF_Close();    // 关闭I2C外设
    
    return iRetVal;  
}   

// 开发板备用演示程序
/*******************************************************************************
 函数名：TMP_Read
 功  能：读取CC3200 LaunchPad板载温度传感器TMA006 地址0x41
 参  数：无
 返回值：无
*******************************************************************************/
void TMP_Read()
{
  I2C_IF_Open(I2C_MASTER_MODE_FST);
  
  unsigned char ucData[2];
  int iRetVal = -1;
  
  ucData[0] = 1;
  I2C_IF_Write(TMP_ADD, &ucData[0], 1, 0);    // 地址指向TMP006 地址为0x41
  
  for(;;)
  {
    iRetVal = I2C_IF_Read(TMP_ADD, &ucData[0], 2);    // 读取温度值
    if(!iRetVal){
      unsigned short usTemp = (ucData[0] << 8) + ucData[1];
      
      // 处理正温度值
      if(!(usTemp & 0x8000)){
        char str[1400];
        snprintf(str, sizeof(str), "当前温度值是: %d.%02d℃\n\r", usTemp>>7, ((usTemp&0x7f)*25)>>5);    // 显示整数部分 & 显示两位小数
        Report(str);
      }
    } 
    MAP_UtilsDelay(8000000);    // 延时约0.6s
  }

  
  
  // 非循环调试
//  iRetVal = I2C_IF_Read(TMP_ADD, &ucData[0], 2);    // 读取温度值
//  if(!iRetVal){
//      unsigned short usTemp = (ucData[0] << 8) + ucData[1];
//      
//      // 处理正温度值
//      if(!(usTemp & 0x8000)){
//        char str[1400];
//        snprintf(str, sizeof(str), "%d.%02d", usTemp>>7, ((usTemp&0x7f)*25)>>5);
//        Report(str);
////        TcpClientSend(str);
//      }
//      MAP_UtilsDelay(8000000);    // 延时约0.6s
//  } 
//    I2C_IF_Close();
}

/*******************************************************************************
 函数名：BMA_Read
 功  能：读取CC3200 LaunchPad板载加速度传感器BMA222 地址0x18
 参  数：无
 返回值：无
*******************************************************************************/
void BMA_Read()
{
  I2C_IF_Open(I2C_MASTER_MODE_FST);
  
  unsigned char ucData[6];
  int iRetVal = -1;
  
  ucData[0] = 2;
  I2C_IF_Write(BMA_ADD, &ucData[0], 1, 0);    // 地址指向BMA222 地址为0x18
  
  for(;;)
  {
    iRetVal = I2C_IF_Read(BMA_ADD, &ucData[0], 6);    // 读取加速度值
    if(!iRetVal){
      Report("x: %03d  ", ucData[1]);    // 显示x轴加速度
      Report("y: %03d  ", ucData[3]);    // 显示y轴加速度
      Report("z: %03d\n\r", ucData[5]);    // 显示z轴加速度
    }
    MAP_UtilsDelay(8000000);    // 延时约0.6s
  }
  
}