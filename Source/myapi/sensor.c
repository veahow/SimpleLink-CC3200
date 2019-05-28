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
 ��������SHT2x_Test
 ��  �ܣ�sht2x���Գ��� ��֤�Լ��Ĳ��� ������Ҫ��д
 ��  ������
 ����ֵ����
*******************************************************************************/
void SHT2x_Test()
{
    // �¶Ȳ�������
//    I2C_IF_Open(I2C_MASTER_MODE_FST);  
//  
//    unsigned char ucRegOffset;
//    unsigned char aucRdDataBuf[5];
//    unsigned char tem[2];
//    unsigned int temp, temp_int, temp_dec;
//    int iRetVal = 0;
//    int flag = 0;
//    
//    ucRegOffset = 0xf3;    // ������ģʽ�����¶Ȳ���
//    
//    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // ��ַָ��SHT20 ��ַΪ0x40
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
//    // �¶Ȼ��㹫ʽ
//    temp = tem[0];
//    temp = (temp<<8)|(tem[1]&0xfc);
//    temp = temp*17572;
//    temp= (unsigned int)(temp/65536);             //�¶ȼ���
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
//    // �¶�ֵ�������¶�ֵС��
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
//    I2C_IF_Close();    // �ر�I2C����
  
  
      // ʪ�Ȳ�������
    I2C_IF_Open(I2C_MASTER_MODE_FST);  
  
    unsigned char ucRegOffset;
    unsigned char aucRdDataBuf[5];
    unsigned char hum[2];
    unsigned int humi, humi_int, humi_dec;
    int iRetVal = 0;
    
    ucRegOffset = 0xf5;    // ������ģʽ����ʪ�Ȳ���
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // ��ַָ��SHT20 ��ַΪ0x40

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
    
    // ʪ�Ȼ��㹫ʽ
    humi = hum[0];                     //�¶ȼ���
    humi = (humi<<8)|(hum[1]&0xf0);
    humi = humi*12500;
    humi = (uint16_t)((uint32_t)(humi/65536));
    
    if(humi>=600) humi = humi-600;
    else humi=0;

    // ʪ��ֵ������ʪ��ֵС��
    humi_int = (unsigned int)(humi/100); 
    humi_dec = (unsigned int)(humi%100);  

    
    Report("Humidity:%d.%d\n", humi_int, humi_dec);
    
    I2C_IF_Close();    // �ر�I2C����
}

/*******************************************************************************
 ��������SHT2x_Read_Tempature
 ��  �ܣ�sht2x���¶�
 ��  ����*temp_int �¶�����
         *temp_dec �¶�С��
 ����ֵ��0 ��ȡ����  -1��ȡʧ��
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
    
    ucRegOffset = 0xf3;    // ������ģʽ�����¶Ȳ���
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // ��ַָ��SHT20 ��ַΪ0x40

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
    
    // �¶Ȼ��㹫ʽ
    temp = tem[0];
    temp = (temp<<8)|(tem[1]&0xfc);
    temp = temp*17572;
    temp= (unsigned int)(temp/65536);             //�¶ȼ���
    if(temp >= 4685){
      temp = temp - 4685;
      flag = 0;
    }
    else
    {
      temp = 4685 - temp;
      flag = 1;
    }
    
    
    
    // �¶�ֵ�������¶�ֵС��
    *temp_dec = (unsigned int)(temp%100);
    *temp_int = (unsigned int)(temp/100);
    
    if(flag==1)
    {
       *temp_int = *temp_int|0x80; 
    }
    
    Report("Temperature:%d.%d \n\r", *temp_int, *temp_dec);
    
    I2C_IF_Close();    // �ر�I2C����
  
    return iRetVal; 
}

/*******************************************************************************
 ��������SHT2x_Read_Humidity
 ��  �ܣ�sht2x��ʪ��
 ��  ����*humi_int ʪ������ֵ
         *humi_dec ʪ��С��ֵ
 ����ֵ��0 ��ȡ����  1��ȡʧ��
*******************************************************************************/
int SHT2x_Read_Humidity(unsigned int *humi_int, unsigned int *humi_dec)
{
    I2C_IF_Open(I2C_MASTER_MODE_FST);  
  
    unsigned char ucRegOffset;
    unsigned char aucRdDataBuf[5];
    unsigned char hum[2];
    unsigned int humi;
    int iRetVal = 0;
    
    ucRegOffset = 0xf5;    // ������ģʽ����ʪ�Ȳ���
    
    iRetVal = I2C_IF_Write(SHT2x_ADD, &ucRegOffset, 1, 0);    // ��ַָ��SHT20 ��ַΪ0x40

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
    
    // ʪ�Ȼ��㹫ʽ
    humi = hum[0];                     //ʪ�ȼ���
    humi = (humi<<8)|(hum[1]&0xf0);
    humi = humi*12500;
    humi = (unsigned int)(humi/65536);
    
    if(humi>=600) humi = humi-600;
    else humi=0;

    // ʪ��ֵ������ʪ��ֵС��
    *humi_int = (unsigned int)(humi/100); 
    *humi_dec = (unsigned int)(humi%100);  

    
    Report("Humidity:%d.%d \n\r", *humi_int, *humi_dec);
    
    I2C_IF_Close();    // �ر�I2C����
    
    return iRetVal;  
}   

// �����屸����ʾ����
/*******************************************************************************
 ��������TMP_Read
 ��  �ܣ���ȡCC3200 LaunchPad�����¶ȴ�����TMA006 ��ַ0x41
 ��  ������
 ����ֵ����
*******************************************************************************/
void TMP_Read()
{
  I2C_IF_Open(I2C_MASTER_MODE_FST);
  
  unsigned char ucData[2];
  int iRetVal = -1;
  
  ucData[0] = 1;
  I2C_IF_Write(TMP_ADD, &ucData[0], 1, 0);    // ��ַָ��TMP006 ��ַΪ0x41
  
  for(;;)
  {
    iRetVal = I2C_IF_Read(TMP_ADD, &ucData[0], 2);    // ��ȡ�¶�ֵ
    if(!iRetVal){
      unsigned short usTemp = (ucData[0] << 8) + ucData[1];
      
      // �������¶�ֵ
      if(!(usTemp & 0x8000)){
        char str[1400];
        snprintf(str, sizeof(str), "��ǰ�¶�ֵ��: %d.%02d��\n\r", usTemp>>7, ((usTemp&0x7f)*25)>>5);    // ��ʾ�������� & ��ʾ��λС��
        Report(str);
      }
    } 
    MAP_UtilsDelay(8000000);    // ��ʱԼ0.6s
  }

  
  
  // ��ѭ������
//  iRetVal = I2C_IF_Read(TMP_ADD, &ucData[0], 2);    // ��ȡ�¶�ֵ
//  if(!iRetVal){
//      unsigned short usTemp = (ucData[0] << 8) + ucData[1];
//      
//      // �������¶�ֵ
//      if(!(usTemp & 0x8000)){
//        char str[1400];
//        snprintf(str, sizeof(str), "%d.%02d", usTemp>>7, ((usTemp&0x7f)*25)>>5);
//        Report(str);
////        TcpClientSend(str);
//      }
//      MAP_UtilsDelay(8000000);    // ��ʱԼ0.6s
//  } 
//    I2C_IF_Close();
}

/*******************************************************************************
 ��������BMA_Read
 ��  �ܣ���ȡCC3200 LaunchPad���ؼ��ٶȴ�����BMA222 ��ַ0x18
 ��  ������
 ����ֵ����
*******************************************************************************/
void BMA_Read()
{
  I2C_IF_Open(I2C_MASTER_MODE_FST);
  
  unsigned char ucData[6];
  int iRetVal = -1;
  
  ucData[0] = 2;
  I2C_IF_Write(BMA_ADD, &ucData[0], 1, 0);    // ��ַָ��BMA222 ��ַΪ0x18
  
  for(;;)
  {
    iRetVal = I2C_IF_Read(BMA_ADD, &ucData[0], 6);    // ��ȡ���ٶ�ֵ
    if(!iRetVal){
      Report("x: %03d  ", ucData[1]);    // ��ʾx����ٶ�
      Report("y: %03d  ", ucData[3]);    // ��ʾy����ٶ�
      Report("z: %03d\n\r", ucData[5]);    // ��ʾz����ٶ�
    }
    MAP_UtilsDelay(8000000);    // ��ʱԼ0.6s
  }
  
}